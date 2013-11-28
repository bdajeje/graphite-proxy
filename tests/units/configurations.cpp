#include <boost/test/unit_test.hpp>

#include <configurations_loader.hpp>
#include <properties.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <string>
#include <vector>

using namespace graphite_proxy;
using namespace server::props;

// Test loading configuration from config file
BOOST_AUTO_TEST_CASE( read_conf_file )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  std::vector<std::string> params;
  params.push_back("something"); // Always give a first param to argc/argv the application dir path (even faked)
  params.push_back(PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG);
  params.push_back("./");
  server::utils::ConfigurationsLoader configs_loader( params );

  BOOST_CHECK( configs_loader.isValid() );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_SERVER_ADDRESS, "" ), "127.0.0.1" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_SERVER_PORT, "" ), "8080" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_CLIENT_ADDRESS, "" ), "127.0.0.1" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_CLIENT_PORT, "" ), "8081" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_BUFFER_FLUSH_TIME, "" ), "2" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_BUFFER_MAX_ITEMS, "" ), "10000" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_BUFFER_DROP_OLDER, "" ), "1" );
  BOOST_CHECK_EQUAL( configs_loader.getProperty<std::string>( PROPERTIES_LOGS_LEVEL, "" ), "INFO" );
}

// Test loading configuration from config file without specified '/' at the end of the config file path parameter
BOOST_AUTO_TEST_CASE( read_conf_file_without_end_character )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  std::vector<std::string> params;
  params.push_back("something"); // Always give a first param to argc/argv the application dir path (even faked)
  params.push_back(PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG);
  params.push_back(".");
  server::utils::ConfigurationsLoader configs_loader( params );

  BOOST_CHECK( configs_loader.isValid() );
}

// Test can't read configurations from config file
BOOST_AUTO_TEST_CASE( cant_read_conf_file )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Give a wrong configuration files directory so the conf file can't be found
  std::vector<std::string> params;
  params.push_back("something"); // Always give a first param to argc/argv the application dir path (even faked)
  params.push_back(PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG);
  params.push_back("/tmp");
  server::utils::ConfigurationsLoader configs_loader( params );

  BOOST_CHECK( !configs_loader.isValid() );
}
