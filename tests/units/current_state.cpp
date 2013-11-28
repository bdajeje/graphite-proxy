#include <boost/test/unit_test.hpp>

#include <networking/server.hpp>
#include <current_state.hpp>

#include <graphite_proxy/models/timer.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>
#include <graphite_proxy/models/router.hpp>
#include <graphite_proxy/networking/client.hpp>

#include <boost/make_shared.hpp>

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace graphite_proxy;

BOOST_AUTO_TEST_CASE( current_state )
{
  std::string state_file = "unittests_current_state.tmp";

  // Delete old file if present
  remove(state_file.c_str());

  // Test normal CurrentState behavior
  boost::shared_ptr<Router>       router;
  boost::asio::io_service         service;
  networking::client_ptr             g_client     = boost::make_shared<networking::Client>( "127.0.0.1", "2003" );
  global_buffer_ptr                  g_buffer     = boost::make_shared<GlobalBuffer>( 10, true, g_client );
  timer_ptr                          g_timer      = boost::make_shared<Timer>( g_buffer, g_client, 60 );
  maths::maths_ptr                   g_maths      = boost::make_shared<maths::MathsPipeline>( "conf/maths.xml", g_buffer, 60, 100 );
  statistics_ptr                     g_stats      = Statistics::init( g_buffer, g_maths, router, 60, "unittests" );
  server::networking::tcp_server_ptr g_tcp_server = boost::make_shared<server::networking::Server>( &service, router, "127.0.0.1", "8090" );
  server::networking::udp_server_ptr g_udp_server = boost::make_shared<server::networking::UDPServer>( service, router, 8090 );
  server::utils::CurrentState     state( state_file, g_client, g_buffer, g_timer, g_stats, g_maths, g_tcp_server, g_udp_server);
  std::string result = state.save();

  BOOST_REQUIRE( !result.empty() );

  // Read from save file
  std::string file_result, line;
  std::ifstream file;
  file.open(state_file.c_str());
  BOOST_REQUIRE( file.is_open() );
  while( file.good() )
  {
    getline(file, line);
    file_result += line;
  }
  file.close();

  BOOST_CHECK( !file_result.empty() );
}

