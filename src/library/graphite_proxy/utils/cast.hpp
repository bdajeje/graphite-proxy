#ifndef CAST_HPP
#define CAST_HPP

#include <string>

namespace graphite_proxy {
namespace utils {
namespace cast {

/*! Convert a boolean to a human readable text
 *  \param value        to convert
 *  \param english_mode by default this convertion returns either "true" or "false". With the english_mode activated the results will be "yes" or "no".
 *  \param return either "true" or "false". With the english_mode activated the results will be "yes" or "no".
 */
std::string toString( bool value, bool english_mode = false );

} // namespace cast
} // namespace utils
} // namespace graphite_proxy

#endif // CAST_HPP
