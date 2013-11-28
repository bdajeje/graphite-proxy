#include "router.hpp"

#include <graphite_proxy/models/statistics/statistics_metrics.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <fstream>
#include <stdio.h>

namespace graphite_proxy {

Router::Router( global_buffer_ptr global_buffer, maths::maths_ptr maths_pipeline )
  : m_global_buffer( global_buffer )
  , m_maths_pipeline( maths_pipeline )
{
  // Nothing
}

bool Router::routeMessage( const std::string& message ) const
{
  if( message.empty() )
    return false;

  return this->routeMessage( Message::createMessage(message) );
}

bool Router::routeMessage( const message_ptr message ) const
{
  if( message && message->isValid() )
  {
    STATS_INCREMENT( stats::STATS_MESSAGE_CREATED );

    if( m_maths_pipeline && m_maths_pipeline->isWanted( message->getType() ) )
    {
      LOG_DEBUG( "Route message to Maths: " + message->serialize(), utils::logging::LOG_HEADER_ROUTER );
      m_maths_pipeline->add( message );
    }
    else
    {
      LOG_DEBUG( "Route message to Global Buffer: " + message->serialize(), utils::logging::LOG_HEADER_ROUTER );
      m_global_buffer->add( message );
    }

    return true;
  }
  else
  {
    this->badSyntax( message );
    return false;
  }
}

int64_t Router::serialize( const std::string& pass_through_messages_filepath, const std::string& maths_messages_filepath )
{
  // Retrieve pending pass through messages into the program
  std::vector<message_ptr> messages;
  m_global_buffer->get( messages );
  long int nbr_saved_messages = this->serializeMessages( messages, pass_through_messages_filepath );
  if( nbr_saved_messages != -1 )
  {
    LOG_DEBUG( std::to_string(nbr_saved_messages) + " pass through messages saved", utils::logging::LOG_HEADER_ROUTER );
  }
  else
  {
    LOG_ERROR( std::string("Can't save pass through messages into ") + pass_through_messages_filepath, utils::logging::LOG_HEADER_ROUTER );
  }

  // Retrieve pending math messages into the program
  if( m_maths_pipeline )
  {
    // Remove pass through messages from the messages container
    messages.clear();

    // Get math buffers to prepare messages that will be saved with their suffixes
    const std::map<std::string, std::vector<maths::MathOperation*>>& math_buffers = m_maths_pipeline->getBuffers();
    std::vector<std::string> suffixes;
    for( auto& math_buffer : math_buffers )
    {
      const std::vector<maths::MathOperation*> math_operations = math_buffer.second;
      for( size_t j = 0, size = math_operations.size(); j < size; j++ )
      {
        const size_t previous_size = messages.size();
        MessageBuffer& buffer      = math_operations[j]->buffer;
        buffer.get(messages);
        const size_t nbr_added_messages = messages.size() - previous_size;
        if( nbr_added_messages > 0 )
        {
          // Add computation type as suffix for each messages
          const std::string& computation_type = math_operations[j]->computation.readType();
          for( size_t i = 0; i < nbr_added_messages; i++ )
            suffixes.push_back( computation_type );
        }
      }
    }

    // Save math messages
    long int math_saved_messages = this->serializeMessages( messages, maths_messages_filepath, suffixes );
    if( math_saved_messages != -1 )
    {
      LOG_DEBUG( std::to_string(math_saved_messages) + " maths messages saved", utils::logging::LOG_HEADER_ROUTER );
      nbr_saved_messages += math_saved_messages;
    }
    else
    {
      LOG_ERROR( std::string("Can't save math messages into ") + pass_through_messages_filepath, utils::logging::LOG_HEADER_ROUTER );
    }
  }

  return nbr_saved_messages;
}

long int Router::serializeMessages( const std::vector<message_ptr>& messages, const std::string& filename, const std::vector<std::string>& suffixes ) const
{
  if( messages.empty() )
    return 0;

  // Access save file
  std::ofstream file;
  file.open( filename.c_str() );
  if ( !file.is_open() )
  {
    LOG_ERROR( "Can't write in file to save messages (they will be lost): " + filename, utils::logging::LOG_HEADER_ROUTER );
    return -1;
  }

  // Write messages into the save file
  const std::vector<message_ptr>::size_type messages_count = messages.size();
  const std::vector<std::string>::size_type suffixes_count = suffixes.size();
  std::vector<message_ptr>::size_type i;
  for ( i = 0; i < messages_count; i++ )
  {
    // Print message
    file << messages[i]->serialize();

    // Add suffix if there is one
    if( i < suffixes_count )
      file << ' ' << suffixes[i];

    // End of line
    file << '\n';
  }
  file.close();

  return messages_count;
}

long int Router::load( const std::string& pass_through_messages_filepath, const std::string& maths_messages_filepath )
{
  // Load pass through messages
  long int loaded = this->loadMessages( pass_through_messages_filepath, false );

  // Load math messages
  if( m_maths_pipeline )
    loaded += this->loadMessages( maths_messages_filepath, true );

  return loaded;
}

long int Router::loadMessages( const std::string& filename, bool math ) const
{
  LOG_DEBUG( std::string("Loading saved messages from ") + filename, utils::logging::LOG_HEADER_ROUTER );

  // Access save file
  std::ifstream file;
  file.open( filename.c_str() );
  if ( !file.is_open() )
  {
    LOG_INFO( "Can't read file to load messages: " + filename, utils::logging::LOG_HEADER_ROUTER );
    return -1;
  }

  // Load each line
  long int loaded = 0;
  char line[MAX_MESSAGE_LENGTH];
  while( file.good() )
  {
    file.getline( line, MAX_MESSAGE_LENGTH );
    std::string line_str(line);
    if(line_str.empty())
      continue;

    // Send read message to the math pipeline (specific load message behavior) or to the router (normal load behavior)
    if( math )
    {
      // For math messages, split the line as we want to retrieve the computation type at the end of the line
      std::vector<std::string> split;
      boost::split(split, line_str, boost::is_any_of(" "));
      if( split.size() != 4 )
      {
        LOG_WARNING( std::string("Bad saved math message syntax: ") + line_str, utils::logging::LOG_HEADER_ROUTER );
        continue;
      }

      const std::string computation_type = split[3];
      const std::string message          = line_str.substr(0, line_str.length() - computation_type.length());
      if( m_maths_pipeline && m_maths_pipeline->loadMessage(Message::createMessage(message), computation_type) )
        loaded++;
    }
    else if( this->routeMessage( Message::createMessage(line_str) ) )
      loaded++;
  }

  // Close and delete file
  file.close();
  remove( filename.c_str() );

  LOG_DEBUG( std::to_string( loaded ) + " messages loaded", utils::logging::LOG_HEADER_ROUTER );
  return loaded;
}

void Router::badSyntax( const message_ptr message ) const
{
  if( message )
  {
    LOG_WARNING( "Bad message syntax, drop it: " + message->serialize(), utils::logging::LOG_HEADER_REQUEST );
  }
  else
  {
    LOG_WARNING( "Bad message syntax", utils::logging::LOG_HEADER_REQUEST );
  }

  STATS_INCREMENT( stats::STATS_REQUESTS_DROPPED );
}

} // namespace graphite_proxy
