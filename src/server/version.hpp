#ifndef GRAPHITE_PROXY_VERSION_HPP
#define GRAPHITE_PROXY_VERSION_HPP

#include <iostream>

namespace graphite_proxy {
namespace utils {
namespace version {

#define GRAPHITE_PROXY_VERSION 1.0
#define GRAPHITE_PROXY_VERSION_COMMAND "-v"
#define GRAPHITE_PROXY_VERSION_COMMAND_ALIAS "--version"

void showVersion()
{
  std::cout << "Graphite Proxy Version " << GRAPHITE_PROXY_VERSION << std::endl;
}

} //   namespace version
} //   namespace utils
} //   namespace graphite_proxy

#endif // GRAPHITE_PROXY_VERSION_HPP
