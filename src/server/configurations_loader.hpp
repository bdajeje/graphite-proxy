#ifndef GRAPHITE_PROXY_CONFIGURATIONSLOADER_HPP
#define GRAPHITE_PROXY_CONFIGURATIONSLOADER_HPP

#include "properties.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <string>
#include <map>
#include <istream>

namespace server {
namespace utils {

/*! Helper class to load configurations from argc/argc or configuration file
 *  \note The configurations will first be loaded from configurations file then from command line.
 *  \note The command line configurations override those load from the configuration file
 */
class ConfigurationsLoader
{
  public:

    /*! Constructor
     *  \param argc from command line
     *  \param argv from command line
     */
    ConfigurationsLoader( int argc, char **argv );

    /*! Alternative constructor
     *  \param params is a list of string parameters representing argv (like ["--logs.level", "warning", "--statistics.enabled", "true"])
     */
    ConfigurationsLoader( const std::vector<std::string> &params );

    /*! Get a specific property by key
     *  \param key is the property name to search for into the configurations
     *  \param default_value is the value to return if the property hasn't been found
     *  \return the value associated to the key or an empty string if the property can't be found
     */
    template <class T> const T getProperty( const std::string &key, T default_value ) const;

    /*! Set a property thanks to its key (name) and value
     *  \param key is the name of the configuration to add
     *  \param value is the value of the configuration to add
     *  \return true if the property has been added
     *  \note setting a property with an already existing key will override the old value
     */
    template <class T> bool setProperty( const std::string &key, T value );

    /*! Get found configuration files directory from argv
     *  \return the found configuration files directory from argv
     */
    const std::string& getConfFilesDir() const { return m_config_directory; }

    /*! Does the configuration has been loaded correctly ?
     *  \return true if the configuration has been loaded correctly
     *  \note the configuration is considered loaded if the config container contains the PROPERTIES_CONFIGURATIONS_FILEPATH entry
     */
    bool isValid() const { return (m_configs.find(server::props::PROPERTIES_CONFIGURATIONS_FILEPATH) != m_configs.end()); }

  protected:

    /*! Initializer called by constructors
     *  \ref ConfigurationsLoader
     */
    void inititialize( const std::vector<std::string> &params );

    /*! Load configurations from configurations file
     *  \param filepath is the path to the configurations file
     *  \return true if the configuration file could have been read, false otherwise
     */
    bool loadFromConfigFile( const std::string &filepath );

    /*! Find an argument by its name into a list of arguments
     *  \param arguments are       the arguments given by argc/argv
     *  \param argument_name       is the name to look for
     *  \param argument_name_alias [optional] is the alias name to look for
     *  \return the value of the searched argument or empty string if not found
     */
    std::string findArg( const std::vector<std::string> &arguments, const std::string &argument_name, const std::string &argument_name_alias = std::string() ) const;

  private:

    /*! Private default constructor */
    ConfigurationsLoader();

  private:

    /*! Configurations container.
     *  \note the first string represents an configuration name
     *  \note the second string represents its value
     */
    std::map<std::string, std::string> m_configs;

    /*! Configuration files directory */
    std::string                        m_config_directory;
};

template <class T>
const T ConfigurationsLoader::getProperty( const std::string &key, T default_value ) const
{
  if (m_configs.count(key) == 0)
  {
    std::cerr << "Trying to find an unknown configuration: " << key << std::endl;
    return default_value;
  }

  try
  {
    std::string value = m_configs.at(key);

    // Specific changes for boolean values
    if ( value == "true" )
      value = "1";
    else if ( value == "false" )
      value = "0";

    return boost::lexical_cast<T>( value );
  }
  catch ( const boost::bad_lexical_cast &e )
  {
    std::cerr << "Properties error, bad " << key << ": " << e.what() << std::endl;
    return default_value;
  }
}

template <class T>
bool ConfigurationsLoader::setProperty( const std::string &key, T value )
{
  if (key.empty())
    return false;

  try
  {
    std::string string_value = std::to_string(value);
    m_configs[key] = string_value;
    return true;
  }
  catch( const boost::bad_lexical_cast &e )
  {
    std::cerr << "Can't set property " << key << ": " << e.what() << std::endl;
    return false;
  }
}

} // namespace utils
} // namespace server

#endif // GRAPHITE_PROXY_CONFIGURATIONSLOADER_HPP
