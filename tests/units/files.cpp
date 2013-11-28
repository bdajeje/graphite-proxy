#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/files.hpp>

#include <string>

using namespace graphite_proxy::utils::files;

BOOST_AUTO_TEST_CASE( files_find_file_path )
{
  BOOST_CHECK_EQUAL( findFilePath( "./home/jeremy/graphite_proxy" ), "./home/jeremy/" );
  BOOST_CHECK_EQUAL( findFilePath( "jeremy/graphite_proxy" ), "jeremy/" );
  BOOST_CHECK_EQUAL( findFilePath( "graphite_proxy" ), "" );
  BOOST_CHECK_EQUAL( findFilePath( "" ), "" );
}
