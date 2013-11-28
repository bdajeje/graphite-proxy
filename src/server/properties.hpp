#ifndef GRAPHITE_PROXY_PROPERTIES_HPP
#define GRAPHITE_PROXY_PROPERTIES_HPP

#include <string>

namespace server {
namespace props {

// Name of the root node in the configuration file
static const std::string PROPERTIES_ROOT                                  = "graphite-proxy";

// Configurations properties
static const std::string PROPERTIES_CONFIGURATIONS_FILES_DIR              = "config-dir";
static const std::string PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG          = "-c";
static const std::string PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG_ALIAS    = "--config-dir";
static const std::string PROPERTIES_CONFIGURATIONS_FILEPATH               = "config-filepath";
static const std::string PROPERTIES_CONFIGURATIONS_FILEPATH_DEFAULT       = "conf/configurations.xml";

// Server properties
static const std::string PROPERTIES_SERVER_ADDRESS                        = "server.address";
static const std::string PROPERTIES_SERVER_PORT                           = "server.port";
static const std::string PROPERTIES_SERVER_PORT_DEFAULT                   = "8090";
static const std::string PROPERTIES_SERVER_UDP_PORT                       = "server.udp_port";
static const std::string PROPERTIES_SERVER_UDP_PORT_DEFAULT               = "8091";

// Client properties
static const std::string PROPERTIES_CLIENT_ADDRESS                        = "client.address";
static const std::string PROPERTIES_CLIENT_PORT                           = "client.port";
static const std::string PROPERTIES_CLIENT_PORT_DEFAULT                   = "2003";

// Buffer properties
static const std::string PROPERTIES_BUFFER_MAX_ITEMS                      = "buffer.flush.size";
static const unsigned int PROPERTIES_BUFFER_MAX_ITEMS_DEFAULT             = 10000;
static const std::string PROPERTIES_BUFFER_FLUSH_TIME                     = "buffer.flush.time";
static const unsigned int PROPERTIES_BUFFER_FLUSH_TIME_DEFAULT            = 5; // in seconds
static const std::string PROPERTIES_BUFFER_DROP_OLDER                     = "buffer.drop.oldest";
static const bool PROPERTIES_BUFFER_DROP_OLDER_DEFAULT                    = true;
static const std::string PROPERTIES_BUFFER_CLEANER_ACTIVATED              = "buffer.cleaning.activated";
static const bool PROPERTIES_BUFFER_CLEANER_ACTIVATED_DEFAULT             = true;
static const std::string PROPERTIES_BUFFER_CLEANER_TIME                   = "buffer.cleaning.time";
static const unsigned int PROPERTIES_BUFFER_CLEANER_TIME_DEFAULT          = 300;
static const std::string PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME         = "buffer.cleaning.max-empty-time";
static const unsigned int PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME_DEFAULT= 3;

// Logger properties
static const std::string PROPERTIES_LOGS_LEVEL                            = "logs.level";
static const std::string PROPERTIES_LOGS_LEVEL_DEFAULT                    = "warning";
static const std::string PROPERTIES_LOGS_COLOR                            = "logs.colors";
static const bool PROPERTIES_LOGS_COLOR_DEFAULT                           = false;
static const std::string PROPERTIES_LOGS_DESTINATION                      = "logs.destination";
static const std::string PROPERTIES_LOGS_DESTINATION_DEFAULT              = "stdout";

// statictics properties
static const std::string PROPERTIES_STATS_ENABLE                          = "stats.enabled";
static const bool PROPERTIES_STATS_ENABLE_DEFAULT                         = true;
static const std::string PROPERTIES_STATS_SLEEP_TIME                      = "stats.time";
static const unsigned int PROPERTIES_STATS_SLEEP_TIME_DEFAULT             = 600; // in seconds (10 minutes here)

// Maths properties
static const std::string PROPERTIES_MATHS_FILEPATH                        = "maths-filepath";
static const std::string PROPERTIES_MATHS_FILEPATH_DEFAULT                = "conf/maths.xml";
static const std::string PROPERTIES_MATHS_ENABLE                          = "maths.enabled";
static const bool PROPERTIES_MATHS_ENABLE_DEFAULT                         = true;
static const std::string PROPERTIES_MATHS_MAX_ITEMS                       = "maths.size";
static const unsigned int PROPERTIES_MATHS_MAX_ITEMS_DEFAULT              = 9999;
static const std::string PROPERTIES_MATHS_SLEEP_TIME                      = "maths.time";
static const unsigned int PROPERTIES_MATHS_SLEEP_TIME_DEFAULT             = 60;

// Router properties
static const std::string PROPERTIES_ROUTER_SAVE_ON_CLOSE                  = "router.save";
static const bool PROPERTIES_ROUTER_SAVE_ON_CLOSE_DEFAULT                 = true;
static const std::string PROPERTIES_ROUTER_SAVE_FILE                      = "router.file";
static const std::string PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE          = "router.pass-through";
static const std::string PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE_DEFAULT  = "pass_through_messages.gp";
static const std::string PROPERTIES_ROUTER_MATHS_SAVE_FILE                = "router.maths";
static const std::string PROPERTIES_ROUTER_MATHS_SAVE_FILE_DEFAULT        = "maths_messages.gp";

// Signals properties
static const std::string PROPERTIES_SIGNALS_CURRENT_STATE_FILE            = "signals.current-state.file";
static const std::string PROPERTIES_SIGNALS_CURRENT_STATE_FILE_DEFAULT    = "current_state.gp";

} // namespace props
} // namespace server

#endif // GRAPHITE_PROXY_PROPERTIES_HPP

