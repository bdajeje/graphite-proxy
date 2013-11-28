#ifndef GRAPHITE_PROXY_FILES_HPP
#define GRAPHITE_PROXY_FILES_HPP

#include <string>

namespace graphite_proxy {
namespace utils {
namespace files {

/*! Find the configuration filepath from the application instance directory
 *  \param application_path is the first string given in argv to locate the programm instance
 *  \return the found full filepath or an empty string
 */
std::string findFilePath( const std::string &application_path );

} // namespace files
} // namespace utils
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_FILES_HPP
