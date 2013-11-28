#include <networking/server.hpp>
#include <networking/udp_server.hpp>
#include <configurations_loader.hpp>
#include <properties.hpp>
#include <version.hpp>
#include <help.hpp>
#include <exit_status.hpp>
#include <current_state.hpp>
#include <system_helper.hpp>

#include <graphite_proxy/models/timer.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/buffers/cleaner.hpp>
#include <graphite_proxy/models/router.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>

#include <graphite_proxy/networking/client.hpp>

#include <graphite_proxy/utils/files.hpp>
#include <graphite_proxy/utils/command_line.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>
#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <iostream>
#include <csignal>

using namespace graphite_proxy;

networking::client_ptr                                  g_client;
global_buffer_ptr                                       g_buffer;
timer_ptr                                               g_timer;
router_ptr                                              g_router;
statistics_ptr                                          g_stats;
maths::maths_ptr                                        g_maths;
boost::asio::io_service                                 g_service;
server::networking::tcp_server_ptr                      g_tcp_server;
server::networking::udp_server_ptr                      g_udp_server;
boost::shared_ptr<server::utils::ConfigurationsLoader>  g_configs_loader;
boost::shared_ptr<Cleaner>                              g_cleaner;

/*! Function to treat operating system signals
 *  \param signal_type is the type of the signal
 */
void signalsHandler( int signal_type )
{
  // Special signal to reload maths configurations
  if( signal_type == SIGUSR1 ) // SIGUSR1 == 10
  {
    if( g_maths )
    {
      const std::string maths_filepath = g_configs_loader->getConfFilesDir() + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_MATHS_FILEPATH, server::props::PROPERTIES_MATHS_FILEPATH_DEFAULT );
      g_maths->reloadConfigurations( maths_filepath );
    }
    else LOG_INFO( "Maths module disabled", utils::logging::LOG_HEADER_MAIN );
  }
  // Special signal to dump current state in a file
  else if( signal_type == SIGUSR2 ) // SIGUSR2 == 12
  {
    static const std::string state_file = g_configs_loader->getConfFilesDir() + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_SIGNALS_CURRENT_STATE_FILE, server::props::PROPERTIES_SIGNALS_CURRENT_STATE_FILE_DEFAULT );
    server::utils::CurrentState state( state_file, g_client, g_buffer, g_timer, g_stats, g_maths, g_tcp_server, g_udp_server );
    LOG_INFO( state.save(), utils::logging::LOG_HEADER_MAIN );
  }
  else // Any other signals, quit the application
  {
    static bool is_quitting = false;
    if( !is_quitting )
    {
      is_quitting = true;
      LOG_INFO( "Exit with signal: " + boost::lexical_cast<int>(signal_type), utils::logging::LOG_HEADER_MAIN );

      g_service.stop();

      // Save pending messages if requested
      if( g_configs_loader->getProperty<bool>( server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE, server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE_DEFAULT ) )
      {
        const std::string& conf_dir_path                 = g_configs_loader->getConfFilesDir();
        const std::string pass_through_messages_filepath = conf_dir_path + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE, server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE_DEFAULT );
        const std::string maths_messages_filepath        = conf_dir_path + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE, server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE_DEFAULT );
        g_router->serialize( pass_through_messages_filepath, maths_messages_filepath );
      }
    }
    else
    {
      LOG_WARNING( "Force quit", utils::logging::LOG_HEADER_MAIN );
      exit( EXIT_FORCE_QUIT );
    }
  }
}

