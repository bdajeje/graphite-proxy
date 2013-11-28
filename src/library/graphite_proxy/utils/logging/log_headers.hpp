#ifndef GRAPHITE_PROXY_LOG_HEADERS_HPP
#define GRAPHITE_PROXY_LOG_HEADERS_HPP

namespace graphite_proxy {
namespace utils {
namespace logging {

static const std::string LOG_HEADER_MAIN =              "  MAIN  ";
static const std::string LOG_HEADER_CLIENT =            " CLIENT ";
static const std::string LOG_HEADER_BUFFER =            " BUFFER ";
static const std::string LOG_HEADER_GLOBALBUFFER =      "G_BUFFER";
static const std::string LOG_HEADER_TIMER =             " TIMER  ";
static const std::string LOG_HEADER_LOGGER =            " LOGGER ";
static const std::string LOG_HEADER_STATISTICS =        " STATS  ";
static const std::string LOG_HEADER_CLEANER =           "CLEANER ";
static const std::string LOG_HEADER_MATHS =             " MATHS  ";
static const std::string LOG_HEADER_MATHS_COMPUTATION = "MATHS CO";
static const std::string LOG_HEADER_REQUEST =           "REQUEST ";
static const std::string LOG_HEADER_ROUTER =            " ROUTER ";
static const std::string LOG_HEADER_UTILS =             " UTILS  ";
static const std::string LOG_HEADER_CURRENT_STATE =     " STATE  ";

} // namespace logging
} // namespace utils
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_LOG_HEADERS_HPP
