#include <boost/test/unit_test.hpp>

#include <graphite_proxy/models/message.hpp>

#include <vector>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( message_static_creation )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Normal message
  message_ptr message_1 = Message::createMessage("type.1 10 123456789");
  BOOST_CHECK( message_1->isValid() );
  BOOST_CHECK_EQUAL( message_1->getTimestamp(), 123456789 );
  BOOST_CHECK_EQUAL( message_1->getValue(), 10 );
  BOOST_CHECK_EQUAL( message_1->getType(), "type.1" );

  // Miss value
  message_ptr message_2 = Message::createMessage("type.1");
  BOOST_CHECK( !message_2 );

  // Weird syntaxes but have to work
  message_ptr message_4 = Message::createMessage("   type.1 0 1657   ");
  BOOST_CHECK( message_4 );
  BOOST_CHECK_EQUAL( message_4->getTimestamp(), 1657 );
  message_ptr message_6 = Message::createMessage("   type.1 0 0");
  BOOST_CHECK( message_6 );
  BOOST_CHECK_EQUAL( message_6->getTimestamp(), 0 );
  message_ptr message_8 = Message::createMessage("type.1 0 6574  ");
  BOOST_CHECK( message_8 );
  BOOST_CHECK_EQUAL( message_8->getTimestamp(), 6574 );
}

BOOST_AUTO_TEST_CASE( messages_static_creation )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  std::vector<message_ptr> messages;
  BOOST_CHECK( Message::createMessages("type.1 0 0\ntype.2 0 0\ntype.3 0 0\n", messages) );
  BOOST_CHECK_EQUAL( messages.size(), 3 );
  BOOST_CHECK_EQUAL( messages[0]->serialize(), "type.1 0.000000 0" );
  BOOST_CHECK_EQUAL( messages[1]->serialize(), "type.2 0.000000 0" );
  BOOST_CHECK_EQUAL( messages[2]->serialize(), "type.3 0.000000 0" );

  std::vector<message_ptr> messages_2;
  BOOST_CHECK_EQUAL( Message::createMessages("some para\nnani\nagain and again\n", messages_2), false );
  BOOST_CHECK( messages_2.empty() );

  std::vector<message_ptr> messages_3;
  BOOST_CHECK( Message::createMessages("some para\nsomething.correct 0 4657\nagain and again\nsomething.correct 10 4657  ", messages_3) );
  BOOST_CHECK_EQUAL( messages_3.size(), 2 );
}

BOOST_AUTO_TEST_CASE( message_public_behavior )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  Message message_1;
  BOOST_CHECK_EQUAL( message_1.getType(), "" );
  BOOST_CHECK_EQUAL( message_1.getValue(), 0.0 );
  BOOST_CHECK_EQUAL( message_1.getTimestamp(), 0 );
  BOOST_CHECK_EQUAL( message_1.serialize(), "" );
  BOOST_CHECK_EQUAL( message_1.length(), 0 );
  BOOST_CHECK_EQUAL( message_1.isValid(), false );

  Message message_2( "type.1", 55.12, 123456789, 987654321 );
  BOOST_CHECK_EQUAL( message_2.getType(), "type.1" );
  BOOST_CHECK_EQUAL( message_2.getValue(), 55.12 );
  BOOST_CHECK_EQUAL( message_2.getTimestamp(), 123456789 );
  BOOST_CHECK_EQUAL( message_2.getReceivedTimestamp(), 987654321 );
  BOOST_CHECK_EQUAL( message_2.serialize(), "type.1 55.120000 123456789" );
  BOOST_CHECK_EQUAL( message_2.length(), 26 );
  BOOST_CHECK_EQUAL( message_2.isValid(), true );
}

