#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/networking/client.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/timer.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( timer_public_behavior )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  networking::client_ptr client = boost::make_shared<networking::Client>( "127.0.0.1", "8081" );
  global_buffer_ptr      buffer = boost::make_shared<GlobalBuffer>( 99, true, client );
  timer_ptr              timer  = boost::make_shared<Timer>( buffer, client, 50 );

  BOOST_CHECK_EQUAL( timer->isStarted(), false );

  timer->start();
  BOOST_CHECK_EQUAL( timer->isStarted(), true );

  timer->stop();
  BOOST_CHECK_EQUAL( timer->isStarted(), false );
}
