#include <boost/test/unit_test.hpp>

#include <graphite_proxy/utils/command_line.hpp>

#include <string>

using namespace graphite_proxy::utils::cl;

BOOST_AUTO_TEST_CASE( command_line_args_contain )
{
  const int argc = 3;
  char **argv = new char*[argc];
  argv[0] = new char[1]; argv[0][0] = '\0';
  argv[1] = new char[7]; argv[1][0] = '-'; argv[1][1] = '-'; argv[1][2] = 'h'; argv[1][3] = 'e'; argv[1][4] = 'l'; argv[1][5] = 'p'; argv[1][6] = '\0';
  argv[2] = new char[3]; argv[2][0] = 'y'; argv[2][1] = 'o'; argv[2][2] = '\0';

  const int aliases_length = 2;
  std::string aliases_1[aliases_length] = { "--help", "-h" };
  std::string aliases_2[aliases_length] = { "--version", "-v" };

  BOOST_CHECK_EQUAL( argsContain( argc, argv, aliases_1, aliases_length ), true );
  BOOST_CHECK_EQUAL( argsContain( argc, argv, aliases_2, aliases_length ), false );

  delete[] argv[2];
  delete[] argv[1];
  delete[] argv[0];
  delete[] argv;
}
