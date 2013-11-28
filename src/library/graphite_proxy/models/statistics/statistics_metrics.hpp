#ifndef GRAPHITE_PROXY_STATISTICS_METRICS_HPP
#define GRAPHITE_PROXY_STATISTICS_METRICS_HPP

#include <string>

namespace graphite_proxy {
namespace stats {

// Request statistics
static const std::string STATS_REQUESTS_INCOMING 				 = "requests.incoming.nbr";
static const std::string STATS_REQUESTS_ACCEPTED 				 = "requests.accepted.nbr";
static const std::string STATS_REQUESTS_DROPPED  				 = "requests.dropped.nbr";
static const std::string STATS_REQUESTS_SEND     				 = "requests.send.nbr";
static const std::string STATS_REQUESTS_SEND_CONTENT     = "requests.send.content"; // Number of metrics messages into a client message

// Maths computations
static const std::string STATS_MATHS_MESSAGES            = "maths.messages.created.nbr";
static const std::string STATS_MATHS_SUM                 = "maths.operations.sum";
static const std::string STATS_MATHS_AVERAGE             = "maths.operations.average";
static const std::string STATS_MATHS_VARIANCE            = "maths.operations.variance";
static const std::string STATS_MATHS_DEVIATION           = "maths.operations.deviation";
static const std::string STATS_MATHS_MIN                 = "maths.operations.min";
static const std::string STATS_MATHS_MAX                 = "maths.operations.max";
static const std::string STATS_MATHS_MEDIAN              = "maths.operations.median";
static const std::string STATS_MATHS_TILES               = "maths.operations.tiles";

// Statistics metrics of statistics
static const std::string STATS_STATS_MESSAGES 					 = "statistics.messages.created.nbr";

// Buffers
static const std::string STATS_GLOBAL_BUFFER_MESSAGES_MAX= "global_buffer.messages.max";
static const std::string STATS_MATH_BUFFER_MESSAGES_MAX  = "math_buffer.messages.max";

// Client
static const std::string STATS_CLIENT_CONNECTIONFAILED   = "client.connection.failed.nbr";

// Message
static const std::string STATS_MESSAGE_CREATED 					 = "messages.created.nbr";

} // namespace stats
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_STATISTICS_METRICS_HPP
