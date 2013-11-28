#include <boost/test/unit_test.hpp>

#include <graphite_proxy/models/router.hpp>

#include <graphite_proxy/models/maths/pipeline.hpp>
#include <graphite_proxy/models/maths/math_computation.hpp>

#include <boost/make_shared.hpp>

#include <vector>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( count_computation )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  maths::MathComputation computation_count( maths::SUM, false, 100 );

  BOOST_CHECK_EQUAL( computation_count.isOnCount(), true );
  BOOST_CHECK_EQUAL( computation_count.isOnTimeIteration(), false );
  BOOST_CHECK_EQUAL( computation_count.getType(), maths::SUM );
  BOOST_CHECK_EQUAL( computation_count.getIterationTime(), 0 );
  BOOST_CHECK_EQUAL( computation_count.getCount(), 100 );
  BOOST_CHECK_EQUAL( computation_count.getLastComputeTime(), 0 );
  BOOST_CHECK_EQUAL( computation_count.nextIterationTime(), 0 );
  BOOST_CHECK_EQUAL( computation_count.serialize(), "Message type='0', value='100', on_time='0'" );
}

BOOST_AUTO_TEST_CASE( time_computation )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  maths::MathComputation computation_time( maths::AVERAGE, true, 100, 1 );

  BOOST_CHECK_EQUAL( computation_time.isOnCount(), false );
  BOOST_CHECK_EQUAL( computation_time.isOnTimeIteration(), true );
  BOOST_CHECK_EQUAL( computation_time.getType(), maths::AVERAGE );
  BOOST_CHECK_EQUAL( computation_time.getIterationTime(), 100 );
  BOOST_CHECK_EQUAL( computation_time.getCount(), 0 );
  BOOST_CHECK_EQUAL( computation_time.getLastComputeTime(), 1 );
  BOOST_CHECK_EQUAL( computation_time.nextIterationTime(), 101 );
  BOOST_CHECK_EQUAL( computation_time.serialize(), "Message type='1', value='100', on_time='1'" );

  computation_time.incrementLastComputeTime();

  BOOST_CHECK_EQUAL( computation_time.nextIterationTime(), 201 );
}

BOOST_AUTO_TEST_CASE( computation_enum )
{
  BOOST_CHECK_EQUAL( maths::SUM, 0 );
  BOOST_CHECK_EQUAL( maths::AVERAGE, 1 );
  BOOST_CHECK_EQUAL( maths::MIN, 2 );
  BOOST_CHECK_EQUAL( maths::MAX, 3 );
  BOOST_CHECK_EQUAL( maths::MEDIAN, 4 );
  BOOST_CHECK_EQUAL( maths::TILES, 5 );
  BOOST_CHECK_EQUAL( maths::VARIANCE, 6 );
  BOOST_CHECK_EQUAL( maths::DEVIATION, 7 );
  BOOST_CHECK_EQUAL( maths::UNKNOWN, 8 );
}

BOOST_AUTO_TEST_CASE( maths_average )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 10.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 0.5, 10 ) );

  message_ptr result = pipeline.average( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 4.5 );
}

BOOST_AUTO_TEST_CASE( maths_sum )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );

  message_ptr result = pipeline.sum( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 6 );
}

BOOST_AUTO_TEST_CASE( maths_variance )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3, 10 ) );

  message_ptr result = pipeline.variance( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_CLOSE( result->getValue(), 0.667, 0.06 );
}

BOOST_AUTO_TEST_CASE( maths_deviation )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3, 10 ) );

  message_ptr result = pipeline.deviation( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_CLOSE( result->getValue(), 0.816, 0.07 );
}

BOOST_AUTO_TEST_CASE( maths_max )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );

  message_ptr result = pipeline.max( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 3 );
}

BOOST_AUTO_TEST_CASE( maths_min )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );

  message_ptr result = pipeline.min( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 1 );
}

BOOST_AUTO_TEST_CASE( maths_median_odd )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 1.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );

  message_ptr result = pipeline.median( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 2 );
}

BOOST_AUTO_TEST_CASE( maths_median_even )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 1.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 4.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );

  message_ptr result = pipeline.median( messages );

  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( result->getValue(), 2.5 );
}

BOOST_AUTO_TEST_CASE( maths_tiles )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 3, true, client );
  maths::MathsPipeline pipeline( "conf/maths.xml", buffer, 1, 500 );

  std::vector<message_ptr> messages;
  messages.push_back( boost::make_shared<Message>( "test.1", 4.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 4.7, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 3.6, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 2.0, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 6.4, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 8.2, 10 ) );
  messages.push_back( boost::make_shared<Message>( "test.1", 5.0, 10 ) );

  message_ptr result = pipeline.tiles( messages, 6.4, true, 100 );
  BOOST_CHECK_EQUAL( result->isValid(), true );
  BOOST_CHECK_EQUAL( result->getType(), "test.1" );
  BOOST_CHECK_EQUAL( (int)result->getValue(), 71 );

  result = pipeline.tiles( messages, 6.4, false, 100 );
  BOOST_CHECK_EQUAL( (int)result->getValue(), 78 );

  result = pipeline.tiles( messages, 100, false, 100 );
  BOOST_CHECK_EQUAL( (int)result->getValue(), 100 );

  result = pipeline.tiles( messages, 0, false, 100 );
  BOOST_CHECK_EQUAL( (int)result->getValue(), 0 );

  result = pipeline.tiles( messages, 6, false, 100 );
  BOOST_CHECK_EQUAL( (int)result->getValue(), 71 );
}
