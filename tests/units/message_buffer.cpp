#include <boost/test/unit_test.hpp>

#include <graphite_proxy/models/message.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <vector>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( buffer_get_items )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );

  BOOST_CHECK_EQUAL( buffer->add( boost::make_shared<Message>("test.1", 10, 40) ), true );
  BOOST_CHECK_EQUAL( buffer->add( boost::make_shared<Message>("test.1", 40, 60) ), true );

  // Get all messages of type "test.1"
  std::vector<message_ptr> messages;
  buffer->get( "test.1", messages );

  BOOST_CHECK_EQUAL( messages.size(), 2 );
  BOOST_CHECK_EQUAL( messages[0]->getValue(), 10 ); BOOST_CHECK_EQUAL( messages[0]->getTimestamp(), 40 );
  BOOST_CHECK_EQUAL( messages[1]->getValue(), 40 ); BOOST_CHECK_EQUAL( messages[1]->getTimestamp(), 60 );

  messages.clear();
  BOOST_CHECK_EQUAL( messages.size(), 0 );
}

BOOST_AUTO_TEST_CASE( message_buffer_get_nbr_of_items )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  MessageBuffer buffer( "buffer name", 3, true );

  BOOST_CHECK_EQUAL( buffer.getName(), "buffer name" );
  BOOST_CHECK_EQUAL( buffer.empty(), true );
  BOOST_CHECK_EQUAL( buffer.size(), 0 );

  buffer.add( boost::make_shared<Message>( "type.1", 0.0 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 1.0 ) );

  BOOST_CHECK_EQUAL( buffer.empty(), false );
  BOOST_CHECK_EQUAL( buffer.size(), 2 );

  std::vector<message_ptr> messages;
  buffer.get( messages, 1 ); // Retrieve only one message

  BOOST_CHECK_EQUAL( buffer.empty(), false );
  BOOST_CHECK_EQUAL( buffer.size(), 1 );
  BOOST_CHECK_EQUAL( messages.size(), 1 );

  messages.clear();
  buffer.get( messages, 1 );

  BOOST_CHECK_EQUAL( buffer.empty(), true );
  BOOST_CHECK_EQUAL( buffer.size(), 0 );
  BOOST_CHECK_EQUAL( messages.size(), 1 );
}

BOOST_AUTO_TEST_CASE( message_buffer_get_older_than )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  MessageBuffer buffer( "buffer name", 5, true );

  buffer.add( boost::make_shared<Message>( "type.1", 0, 10 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 1, 20 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 2, 30 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 3, 40 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 4, 50 ) );

  std::vector<message_ptr> messages;

  // Nothing to take
  buffer.getOlderThan( messages, 0 );
  BOOST_CHECK_EQUAL( messages.empty(), true );
  BOOST_CHECK_EQUAL( buffer.size(), 5 );

  // Take all
  buffer.getOlderThan( messages, 50 );
  BOOST_CHECK_EQUAL( messages.size(), 5 );
  BOOST_CHECK_EQUAL( buffer.size(), 0 );
  BOOST_CHECK_EQUAL( messages[0]->getTimestamp(), 10 );
  BOOST_CHECK_EQUAL( messages[1]->getTimestamp(), 20 );
  BOOST_CHECK_EQUAL( messages[2]->getTimestamp(), 30 );
  BOOST_CHECK_EQUAL( messages[3]->getTimestamp(), 40 );
  BOOST_CHECK_EQUAL( messages[4]->getTimestamp(), 50 );

  buffer.add( boost::make_shared<Message>( "type.1", 4, 50 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 0, 10 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 2, 30 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 1, 20 ) );
  buffer.add( boost::make_shared<Message>( "type.1", 3, 40 ) );

  messages.clear();

  // Take from unordered list
  buffer.getOlderThan( messages, 20 );
  BOOST_CHECK_EQUAL( messages.size(), 2 );
  BOOST_CHECK_EQUAL( buffer.size(), 3 );
  BOOST_CHECK_EQUAL( messages[0]->getTimestamp(), 10 );
  BOOST_CHECK_EQUAL( messages[1]->getTimestamp(), 20 );

  messages.clear();

  // Take what remains
  buffer.getOlderThan( messages, 100 );
  BOOST_CHECK_EQUAL( messages.size(), 3 );
  BOOST_CHECK_EQUAL( buffer.size(), 0 );
  BOOST_CHECK_EQUAL( messages[0]->getTimestamp(), 50 );
  BOOST_CHECK_EQUAL( messages[1]->getTimestamp(), 30 );
  BOOST_CHECK_EQUAL( messages[2]->getTimestamp(), 40 );
}

BOOST_AUTO_TEST_CASE( buffer_override_oldest )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );

  buffer->add( boost::make_shared<Message>("test.2", 10, 40) );
  buffer->add( boost::make_shared<Message>("test.2", 40, 60) );
  buffer->add( boost::make_shared<Message>("test.2", 50, 70) );
  buffer->add( boost::make_shared<Message>("test.2", 99, 80) );

  std::vector<message_ptr> messages;
  buffer->get( "test.2", messages );

  BOOST_CHECK_EQUAL( messages.size(), 3 );
  BOOST_CHECK_EQUAL( messages[0]->getValue(), 40 );
  BOOST_CHECK_EQUAL( messages[1]->getValue(), 50 );
  BOOST_CHECK_EQUAL( messages[2]->getValue(), 99 );
}

BOOST_AUTO_TEST_CASE( buffer_drop_new )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, false, client );

  buffer->add( boost::make_shared<Message>("test.3", 10, 40) );
  buffer->add( boost::make_shared<Message>("test.3", 40, 60) );
  buffer->add( boost::make_shared<Message>("test.3", 50, 70) );
  buffer->add( boost::make_shared<Message>("test.3", 99, 80) ); // This message has to be dropped (no more free space available and drop_oldest == false)

  std::vector<message_ptr> messages;
  buffer->get( "test.3", messages );

  BOOST_CHECK_EQUAL( messages.size(), 3 );
  BOOST_CHECK_EQUAL( messages[0]->getValue(), 10 ); BOOST_CHECK_EQUAL( messages[0]->getTimestamp(), 40 );
  BOOST_CHECK_EQUAL( messages[1]->getValue(), 40 ); BOOST_CHECK_EQUAL( messages[1]->getTimestamp(), 60 );
  BOOST_CHECK_EQUAL( messages[2]->getValue(), 50 ); BOOST_CHECK_EQUAL( messages[2]->getTimestamp(), 70 );
}
