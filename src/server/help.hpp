#ifndef GRAPHITE_PROXY_HELP_HPP
#define GRAPHITE_PROXY_HELP_HPP

#include <version.hpp>
#include <properties.hpp>
#include <exit_status.hpp>

#include <iostream>
#include <string>

namespace graphite_proxy {
namespace utils {
namespace help {

#define GRAPHITE_PROXY_HELP_COMMAND "-h"
#define GRAPHITE_PROXY_HELP_COMMAND_ALIAS "--help"

void showHelp()
{
  const std::string bold      = "\033[1m";
  const std::string normal    = "\033[0m";
  const std::string underline = "\033[4m";

  std::stringstream help;

  // Name
  help << bold << "NAME\n" << normal;
  help << "\tgraphite-proxy - Graphite Proxy Server V" << GRAPHITE_PROXY_VERSION << "\n";
  help << "\n";

  // Synopsis
  help << bold << "SYNOPSIS\n" << normal;
  help << "\tgraphite-proxy [OPTION]\n";
  help << "\n";

  // Description
  help << bold << "DESCRIPTION\n" << normal;
  help << "\tLaunch a Graphite Proxy Server to receive plain text graphite messages, compute them and then send them to a Graphite server\n\n";
  help << "\tThere is only one mandatory argument the -c (or --config-dir) which specify the 'conf' directory of the Graphite Proxy:\n";
  help << "\n";

  // Params
  help << underline << "\tProgram arguments:\n\n" << normal;
    // --config-dir
    help << bold << "\t-c, --config-dir\n" << normal;
    help << "\t\tSpecify the 'conf' directory of the Graphite Proxy\n\n";
    // --version
    help << bold << "\t" << GRAPHITE_PROXY_VERSION_COMMAND << ", " << GRAPHITE_PROXY_VERSION_COMMAND_ALIAS << "\n" << normal;
    help << "\t\tShow the version number of the graphite-proxy program\n\n";
    // --help
    help << bold << "\t" << GRAPHITE_PROXY_HELP_COMMAND << ", " << GRAPHITE_PROXY_HELP_COMMAND_ALIAS << "\n" << normal;
    help << "\t\tShow the help documentation of the graphite-proxy program\n\n";

  // Exit status
  help << bold << "EXIT STATUS\n" << normal;
  help << "\t0\tif OK\n";
  help << "\t" << EXIT_BAD_CONFIGURATION << "\tif bad configurations (e.g., normal quit but one or more configuration are badly set)\n";
  help << "\t" << EXIT_FORCE_QUIT << "\tif exit force quit (e.g., can't quit normally, force quit)\n";
  help << "\n";

  // Author
  help << bold << "AUTHOR\n" << normal;
  help << "\tProgrammed by Jeremy Gousse (jeremy.gousse@gmail.com)\n";
  help << "\tSpecial thanks to Jean-Sebastien Gelinas who advised and help me on the project\n";
  help << "\n";

  // Reporting bugs
  help << bold << "REPORTING BUGS\n" << normal;
  help << "\tReport bugs to jeremy.gousse@gmail.com\n";
  help << "\n";

  // See also
  help << bold << "SEE ALSO\n" << normal;
  help << "\tThe full documentation for graphite-proxy is the README file located into the source root directory of the project.\n";
  help << "\n";

  std::cout << help.str() << std::endl;
}

} //   namespace help
} //   namespace utils
} //   namespace graphite_proxy

#endif // GRAPHITE_PROXY_HELP_HPP
