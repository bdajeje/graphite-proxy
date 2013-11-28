#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main

#include <boost/test/unit_test.hpp>

struct TestConfig {
  TestConfig()
  {
    // Called before any test
  }

  ~TestConfig()
  {
    // Called after all tests
  }
};

//____________________________________________________________________________//

BOOST_GLOBAL_FIXTURE( TestConfig );