int main( int argc, char **argv )
{
  {
    // User want to see application version
    std::string version_commands[2] = { GRAPHITE_PROXY_VERSION_COMMAND, GRAPHITE_PROXY_VERSION_COMMAND_ALIAS };
    if( utils::cl::argsContain( argc, argv, version_commands, 2 ) )
    {
      utils::version::showVersion();
      return EXIT_SUCCESS;
    }
  }

  {
    // User want to see application help
    std::string help_commands[2] = { GRAPHITE_PROXY_HELP_COMMAND, GRAPHITE_PROXY_HELP_COMMAND_ALIAS };
    if( utils::cl::argsContain( argc, argv, help_commands, 2 ) )
    {
      utils::help::showHelp();
      return EXIT_SUCCESS;
    }
  }

  // Loading configurations
  g_configs_loader = boost::make_shared<server::utils::ConfigurationsLoader>( argc, argv );

  // Logger creation (call this before anything else ! because other classes need it to log)
  utils::logging::Logger::init( g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_LOGS_DESTINATION, server::props::PROPERTIES_LOGS_DESTINATION_DEFAULT ),
                                g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_LOGS_LEVEL, server::props::PROPERTIES_LOGS_LEVEL_DEFAULT ) );

  // The logger is ready, we can start logging
  if( !g_configs_loader->isValid() )
  {
    LOG_ERROR( "Bad configuration initialization, aborting.", utils::logging::LOG_HEADER_MAIN );
    return EXIT_BAD_CONFIGURATION;
  }

  // Setting logger color if required
  if( g_configs_loader->getProperty<bool>( server::props::PROPERTIES_LOGS_COLOR, server::props::PROPERTIES_LOGS_COLOR_DEFAULT ) )
  {
    utils::logging::Logger::setColor( server::networking::LOG_HEADER_SERVER,        utils::logging::Color::Red );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_CLIENT,            utils::logging::Color::Yellow );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_GLOBALBUFFER,      utils::logging::Color::Blue );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_TIMER,             utils::logging::Color::Green );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_STATISTICS,        utils::logging::Color::Purple );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_MATHS,             utils::logging::Color::Cyan );
    utils::logging::Logger::setColor( utils::logging::LOG_HEADER_MATHS_COMPUTATION, utils::logging::Color::Cyan );
  }

  LOG_DEBUG( std::string("Starting Graphite Proxy (PID ") + std::to_string(server::utils::SystemHelper::getProcessID()) + ")", utils::logging::LOG_HEADER_MAIN );

  // Client creation
  g_client = boost::make_shared<networking::Client>( g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_CLIENT_ADDRESS, "127.0.0.1" ),
                                                     g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_CLIENT_PORT, server::props::PROPERTIES_CLIENT_PORT_DEFAULT ) );

  if(!g_client->isReady())
  {
    LOG_ERROR( "Client miss configuration", utils::logging::LOG_HEADER_CLIENT );
    return EXIT_BAD_CONFIGURATION;
  }

  // Buffer creation
  g_buffer = boost::make_shared<GlobalBuffer>( g_configs_loader->getProperty<ulong>( server::props::PROPERTIES_BUFFER_MAX_ITEMS, server::props::PROPERTIES_BUFFER_MAX_ITEMS_DEFAULT ),
                                               g_configs_loader->getProperty<bool>( server::props::PROPERTIES_BUFFER_DROP_OLDER, server::props::PROPERTIES_BUFFER_DROP_OLDER_DEFAULT ),
                                               g_client );

  const std::string& config_dir = g_configs_loader->getConfFilesDir();

  // Maths creation
  if(g_configs_loader->getProperty<bool>( server::props::PROPERTIES_MATHS_ENABLE, server::props::PROPERTIES_MATHS_ENABLE_DEFAULT ))
  {
    g_maths = boost::make_shared<maths::MathsPipeline>( config_dir + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_MATHS_FILEPATH, server::props::PROPERTIES_MATHS_FILEPATH_DEFAULT ), g_buffer,
                                                        g_configs_loader->getProperty<ulong>( server::props::PROPERTIES_MATHS_SLEEP_TIME, server::props::PROPERTIES_MATHS_SLEEP_TIME_DEFAULT ),
                                                        g_configs_loader->getProperty<ulong>( server::props::PROPERTIES_MATHS_MAX_ITEMS, server::props::PROPERTIES_MATHS_MAX_ITEMS_DEFAULT ) );
  }
  else LOG_INFO( "Maths module disabled", utils::logging::LOG_HEADER_MATHS );

  // Router creation
  g_router = boost::make_shared<Router>( g_buffer, g_maths );

  // Server creation
  std::string address = g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_SERVER_ADDRESS, "127.0.0.1" );
  std::string port = g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_SERVER_PORT, server::props::PROPERTIES_SERVER_PORT_DEFAULT );
  g_tcp_server = boost::make_shared<server::networking::Server>( &g_service, g_router, address, port);

  // UDP server creation
  unsigned short udp_port;
  try { udp_port = boost::lexical_cast<unsigned short>(g_configs_loader->getProperty<std::string>(server::props::PROPERTIES_SERVER_UDP_PORT, server::props::PROPERTIES_SERVER_UDP_PORT_DEFAULT)); }
  catch (boost::bad_lexical_cast &)
  {
    LOG_ERROR("Unable to read udp port property", utils::logging::LOG_HEADER_MAIN);
    return EXIT_BAD_CONFIGURATION;
  }
  g_udp_server = boost::make_shared<server::networking::UDPServer>(g_service, g_router, udp_port);

  // Timer creation
  g_timer = boost::make_shared<Timer>( g_buffer, g_client, g_configs_loader->getProperty<long>( server::props::PROPERTIES_BUFFER_FLUSH_TIME, server::props::PROPERTIES_BUFFER_FLUSH_TIME_DEFAULT ) );
  LOG_INFO( "Flushing pass through messages every " + std::to_string( g_timer->getSleepTime() ) + " seconds", utils::logging::LOG_HEADER_TIMER );

  // Cleaner creation
  if(g_configs_loader->getProperty<bool>( server::props::PROPERTIES_BUFFER_CLEANER_ACTIVATED, server::props::PROPERTIES_BUFFER_CLEANER_ACTIVATED_DEFAULT ))
    g_cleaner = boost::make_shared<Cleaner>( g_buffer, g_maths
                                                     , g_configs_loader->getProperty<unsigned long>( server::props::PROPERTIES_BUFFER_CLEANER_TIME, server::props::PROPERTIES_BUFFER_CLEANER_TIME_DEFAULT )
                                                     , g_configs_loader->getProperty<unsigned long>( server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME, server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME_DEFAULT ));
  else LOG_INFO( "Cleaner module disabled", utils::logging::LOG_HEADER_CLEANER );

  // Statistics creation
  if(g_configs_loader->getProperty<bool>( server::props::PROPERTIES_STATS_ENABLE, server::props::PROPERTIES_STATS_ENABLE_DEFAULT ))
    g_stats = Statistics::init( g_buffer, g_maths, g_router, g_configs_loader->getProperty<long>( server::props::PROPERTIES_STATS_SLEEP_TIME, server::props::PROPERTIES_STATS_SLEEP_TIME_DEFAULT ), server::utils::SystemHelper::getHostname() );
  else LOG_INFO( "Statistics module disabled", utils::logging::LOG_HEADER_STATISTICS );

  // Signals
  signal( SIGINT,  signalsHandler ); // Signal to quit application
  signal( SIGTERM, signalsHandler ); // Signal to quit application
  signal( SIGQUIT, signalsHandler ); // Signal to quit application
  signal( SIGUSR1, signalsHandler ); // Signal to reload configurations
  signal( SIGUSR2, signalsHandler ); // Signal to dump current state in a file

  // Start the main timer
  g_timer->start();

  // Start Statistics module
  if(g_stats)
    g_stats->start();

  // Start Maths module
  if(g_maths)
    g_maths->start();

  // Start the cleaner module
  if(g_cleaner)
    g_cleaner->start();

  // Load previously saved messages if requested
  if( g_configs_loader->getProperty<bool>( server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE, server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE_DEFAULT ) )
  {
    g_router->load( config_dir + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE, server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE_DEFAULT ),
                    config_dir + g_configs_loader->getProperty<std::string>( server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE, server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE_DEFAULT) );
  }

  // Start the servers
  g_tcp_server->start();
  g_udp_server->start();
  g_service.run();

  return EXIT_SUCCESS;
}
