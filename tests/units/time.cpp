#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/time.hpp>
#include <graphite_proxy/utils/logging/logger.hpp>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( time_parsing )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Bad syntax times
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("5454"), 0 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime(""), 0 );

  // Good syntax times
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("42s"), 42 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("42S"), 42 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("8m"), 480 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("8M"), 480 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("1h"), 3600 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("1H"), 3600 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("1h15m"), 4500 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("1H15M"), 4500 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("3h27m21s"), 12441 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("3H27M21S"), 12441 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("27M3H21S"), 12441 );
  BOOST_CHECK_EQUAL( graphite_proxy::utils::time::parseTime("21S3H27M"), 12441 );
}
