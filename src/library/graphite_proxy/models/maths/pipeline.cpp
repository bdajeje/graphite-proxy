#include "pipeline.hpp"

#include <graphite_proxy/utils/time.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <graphite_proxy/models/statistics/statistics_metrics.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>

#include <algorithm>
#include <iostream>
#include <math.h>

namespace graphite_proxy {
namespace maths {

MathsPipeline::MathsPipeline( const std::string &conf_filepath, global_buffer_ptr global_buffer, unsigned long sleep_time, unsigned long buffer_max_size )
  : Iterations( sleep_time, utils::logging::LOG_HEADER_MATHS )
  , m_buffer( global_buffer )
  , m_buffer_max_size( buffer_max_size )
{
  m_valid = this->loadConfigurations( conf_filepath );

  if ( !m_valid )
    LOG_ERROR( "Maths configuration file can't be loaded", m_name );
}

MathsPipeline::~MathsPipeline()
{
  // Delete categories
  this->clearCategories();

  // Delete buffers
  for( auto it = m_buffers.begin(); it != m_buffers.end(); ++it )
  {
    std::vector<MathOperation*>& operations = it->second;
    for( size_t i = 0, size = operations.size(); i < size; i++ )
      delete operations[i];
  }
}

void MathsPipeline::clearCategories()
{
  // Delete each category pointer
  std::list<MathsCategory*>::iterator it;
  for( it = m_categories.begin(); it != m_categories.end(); ++it )
    delete *it;

  m_categories.clear();
}

bool MathsPipeline::reloadConfigurations( const std::string &conf_filepath )
{
  LOG_DEBUG( "Reloading configurations from", m_name );

  boost::mutex::scoped_lock lock( m_mutex );

  // Reset previous configurations
  this->clearCategories();

  // Load configurations
  return this->loadConfigurations( conf_filepath );
}

bool MathsPipeline::loadConfigurations( const std::string &conf_filepath )
{
  std::ifstream file( conf_filepath.c_str() );

  if (file.is_open())
  {
    LOG_DEBUG( "Reading Maths configuration file: " + conf_filepath, m_name );
  }
  else
  {
    LOG_ERROR( "Can't found maths configuration file: " + conf_filepath, m_name );
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
    LOG_ERROR( std::string("Can't parse maths configuration file: ") + e.what(), m_name );
    return false;
  }

  file.close();

  // Find root node
  boost::property_tree::ptree root;
  boost::property_tree::ptree default_value;
  root = tree.get_child( NODE_ROOT, default_value );
  if (root == default_value)
  {
    LOG_ERROR( std::string("Can't find ") + NODE_ROOT + " node", m_name );
    return false;
  }

  // Prepare some variables and data for the load
  ulong value;
  bool time_value;
  const ulong init_compute_time     = boost::chrono::duration_cast<boost::chrono::seconds>( boost::chrono::system_clock::now().time_since_epoch() ).count();
  const std::string name_property   = std::string("<xmlattr>.") + ATTRIBUTE_NAME;
  const std::string below_property  = std::string("<xmlattr>.") + ATTRIBUTE_BELOW;
  const std::string multi_property  = std::string("<xmlattr>.") + ATTRIBUTE_MULTIPLICATOR;
  const std::string value_property  = std::string("<xmlattr>.") + ATTRIBUTE_VALUE;

  const boost::regex regex_integer( REGEX_INTEGER );
  const boost::regex regex_time( REGEX_TIME );

  // Go throw each "category" node
  for( boost::property_tree::ptree::iterator it = root.begin(); it != root.end(); ++it )
  {
    // Is it a "category" node?
    if (it->first != NODE_CATEGORY)
      continue;

    // Retrieve category node
    boost::property_tree::ptree &category_node = it->second;

    // Does this node have a name?
    const std::string category_filter_name = category_node.get( name_property, "" );
    if (category_filter_name == "")
    {
      LOG_WARNING( "Category without a name, ignored.", m_name );
      continue;
    }

    // If the node is empty ignored
    if( category_node.empty() )
    {
      LOG_WARNING( std::string("Category: ") + category_filter_name + " empty, ignored.", m_name );
      continue;
    }

    // Create the category
    MathsCategory* category = new MathsCategory(category_filter_name);
    LOG_DEBUG( std::string("Loading category: ") + category_filter_name, m_name );

    // Go throw each maths operation for this category node
    for( boost::property_tree::ptree::iterator operation_it = category_node.begin(); operation_it != category_node.end(); ++operation_it )
    {
      const std::string& computation_string = operation_it->first;

      // Ignore <xmlattr> nodes
      if( computation_string == "<xmlattr>" )
        continue;

      // If unknown maths operation ignored and continue
      const ComputationType computation_type = MathComputation::stringToComputationType( computation_string );
      if ( computation_type == UNKNOWN )
        continue;

      // Get maths operation node value
      boost::property_tree::ptree &operation_node = operation_it->second;
      std::string operation_value = operation_node.get_value("");
      if ( operation_value == "" )
        continue;

      // Is this value a number or a time value ? (or something we don't want)
      if ( boost::regex_match( operation_value, regex_integer ) )
      {
        value = boost::lexical_cast<ulong>(operation_value);
        time_value = false;
      }
      else if( boost::regex_match( operation_value, regex_time ) )
      {
        value = utils::time::parseTime( operation_value );
        time_value = true;
      }
      else continue;

      // We need at least a time value or a count value to create a MathComputation
      if ( (time_value && value > ATTRIBUTE_TIME_MIN_VALUE) || (!time_value && value > ATTRIBUTE_MIN_VALUE) )
      {
        // Add the found Math Computation to the current category
        MathComputation computation( computation_type, time_value, value, init_compute_time );

        // Looking for specific MathComputation options (setting default values if attribute is not setted)
        if ( computation_type == TILES )
        {
          std::string attribute_value = operation_node.get( value_property, "" );
          if ( !attribute_value.empty() )
          {
            computation.addOption( ATTRIBUTE_VALUE, attribute_value );
            computation.addOption( ATTRIBUTE_BELOW, category_node.get( below_property, ATTRIBUTE_DEFAULT_BELOW ) );
            computation.addOption( ATTRIBUTE_MULTIPLICATOR, category_node.get( multi_property, ATTRIBUTE_DEFAULT_MULTIPLICATOR ) );
          }
          else LOG_ERROR( computation_string + " need a '" + ATTRIBUTE_VALUE + "' attribute in category: " + category_filter_name, m_name );
        }

        LOG_DEBUG( std::string("Adding computation: ") + computation_string, m_name );
        category->addComputation( computation );
      }
    }

    // If the category has some computations keep it otherwise delete it
    if( category->empty() )
    {
      LOG_DEBUG( std::string("Category: ") + category_filter_name + " empty so delete it", m_name );
      delete category;
    }
    else
    {
      LOG_DEBUG( std::string("Creating category: ") + category_filter_name, m_name );
      m_categories.push_back( category );
    }
  }

  if ( m_categories.empty() )
  {
    LOG_WARNING( "No categories to load, disable Maths module", m_name );
    return false;
  }
  else
  {
    return true;
  }
}

void MathsPipeline::iteration()
{
  LOG_DEBUG( "Starting new maths computing iteration", m_name );
  const ulong now = utils::time::now();
  boost::mutex::scoped_lock lock( m_mutex );

  // Inspect each buffer
  for( auto it = m_buffers.begin(); it != m_buffers.end(); ++it )
  {
    // Retrieve Maths Computations of this buffer
    const std::string&          message_type = it->first;
    std::vector<MathOperation*>& operations  = it->second;

    // Inspect each operations to do with this buffer
    for( size_t i = 0, operations_size = operations.size(); i < operations_size; i++ )
    {
      MathComputation& computation  = operations[i]->computation;
      MessageBuffer& message_buffer = operations[i]->buffer;

      if( computation.isOnCount() )
      {
        // Computation on number of received messages
        const size_t buffer_size       = message_buffer.size();
        const size_t computation_count = computation.getCount();

        if( buffer_size >= computation_count )
          this->computeOnCount( computation, message_buffer );
        else
          LOG_DEBUG( message_type + " => onCount not ready, need " + std::to_string(computation_count) + " <= " + std::to_string(buffer_size), m_name );
      }
      else if( computation.isOnTimeIteration() )
      {
        // Computation on time iterations
        const ulong next_iteration_time = computation.nextIterationTime();

        if( now >= next_iteration_time )
          this->computeOnTime( computation, message_buffer, now );
        else
        {
          if( !message_buffer.empty() )
            LOG_DEBUG( message_type + " => onTime not ready, need " + std::to_string(next_iteration_time) + " <= " + std::to_string(now), m_name );
        }
      }
    }
  }
}

void MathsPipeline::computeOnCount( MathComputation& computation, MessageBuffer& message_buffer )
{
  std::vector<message_ptr> messages;
  message_buffer.get( messages, computation.getCount() );

  LOG_DEBUG( message_buffer.getName() + " => onCount of " + std::to_string(messages.size()) + " messages", m_name );

  this->compute( messages, computation );
}

void MathsPipeline::computeOnTime( MathComputation& computation, MessageBuffer& message_buffer, unsigned long now )
{
  // Compute messages for each window of iteration time
  do
  {
    std::vector<message_ptr> messages;
    message_buffer.getOlderThan( messages, computation.nextIterationTime() );

    if( messages.empty() )
    {
      LOG_DEBUG( message_buffer.getName() + " => onTime no old enougth messages to compute", m_name );
      computation.setLastComputeTime( now );
      break;
    }
    else
    {
      LOG_DEBUG( message_buffer.getName() + " => onTime of " + std::to_string(messages.size()) + " messages to compute", m_name );
      this->compute( messages, computation );
      computation.incrementLastComputeTime();
    }
  }
  while( now > computation.nextIterationTime() );
}

void MathsPipeline::get( std::vector<message_ptr> &target_container )
{
  boost::mutex::scoped_lock lock( m_mutex );

  for( auto it = m_buffers.begin(); it != m_buffers.end(); ++it )
  {
    std::vector<MathOperation*>& operations = it->second;
    for( size_t j = 0, operations_size = operations.size(); j < operations_size; j++ )
      operations[j]->buffer.get( target_container );
  }
}

bool MathsPipeline::add( message_ptr message, const std::string& computation_type )
{
  if(!m_valid)
  {
    LOG_ERROR( "Unvalid state, can't accept any message", m_name );
    return false;
  }

  // Does this message expected?
  const std::string& message_type = message->getType();
  MathsCategory* category = this->isWanted(message_type);
  if( !category )
  {
    LOG_INFO( "Message type not accepted by math module: " + message_type, m_name );
    return false;
  }
  else
  {
    LOG_INFO( "Message accepted by category: " + category->getFilter().str(), m_name );
  }

  // Does a buffer exist for this message type?
  auto found_buffer = m_buffers.find(message_type);
  std::vector<MathOperation*>* buffer = nullptr;

  // No buffer exists yet for this kind of message, let's create it
  if( found_buffer == m_buffers.end() )
  {
    LOG_DEBUG( "No math buffer exists yet for this kind of messages. Creating it.", m_name );

    // Create the buffer
    buffer = &(m_buffers[message_type]);

    // Add operations of this category to the buffer
    const std::vector<MathComputation>& computations = category->getComputations();
    for( size_t i = 0, size = computations.size(); i < size; i++ )
    {
      const MathComputation& computation  = computations[i];
      const std::string buffer_name       = message_type + " " + computation.readType();
      LOG_DEBUG( "Creating math operation: " + buffer_name, m_name );
      buffer->push_back( new MathOperation(computation, buffer_name, m_buffer_max_size) );
    }
  }
  else
  {
    buffer = &(found_buffer->second);
  }

  // If a computation type is specified let's select only the buffer corresponding to this computation
  if( !computation_type.empty() )
  {
    for( size_t i = 0, size = buffer->size(); i < size; i++ )
    {
      if( buffer->operator[](i)->computation.readType() == computation_type )
      {
        MessageBuffer& message_buffer = buffer->operator[](i)->buffer;
        LOG_DEBUG( "Add message to math buffer: " + message_buffer.getName(), m_name );
        message_buffer.add( message );
        break;
      }
    }
  }
  else
  {
    // Add message to the buffers
    for( size_t i = 0, size = buffer->size(); i < size; i++ )
    {
      MessageBuffer& math_buffer = buffer->operator[](i)->buffer;
      LOG_DEBUG( "Add message to math buffer: " + math_buffer.getName(), m_name );
      math_buffer.add( message );
    }
  }

  return true;
}

bool MathsPipeline::loadMessage( message_ptr message, const std::string& computation_type )
{
  // Valid message?
  if( !message && !computation_type.empty() )
    return false;

  return this->add(message, computation_type);
}

MathsCategory* MathsPipeline::isWanted( const std::string &message_type ) const
{
  boost::mutex::scoped_lock lock( m_mutex );

  for( auto it = m_categories.begin(); it != m_categories.end(); ++it )
  {
    MathsCategory* category = *it;
    if( boost::regex_match( message_type, category->getFilter() )  )
      return category;
  }

  return nullptr;
}

void MathsPipeline::compute( const std::vector<message_ptr> &messages, const MathComputation& computation )
{
  if ( messages.empty() )
    return;

  bool added = true;

  // Wich math computation?
  switch ( computation.getType() )
  {
    case SUM:
      STATS_INCREMENT( stats::STATS_MATHS_SUM );
      m_buffer->add( this->sum( messages ) );
      break;
    case AVERAGE:
      STATS_INCREMENT( stats::STATS_MATHS_AVERAGE );
      m_buffer->add( this->average( messages ) );
      break;
    case MAX:
      STATS_INCREMENT( stats::STATS_MATHS_MAX );
      m_buffer->add( this->max( messages ) );
      break;
    case MIN:
      STATS_INCREMENT( stats::STATS_MATHS_MIN );
      m_buffer->add( this->min( messages ) );
      break;
    case MEDIAN:
      STATS_INCREMENT( stats::STATS_MATHS_MEDIAN );
      m_buffer->add( this->median( messages ) );
      break;
    case VARIANCE:
      STATS_INCREMENT( stats::STATS_MATHS_VARIANCE );
      m_buffer->add( this->variance( messages ) );
      break;
    case DEVIATION:
      STATS_INCREMENT( stats::STATS_MATHS_DEVIATION );
      m_buffer->add( this->deviation( messages ) );
      break;
    case TILES:
      STATS_INCREMENT( stats::STATS_MATHS_TILES );
      double value, multiplicator;
      bool   below;

      try
      {
        std::string below_value = computation.getOption( ATTRIBUTE_BELOW );
        if ( below_value.empty() ) break;
        else if ( below_value == "true" ) below_value = "1";
        else if ( below_value == "false" ) below_value = "0";

        value         = boost::lexical_cast<double>( computation.getOption( ATTRIBUTE_VALUE ) );
        multiplicator = boost::lexical_cast<double>( computation.getOption( ATTRIBUTE_MULTIPLICATOR ) );
        below         = boost::lexical_cast<bool>( below_value );
      }
      catch ( const boost::bad_lexical_cast &e )
      {
        LOG_ERROR( std::string("Lexical cast error while reading math computation (" + computation.serialize() + ") option value: ") + e.what(), m_name );
        break;
      }

      m_buffer->add( this->tiles( messages, value, below, multiplicator ) );
      break;
    default:
      added = false;
      LOG_ERROR( "Unknown Math Computation: " + computation.serialize() , m_name );
  }

  if( added )
    STATS_INCREMENT( stats::STATS_MATHS_MESSAGES );
}

message_ptr MathsPipeline::sum( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  double result = messages[0]->getValue();
  std::vector<message_ptr>::size_type i, size = messages.size();

  for( i = 1; i < size; i++ )
    result += messages[i]->getValue();

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::average( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  double result = messages[0]->getValue();

  const size_t size = messages.size();
  for( size_t i = 1; i < size; i++ )
    result += messages[i]->getValue();

  result /= size;

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::variance( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  // Get the average
  const double average = this->average(messages)->getValue();
  double result = 0.0;

  const size_t size = messages.size();
  for( size_t i = 0; i < size; i++ )
  {
    const double difference = messages[i]->getValue() - average;
    result += difference * difference;
  }

  result /= size;

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::deviation( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  // Deviation is only the square of the variance
  double result = sqrt( this->variance(messages)->getValue() );

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::max( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  double result = messages[0]->getValue();
  std::vector<message_ptr>::size_type i, size = messages.size();

  for( i = 1; i < size; i++ )
  {
    if ( messages[i]->getValue() > result )
      result = messages[i]->getValue();
  }

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::min( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  double result = messages[0]->getValue();

  const size_t size = messages.size();
  for( size_t i = 1; i < size; i++ )
  {
    if ( messages[i]->getValue() < result )
      result = messages[i]->getValue();
  }

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

message_ptr MathsPipeline::median( const std::vector<message_ptr> &messages ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  const size_t size = messages.size();
  const size_t middle = size / 2;

  // Inserting messages values in an second vector to order them thx to std::sort
  std::vector<double> values;
  values.reserve( size );
  for( size_t i = 0; i < size; i++ )
    values.push_back( messages[i]->getValue() );

  std::sort( values.begin(), values.end() );

  double value = ( size % 2 != 0 ) ? values[middle] : (values[middle] + values[middle - 1]) / 2;

  return boost::make_shared<Message>( messages[0]->getType(), value );
}

message_ptr MathsPipeline::tiles( const std::vector<message_ptr> &messages, double value, bool strictly_below, double multiplicator ) const
{
  if ( messages.empty() )
    return boost::make_shared<Message>();

  ulong nbr_below = 0;
  ulong nbr_equal = 0;

  const std::vector<message_ptr>::size_type size = messages.size();
  for( size_t i = 0; i < size; i++ )
  {
    if ( messages[i]->getValue() < value )
      nbr_below++;
    else if ( !strictly_below && messages[i]->getValue() == value )
      nbr_equal++;
  }

  double result = ( (nbr_below + 0.5 * nbr_equal) / size ) * multiplicator;

  return boost::make_shared<Message>( messages[0]->getType(), result );
}

unsigned long MathsPipeline::getBuffersMaxMessages() const
{
  unsigned long size, max = 0;

  {
    boost::mutex::scoped_lock lock( m_mutex );

    for( auto it = m_buffers.begin(); it != m_buffers.end(); ++it )
    {
      const std::vector<MathOperation*>& operations = it->second;
      for( size_t i = 0, operations_size = operations.size(); i < operations_size; i++ )
      {
        size = operations[i]->buffer.getMaxMessages();
        if( size > max )
          max = size;
      }
    }
  }

  return max;
}

void MathsPipeline::remove(const std::string& buffer_name)
{
  boost::mutex::scoped_lock lock( m_mutex );
  const unsigned long max = std::numeric_limits<unsigned long>::max();

  for( auto it = m_buffers.begin(); it != m_buffers.end(); ++it )
  {
    unsigned long found = max;
    std::vector<MathOperation*>& operations = it->second;
    for( size_t i = 0, operations_size = operations.size(); i < operations_size; i++ )
    {
      const MessageBuffer& buffer = operations[i]->buffer;
      if( buffer.getName() == buffer_name )
      {
        found = i;
        break;
      }
    }

    // We've found the buffer, let's erase it
    if(found != max)
    {
      // Delete and remove the operation
      delete operations[found];
      operations.erase( operations.begin() + found );

      // Operations become empty, remove the entire entry in m_buffers
      if(operations.empty())
        m_buffers.erase(it);

      break;
    }
  }
}

} // namespace maths
} // namespace graphite_proxy
