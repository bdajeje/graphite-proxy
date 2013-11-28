#ifndef GRAPHITE_PROXY_UTILS_TIME_HPP
#define GRAPHITE_PROXY_UTILS_TIME_HPP

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/chrono.hpp>

namespace graphite_proxy {
namespace utils {
namespace time {

/*! Get the current time
 *  \return a unsigned number representing a timestamp in seconds
 */
ulong now();

/*! Get the human readable current time
 *  \return a string representing a date
 */
std::string humanDateTime();

/*! Parse a string representing a time
 *  \param sentence is the string to parse. It looks like: '3h27m21s'. Only 'h', 'H', 'm', 'M', 's' and 'S' are allowed
 *  \return a long representing the total number of seconds of this time
 *  \note the sentence has no defined order, it means you can '3h27m21s' or '21s3h27m' or '21s27m3h' are equal
 */
ulong parseTime( const std::string &sentence );

/*! Parse a string representing a numeric value
 *  \note it's an helper function which simply calls a boost::lexical_cast and catches boost::bad_lexical cast errors
 *  \param sentence is the string to parse.
 *  \return a ulong representing the sentence value in numeric
 */
ulong parseLong( const std::string &sentence );

} // namespace time
} // namespace utils
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_UTILS_TIME_HPP
