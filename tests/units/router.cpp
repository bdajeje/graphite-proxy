#include <boost/test/unit_test.hpp>

#include <graphite_proxy/models/router.hpp>

#include <graphite_proxy/models/maths/pipeline.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>

#include <boost/make_shared.hpp>

#include <algorithm>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>

using namespace graphite_proxy;

void readFile( const std::string filename, std::vector<std::string>& results )
{
  std::ifstream file;
  file.open( filename.c_str() );
  BOOST_CHECK( file.is_open() );
  while( file.good() )
  {
    char line[1024];
    file.getline( line, 256 );
    std::string string_line( line );
    if ( string_line.size() > 2 )
      results.push_back( line );
  }
}

BOOST_AUTO_TEST_CASE( serialize_messages )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 10, true, client );
  maths::maths_ptr pipeline     = boost::make_shared<maths::MathsPipeline>( "conf/maths.xml", buffer, 99, 99 );

  Router router( buffer, pipeline );

  BOOST_CHECK( router.routeMessage( "normal.1 0 0" ) );
  BOOST_CHECK( router.routeMessage( "normal.2 0 0" ) );
  BOOST_CHECK( router.routeMessage( "test_2.nani 0 1234" ) );
  BOOST_CHECK( router.routeMessage( "test_2.nani 1 5678" ) );

  const std::string filename_pass_though = "unittests_pass_through_serialize.tmp";
  const std::string filename_math        = "unittests_math_serialize.tmp";
  BOOST_REQUIRE_EQUAL( router.serialize( filename_pass_though, filename_math ), 6 ); // 6 messages has to be serialized. 2 normal + 2 maths which have 2 computations each (2 + 2 x 2 )

  // Pass through messages have been serialized correctly ?
  std::vector<std::string> lines;
  readFile( filename_pass_though, lines );
  BOOST_CHECK_EQUAL( lines.size(), 2 );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "normal.1 0.000000 0" ) != lines.end() );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "normal.2 0.000000 0" ) != lines.end() );

  // Math messages have been serialized correctly ?
  lines.clear();
  readFile( filename_math, lines );
  BOOST_CHECK_EQUAL( lines.size(), 4 );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "test_2.nani 0.000000 1234 sum" ) != lines.end() );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "test_2.nani 0.000000 1234 average" ) != lines.end() );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "test_2.nani 1.000000 5678 sum" ) != lines.end() );
  BOOST_CHECK( std::find( lines.begin(), lines.end(), "test_2.nani 1.000000 5678 average" ) != lines.end() );

  // Load saved messages
  BOOST_REQUIRE_EQUAL( router.load( filename_pass_though, filename_math ), 6 ); // Same amount of messages than for the serizalized

  // Check pass through loaded messages
  std::vector<message_ptr> messages;
  buffer->get( messages );
  BOOST_CHECK_EQUAL( messages.size(), 2 );

  // Check math loaded messages
  messages.clear();
  pipeline->get( messages );
  BOOST_CHECK_EQUAL( messages.size(), 4 );

  // Save file has to be deleted
  std::ifstream read_file_pass_through( filename_pass_though.c_str() );
  BOOST_CHECK( !read_file_pass_through );
  std::ifstream read_file_math( filename_math.c_str() );
  BOOST_CHECK( !read_file_math );
}

