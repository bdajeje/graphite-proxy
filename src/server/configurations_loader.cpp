#include "configurations_loader.hpp"

#include <boost/property_tree/xml_parser.hpp>

#include <fstream>

namespace server {
namespace utils {

ConfigurationsLoader::ConfigurationsLoader( int argc, char **argv )
{
  std::vector<std::string> params;
  params.reserve(argc);
  for(int i = 0; i < argc; i++)
    params.push_back( std::string( argv[i] ) );

  this->inititialize( params );
}

ConfigurationsLoader::ConfigurationsLoader( const std::vector<std::string> &params )
{
  this->inititialize( params );
}

void ConfigurationsLoader::inititialize( const std::vector<std::string> &params )
{
  // Init default values
  m_configs[server::props::PROPERTIES_SERVER_ADDRESS]                = "";
  m_configs[server::props::PROPERTIES_SERVER_PORT]                   = server::props::PROPERTIES_SERVER_PORT_DEFAULT;
  m_configs[server::props::PROPERTIES_CLIENT_ADDRESS]                = "";
  m_configs[server::props::PROPERTIES_CLIENT_PORT]                   = "";
  m_configs[server::props::PROPERTIES_BUFFER_MAX_ITEMS]              = std::to_string( server::props::PROPERTIES_BUFFER_MAX_ITEMS_DEFAULT );
  m_configs[server::props::PROPERTIES_BUFFER_FLUSH_TIME]             = std::to_string( server::props::PROPERTIES_BUFFER_FLUSH_TIME_DEFAULT );
  m_configs[server::props::PROPERTIES_BUFFER_DROP_OLDER]             = std::to_string( server::props::PROPERTIES_BUFFER_DROP_OLDER_DEFAULT );
  m_configs[server::props::PROPERTIES_LOGS_LEVEL]                    = server::props::PROPERTIES_LOGS_LEVEL_DEFAULT;
  m_configs[server::props::PROPERTIES_LOGS_COLOR]                    = std::to_string( server::props::PROPERTIES_LOGS_COLOR_DEFAULT );
  m_configs[server::props::PROPERTIES_LOGS_DESTINATION]              = server::props::PROPERTIES_LOGS_DESTINATION_DEFAULT;
  m_configs[server::props::PROPERTIES_STATS_ENABLE]                  = std::to_string( server::props::PROPERTIES_STATS_ENABLE_DEFAULT );
  m_configs[server::props::PROPERTIES_STATS_SLEEP_TIME]              = std::to_string( server::props::PROPERTIES_STATS_SLEEP_TIME_DEFAULT );
  m_configs[server::props::PROPERTIES_MATHS_ENABLE]                  = std::to_string( server::props::PROPERTIES_MATHS_ENABLE_DEFAULT );
  m_configs[server::props::PROPERTIES_MATHS_MAX_ITEMS]               = std::to_string( server::props::PROPERTIES_MATHS_MAX_ITEMS_DEFAULT );
  m_configs[server::props::PROPERTIES_MATHS_SLEEP_TIME]              = std::to_string( server::props::PROPERTIES_MATHS_SLEEP_TIME_DEFAULT );
  m_configs[server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE]          = std::to_string( server::props::PROPERTIES_ROUTER_SAVE_ON_CLOSE_DEFAULT );
  m_configs[server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE]  = server::props::PROPERTIES_ROUTER_PASSTHROUGH_SAVE_FILE_DEFAULT;
  m_configs[server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE]        = server::props::PROPERTIES_ROUTER_MATHS_SAVE_FILE_DEFAULT;
  m_configs[server::props::PROPERTIES_SIGNALS_CURRENT_STATE_FILE]    = server::props::PROPERTIES_SIGNALS_CURRENT_STATE_FILE_DEFAULT;
  m_configs[server::props::PROPERTIES_SERVER_UDP_PORT]               = server::props::PROPERTIES_SERVER_UDP_PORT_DEFAULT;
  m_configs[server::props::PROPERTIES_BUFFER_CLEANER_ACTIVATED]      = std::to_string( server::props::PROPERTIES_BUFFER_CLEANER_ACTIVATED_DEFAULT );
  m_configs[server::props::PROPERTIES_BUFFER_CLEANER_TIME]           = std::to_string( server::props::PROPERTIES_BUFFER_CLEANER_TIME_DEFAULT );
  m_configs[server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME] = std::to_string( server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME_DEFAULT );

  // Find the configuration files directory
  m_config_directory = this->findArg( params, server::props::PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG, server::props::PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG_ALIAS );
  if( m_config_directory.empty() )
  {
    std::cerr << "No argument -c or --config-dir found. Run 'graphite-proxy --help'' to show the help" << std::endl;
    return;
  }
  else if( m_config_directory[m_config_directory.length()-1] != '/' )
  {
    // Check that the config directory ends with a '/'
    m_config_directory += '/';
  }

  // Try to load main configuration file
  std::string conf_filepath = m_config_directory + server::props::PROPERTIES_CONFIGURATIONS_FILEPATH_DEFAULT;
  if( this->loadFromConfigFile( conf_filepath ) )
  {
    // Add properties not found in configurations file
    m_configs[server::props::PROPERTIES_CONFIGURATIONS_FILEPATH] = server::props::PROPERTIES_CONFIGURATIONS_FILEPATH_DEFAULT;
    m_configs[server::props::PROPERTIES_MATHS_FILEPATH]          = server::props::PROPERTIES_MATHS_FILEPATH_DEFAULT;
  }
}

std::string ConfigurationsLoader::findArg( const std::vector<std::string>& arguments, const std::string &argument_name, const std::string &argument_name_alias ) const
{
  if( arguments.empty() )
    return std::string();

  std::vector<std::string>::size_type i;
  std::vector<std::string>::size_type size = arguments.size() - 1;

  for( i = 1; i < size; i += 2 )
  {
    const std::string& argument = arguments.at(i);
    if ( argument == argument_name || argument == argument_name_alias )
      return arguments[i + 1];
  }

  return std::string();
}

bool ConfigurationsLoader::loadFromConfigFile( const std::string &filepath )
{
  std::ifstream file( filepath.c_str() );

  if( !file.is_open() )
  {
    std::cerr << "Can't read configuration file: " + filepath << std::endl;
    return false;
  }

  // Read file and put results inside a boost property tree
  boost::property_tree::ptree tree;
  try
  {
    boost::property_tree::xml_parser::read_xml( file, tree );
  }
  catch ( const boost::property_tree::xml_parser::xml_parser_error &e )
  {
    file.close();
    std::cerr << "Can't parse configuration file: " << e.what() << std::endl;
    return false;
  }

  file.close();

  // Get results
  boost::property_tree::ptree default_value;
  boost::property_tree::ptree root = tree.get_child( server::props::PROPERTIES_ROOT, default_value );
  if( root == default_value )
  {
    std::cerr << "Can't find " << server::props::PROPERTIES_ROOT << " node" << std::endl;
    return false;
  }

  std::string found, key;
  for( auto it = m_configs.begin(); it != m_configs.end(); ++it )
  {
    key   = it->first;
    found = root.get<std::string>( key, "" );

    if (found.empty())
    {
      m_configs[key] = it->second;
      std::cerr << "Can't find property " << key << ", set default value " << it->second << std::endl;
    }
    else
    {
      m_configs[key] = found;
    }
  }

  return true;
}

} // namespace utils
} // namespace server
