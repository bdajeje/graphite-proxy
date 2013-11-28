#ifndef GRAPHITE_PROXY_COMMAND_LINE_HPP
#define GRAPHITE_PROXY_COMMAND_LINE_HPP

#include <string>

namespace graphite_proxy {
namespace utils {
namespace cl {

/*! Does the command line arguments contains at least one of the aliases
 *  \param argc is argc coming from main
 *  \param argc is argv coming from main
 *  \param aliases is the list of possible matches to find
 *  \param aliases_size is the length of the array 'aliases'
 *  \return true if at least on of the aliases has been found into argv
 *  \note if argv or aliases are empty return false
 */
bool argsContain( int argc, char **argv, const std::string aliases[], unsigned int aliases_size );

} // namespace cl
} // namespace utils
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_COMMAND_LINE_HPP
