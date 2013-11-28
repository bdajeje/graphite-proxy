#include <boost/test/unit_test.hpp>

/********** HACK *********/
/* exists only because we need to call iteration() on the math pipeline */
  #define protected public
/*************************/

#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( configuration_file_good_load )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );

  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 99, 99 );
  BOOST_CHECK_EQUAL( pipeline.isValid(), true );
}

BOOST_AUTO_TEST_CASE( configuration_file_bad_load )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );

  maths::MathsPipeline pipeline( "conf/unknown.xml", buffer, 99, 99 );
  BOOST_CHECK_EQUAL( pipeline.isValid(), false );
}

BOOST_AUTO_TEST_CASE( configuration_file_bad_syntax )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );

  maths::MathsPipeline pipeline( "conf/maths_bad.xml", buffer, 99, 99 );
  BOOST_CHECK_EQUAL( pipeline.isValid(), false );
}

BOOST_AUTO_TEST_CASE( regex_category )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 999, true, client );
  maths::MathsPipeline pipeline( "conf/maths_very_generic.xml", buffer, 1, 99 );

  // Is wanted tests
  BOOST_CHECK( pipeline.isWanted("ads_server.nani.nbr") );
  BOOST_CHECK( pipeline.isWanted("ads_server.NANI.nbr") );
  BOOST_CHECK( pipeline.isWanted("ads_server.na9na.nbr") );
  BOOST_CHECK( pipeline.isWanted("ads_server.1234.nbr") );
  BOOST_CHECK( pipeline.isWanted("ads_server.nani.elapsed_time") );
  BOOST_CHECK( !pipeline.isWanted("whatever") );
  BOOST_CHECK( !pipeline.isWanted("ads_serverelapsed_time") );
  BOOST_CHECK( !pipeline.isWanted("ads_server.elapsed_time") );
  BOOST_CHECK( !pipeline.isWanted("ads_server..elapsed_time") );

  // Add messages
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.1.nbr", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.1.nbr", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.2.nbr", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.2.nbr", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.1.elapsed_time", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.1.elapsed_time", 3) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.2.elapsed_time", 1) ) );
  BOOST_CHECK( pipeline.add( boost::make_shared<Message>("ads_server.2.elapsed_time", 3) ) );

  // Check right number of buffers
  BOOST_CHECK_EQUAL( pipeline.getNbrBuffers(), 4 );

  // Force a math computation iteration (HACK)
  pipeline.iteration();

  // Messages should be computed and stored into the global buffer now
  std::vector<message_ptr> maths_messages;
  pipeline.get(maths_messages);
  BOOST_CHECK_EQUAL( maths_messages.size(), 0 );
  std::vector<message_ptr> result_messages;
  buffer->get( result_messages );
  BOOST_CHECK_EQUAL( result_messages.size(), 4 );

  // Let's check taht the maths pipeline did its work correctly
  bool check_message_1, check_message_2, check_message_3, check_message_4;
  check_message_1 = check_message_2 = check_message_3 = check_message_4 = false;
  for( size_t i = 0, size = result_messages.size(); i < size; i++ )
  {
    std::string message = result_messages[i]->serialize();
    if( boost::algorithm::starts_with(message, "ads_server.1.nbr 2") )
        check_message_1 = true;
    else if( boost::algorithm::starts_with(message, "ads_server.2.nbr 2") )
        check_message_2 = true;
    else if( boost::algorithm::starts_with(message, "ads_server.1.elapsed_time 2") )
        check_message_3 = true;
    else if( boost::algorithm::starts_with(message, "ads_server.2.elapsed_time 2") )
        check_message_4 = true;
  }
  BOOST_CHECK(check_message_1);
  BOOST_CHECK(check_message_2);
  BOOST_CHECK(check_message_3);
  BOOST_CHECK(check_message_4);
}

BOOST_AUTO_TEST_CASE( maths_pipeline_get_messages )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 99, 99 );

  BOOST_CHECK( pipeline.add( Message::createMessage("maths.1 0 0") ) );
  BOOST_CHECK( pipeline.add( Message::createMessage("maths.2 0 0") ) );
  BOOST_CHECK( pipeline.add( Message::createMessage("maths.3 0 0") ) );

  std::vector<message_ptr> messages;
  pipeline.get( messages );
  BOOST_CHECK_EQUAL( messages.size(), 3 );

  std::vector<message_ptr> messages_2;
  pipeline.get( messages_2 );
  BOOST_CHECK( messages_2.empty() );
}

BOOST_AUTO_TEST_CASE( maths_pipeline_reload_configurations )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths_load_1.xml", buffer, 99, 99 );

  BOOST_CHECK( !pipeline.isWanted("maths.load_2") );

  pipeline.reloadConfigurations( "conf/maths_load_2.xml" );
  BOOST_CHECK( pipeline.isWanted("maths.load_2") );
}
