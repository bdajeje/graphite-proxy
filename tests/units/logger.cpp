#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>

#include <iostream>
#include <string>

using namespace graphite_proxy;

void test_log( std::string log_message, bool expected_result, utils::logging::Logger::LogLevel log_level, utils::logging::logger_ptr logger )
{
  std::streambuf *old_streambuf = std::cout.rdbuf(); // back up cout's streambuf
  std::ostringstream log_stream;

  std::cout.rdbuf( log_stream.rdbuf() ); // Put handler streambug
  logger->log( log_level, log_message ); // Log message, it will go inside log_stream
  std::cout.rdbuf( old_streambuf ); // Restore normal cout behavior to see boost unit tests messages
  BOOST_CHECK_EQUAL( !log_stream.str().empty(), expected_result );
}

BOOST_AUTO_TEST_CASE( logs_debug )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::DEBUG );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  test_log( "test", true, utils::logging::Logger::LogLevel::DEBUG, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::INFO, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::WARNING, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::ERROR, logger );
}

BOOST_AUTO_TEST_CASE( logs_info )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::INFO );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  test_log( "test", false, utils::logging::Logger::LogLevel::DEBUG, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::INFO, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::WARNING, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::ERROR, logger );
}

BOOST_AUTO_TEST_CASE( logs_warning )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::WARNING );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  test_log( "test", false, utils::logging::Logger::LogLevel::DEBUG, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::INFO, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::WARNING, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::ERROR, logger );
}

BOOST_AUTO_TEST_CASE( logs_error )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::ERROR );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  test_log( "test", false, utils::logging::Logger::LogLevel::DEBUG, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::INFO, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::WARNING, logger );
  test_log( "test", true, utils::logging::Logger::LogLevel::ERROR, logger );
}

BOOST_AUTO_TEST_CASE( logs_quiet )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  test_log( "test", false, utils::logging::Logger::LogLevel::DEBUG, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::INFO, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::WARNING, logger );
  test_log( "test", false, utils::logging::Logger::LogLevel::ERROR, logger );
}

BOOST_AUTO_TEST_CASE( log_configuration )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::ERROR );
  utils::logging::logger_ptr logger = utils::logging::Logger::instance();

  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::DEBUG ), false );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::INFO ), false );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::WARNING ), false );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::ERROR ), true );
}

BOOST_AUTO_TEST_CASE( log_bad_configuration )
{
  std::streambuf *old_streambuf = std::cout.rdbuf();
  std::ostringstream log_stream;
  std::cout.rdbuf( log_stream.rdbuf() );

  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::ERROR );
  utils::logging::Logger::setLevel( "log level which doesn't exist" ); // It will take the default log level set in utils/properties.hpp

  std::cout.rdbuf( old_streambuf );

  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::DEBUG ), true );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::INFO ), true );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::WARNING ), true );
  BOOST_CHECK_EQUAL( utils::logging::Logger::isLogging( utils::logging::Logger::LogLevel::ERROR ), true );
}

BOOST_AUTO_TEST_CASE( string_level_to_log_level )
{
  BOOST_CHECK( utils::logging::Logger::toLevel( utils::logging::level::Debug ) == utils::logging::Logger::LogLevel::DEBUG );
  BOOST_CHECK( utils::logging::Logger::toLevel( utils::logging::level::Error ) == utils::logging::Logger::LogLevel::ERROR );
  BOOST_CHECK( utils::logging::Logger::toLevel( utils::logging::level::Info ) == utils::logging::Logger::LogLevel::INFO );
  BOOST_CHECK( utils::logging::Logger::toLevel( utils::logging::level::Quiet ) == utils::logging::Logger::LogLevel::QUIET );
  BOOST_CHECK( utils::logging::Logger::toLevel( utils::logging::level::Warning ) == utils::logging::Logger::LogLevel::WARNING );
  BOOST_CHECK( utils::logging::Logger::toLevel( "" ) == utils::logging::Logger::LogLevel::DEBUG );
}
