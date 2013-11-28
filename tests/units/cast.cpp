#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/cast.hpp>

using namespace graphite_proxy::utils;

/*! Test without english mode */
BOOST_AUTO_TEST_CASE( no_english_mode )
{
  BOOST_CHECK_EQUAL( cast::toString(true), "true" );
  BOOST_CHECK_EQUAL( cast::toString(false), "false" );
}

/*! Test with english mode */
BOOST_AUTO_TEST_CASE( english_mode )
{
  BOOST_CHECK_EQUAL( cast::toString(true, true), "yes" );
  BOOST_CHECK_EQUAL( cast::toString(false, true), "no" );
}
