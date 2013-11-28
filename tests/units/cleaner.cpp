#include <boost/test/unit_test.hpp>

#include <graphite_proxy/models/buffers/cleaner.hpp>

#include <properties.hpp>
#include <configurations_loader.hpp>

#include <boost/make_shared.hpp>

using namespace graphite_proxy;

/*! Test cleaner loading configuration */
BOOST_AUTO_TEST_CASE( cleaner_configuration )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Load the config
  std::vector<std::string> params;
  params.push_back( "" );
  params.push_back(server::props::PROPERTIES_CONFIGURATIONS_FILES_DIR_ARG);
  params.push_back("./");
  params.push_back( "--buffer.cleaning.time" );
  params.push_back( "10" );
  params.push_back( "--buffer.cleaning.max-empty-time" );
  params.push_back( "30" );
  server::utils::ConfigurationsLoader configs(params);

  // Cleaner creation
  Cleaner cleaner( boost::shared_ptr<GlobalBuffer>(), boost::shared_ptr<maths::MathsPipeline>()
                                                    , configs.getProperty<unsigned long>( server::props::PROPERTIES_BUFFER_CLEANER_TIME, server::props::PROPERTIES_BUFFER_CLEANER_TIME_DEFAULT )
                                                    , configs.getProperty<unsigned long>( server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME, server::props::PROPERTIES_BUFFER_CLEANER_MAX_EMPTY_TIME_DEFAULT ) );
  BOOST_CHECK_EQUAL( cleaner.getSleepTime(), 3 );
  BOOST_CHECK_EQUAL( cleaner.getMaxEmptyTime(), 3 );
}

/*! Test cleaner minimum value for the max-empty-time configuration */
BOOST_AUTO_TEST_CASE( cleaner_min_value )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Cleaner creation
  Cleaner cleaner( boost::shared_ptr<GlobalBuffer>(), boost::shared_ptr<maths::MathsPipeline>(), 10, 0 );
  BOOST_CHECK_EQUAL( cleaner.getSleepTime(), 10 );
  BOOST_CHECK_EQUAL( cleaner.getMaxEmptyTime(), 2 );
}

/*! Test a clean from the cleaner */
BOOST_AUTO_TEST_CASE( cleaner_clean_global_buffer )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Add some messages into the global buffer
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 999, false, client );
  BOOST_CHECK(buffer->add( Message::createMessage("test.2 0 0") ));
  BOOST_CHECK(buffer->add( Message::createMessage("test.2 0 0") ));

  // Check the adding was correct
  std::map<std::string, unsigned long> buffer_sizes = buffer->getBufferSizes();
  BOOST_CHECK_EQUAL( buffer_sizes.at("test.2"), 2 );

  // Remove messages from the buffer
  std::vector<message_ptr> storage;
  buffer->get("test.2", storage);
  BOOST_CHECK_EQUAL( storage.size(), 2 );

  // Launch some iterations
  Cleaner cleaner( buffer, boost::shared_ptr<maths::MathsPipeline>(), 1, 2 );
  cleaner.iterate();
  cleaner.iterate();

  // The empty buffer has been deleted, verify it
  buffer_sizes = buffer->getBufferSizes();
  BOOST_CHECK( buffer_sizes.empty() );
}

/*! Test a clean from the cleaner */
BOOST_AUTO_TEST_CASE( cleaner_clean_math_pipeline )
{
  utils::logging::Logger::init( "", utils::logging::Logger::LogLevel::QUIET );

  // Add some messages into the math pipeline
  networking::client_ptr client = boost::make_shared<networking::Client>( "localhost", "2003" );
  global_buffer_ptr buffer      = boost::make_shared<GlobalBuffer>( 999, false, client );
  maths::MathsPipeline pipeline( "conf/maths_cleaner.xml", buffer, 99, 99 );
  BOOST_CHECK(pipeline.add( Message::createMessage("nani 0 0") ));
  BOOST_CHECK(pipeline.add( Message::createMessage("nana 0 0") ));
  BOOST_CHECK(pipeline.add( Message::createMessage("nana 0 0") ));

  // Remove messages from the math pipeline (there are going into the Global Buffer)
  pipeline.iterate();

  // Launch some cleaner iterations to clean the math pipeline buffers
  Cleaner cleaner( buffer, boost::shared_ptr<maths::MathsPipeline>(), 1, 2 );
  cleaner.iterate();
  cleaner.iterate();

  // Get remaining messages from the math pipeline. It should remain only one message the "ads_server.nani.1" one.
  std::vector<message_ptr> target_container;
  pipeline.get(target_container);
  BOOST_REQUIRE(!target_container.empty());
  BOOST_CHECK_EQUAL(target_container.size(), 1);
  BOOST_CHECK_EQUAL(target_container[0]->getType(), "nani");
}
