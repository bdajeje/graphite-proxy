#include "current_state.hpp"

#include <graphite_proxy/utils/time.hpp>
#include <graphite_proxy/utils/cast.hpp>
#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace server {
namespace utils {

CurrentState::CurrentState( const std::string& filepath, const graphite_proxy::networking::client_ptr client
                                                       , const graphite_proxy::global_buffer_ptr global_buffer
                                                       , const graphite_proxy::timer_ptr timer
                                                       , const graphite_proxy::statistics_ptr stats
                                                       , const graphite_proxy::maths::maths_ptr maths
                                                       , const server::networking::tcp_server_ptr tcp_server
                                                       , const server::networking::udp_server_ptr udp_server )
  : m_filepath(filepath)
  , m_client_is_ready(false)
  , m_gbuffer_drop_oldest(false)
  , m_gbuffer_max_size(0)
  , m_timer_flush_time(0)
  , m_stats_enabled(false)
  , m_maths_buffer_max_size(0)
{
  // Retrieve client informations
  if( client )
  {
    m_client_is_ready = client->isReady();
  }

  // Global buffer informations
  if( global_buffer )
  {
    m_gbuffer_drop_oldest  = global_buffer->isDropOldest();
    m_gbuffer_buffer_sizes = global_buffer->getBufferSizes();
    m_gbuffer_max_size     = global_buffer->getBuffersMaxSize();
  }

  // Timer informations
  if( timer )
  {
    m_timer_flush_time = timer->getSleepTime();
  }

  // Stats informations
  if( stats )
  {
    m_stats_enabled = stats->enabled();
  }

  // Maths informations
  if( maths )
  {
    m_maths_buffer_max_size = maths->getBuffersMaxSize();
    m_maths_categories      = maths->getCategories();
    m_maths_buffers         = maths->getBuffers();
  }

  // TCP server informations
  if( tcp_server )
  {
    m_tcp_server_info.address = tcp_server->getAddress();
    m_tcp_server_info.port    = tcp_server->getPort();
  }

  // UDP server informations
  if( udp_server )
  {
    m_udp_server_info.address = m_tcp_server_info.address;
    m_udp_server_info.port    = udp_server->getPort();
  }
}

std::string CurrentState::save() const
{
  std::stringstream result;
  result << "Graphite Proxy current state " << graphite_proxy::utils::time::humanDateTime() << std::endl << std::endl;
  result << this->showMaths() << std::endl;
  result << this->showServer() << std::endl;
  result << this->showClient() << std::endl;
  result << this->showTimer() << std::endl;
  result << this->showStats() << std::endl;
  result << this->showGlobalBuffer() << std::endl;

  // Save result in file
  std::ofstream file;
  file.open( m_filepath.c_str() );
  if( file.is_open() )
  {
    file << result.str();
    file.close();
  }
  else LOG_ERROR( "Can't save current state result", graphite_proxy::utils::logging::LOG_HEADER_CURRENT_STATE );

  return result.str();
}

std::string CurrentState::writeHeader( const std::string& header_name ) const
{
  unsigned short line_length = 80;
  std::string result;
  result.reserve( line_length );
  std::string first_part = "##### ";

  result = first_part + header_name + " ";
  for( unsigned short i = 0, size = line_length - header_name.size() - first_part.size(); i < size; i++ )
    result += '#';
  result += '\n';

  return result;
}

std::string CurrentState::showServer() const
{
  std::stringstream result;

  result << this->writeHeader("SERVER");

  result << "TCP Server" << std::endl;
  result << "address:  " << m_tcp_server_info.address << std::endl;
  result << "tcp port: " << m_tcp_server_info.port << std::endl;
  result << "udp port: " << m_udp_server_info.port << std::endl;

  return result.str();
}

std::string CurrentState::showClient() const
{
  std::stringstream result;

  result << this->writeHeader("CLIENT");
  result << "is ready: " << graphite_proxy::utils::cast::toString(m_client_is_ready, true) << std::endl;

  return result.str();
}

std::string CurrentState::showGlobalBuffer() const
{
  std::stringstream result;

  result << this->writeHeader("GLOBAL BUFFER");
  result << "drop oldest messages:              " << graphite_proxy::utils::cast::toString(m_gbuffer_drop_oldest, true) << std::endl;
  result << "max number of messages per buffer: " << m_gbuffer_max_size << std::endl;

  if( m_gbuffer_buffer_sizes.empty() )
  {
    result << "no buffers yet" << std::endl;
  }
  else
  {
    const std::string header_buffer   = "Buffer names";
    const std::string header_messages = "Messages";

    // Find max buffer name length
    unsigned int max_buffer_name_length = 0;
    std::map<std::string, unsigned long>::const_iterator it;
    for( it = m_gbuffer_buffer_sizes.begin(); it != m_gbuffer_buffer_sizes.end(); ++it )
    {
      const std::string& buffer_name = it->first;
      if( buffer_name.length() > max_buffer_name_length )
        max_buffer_name_length = buffer_name.length();
    }
    if( max_buffer_name_length < header_buffer.length() )
      max_buffer_name_length = header_buffer.length();

    const std::string vertical_separator = this->minLength( "", max_buffer_name_length + header_messages.length() + 7, '-' );

    result << std::endl;
    result << "\t" << vertical_separator << std::endl;
    result << "\t" << this->addTableLine( header_buffer, max_buffer_name_length, header_messages, header_messages.length() ) << std::endl;
    result << "\t" << vertical_separator << std::endl;
    for( it = m_gbuffer_buffer_sizes.begin(); it != m_gbuffer_buffer_sizes.end(); ++it )
    {
      float divisor                = (m_gbuffer_max_size != 0) ? (float)m_gbuffer_max_size : 1.0;
      float percentage             = it->second * 100 / divisor;
      std::string nbr_messages_str = std::to_string(it->second);
      std::string colored_state    = graphite_proxy::utils::logging::Logger::color(nbr_messages_str, this->getColor( percentage ));

      const unsigned int cell2_min_size = header_messages.length() + (colored_state.length() - nbr_messages_str.length());
      result << "\t" << this->addTableLine( it->first, max_buffer_name_length, colored_state, cell2_min_size ) << std::endl;
    }
    result << "\t" << vertical_separator << std::endl;
  }

  return result.str();
}

std::string CurrentState::showTimer() const
{
  std::stringstream result;

  result << this->writeHeader("TIMER");
  result << "flush time: " << m_timer_flush_time << " seconds" << std::endl;

  return result.str();
}

std::string CurrentState::showStats() const
{
  std::stringstream result;

  result << this->writeHeader("STATISTICS");
  result << "is enabled: " << graphite_proxy::utils::cast::toString(m_stats_enabled, true) << std::endl;

  return result.str();
}

std::string CurrentState::showMaths() const
{
  std::stringstream result;
  unsigned int max_buffer_name_length = 0;

  result << this->writeHeader("MATHEMATICS");

  // Display categories
  result << "categories: ";
  if( m_maths_categories.empty() )
  {
    result << "no categories";
  }
  else
  {
    result << std::endl;
    std::list<graphite_proxy::maths::MathsCategory*>::const_iterator it;
    for( it = m_maths_categories.begin(); it != m_maths_categories.end(); ++it )
    {
      // Write category name
      const graphite_proxy::maths::MathsCategory* category = *it;
      std::string filter = category->getFilter().str();
      result << "\t" << filter << ":" << std::endl;

      // Keep trace of maximum filter length
      if( filter.length() > max_buffer_name_length )
        max_buffer_name_length = filter.length();

      // Write all maths computations for this category
      const std::vector<graphite_proxy::maths::MathComputation>& computations = category->getComputations();
      for( size_t i = 0, size = computations.size(); i < size; i++ )
      {
        result << "\t\t";
        const graphite_proxy::maths::MathComputation& computation = computations[i];
        if( computation.isOnCount() )
          result << computation.readType() << " every " << computation.getCount() << " messages. " << std::endl;
        else
          result << computation.readType() << " every " << computation.getIterationTime() << " seconds. " << std::endl;
      }
    }
  }

  // Display buffers
  result << std::endl;
  if( m_maths_buffers.empty() )
  {
    result << "no buffers yet" << std::endl;
  }
  else
  {
    // Find max buffer name length
    for( auto it = m_maths_buffers.begin(); it != m_maths_buffers.end(); ++it )
    {
      const std::vector<graphite_proxy::maths::MathOperation*>& operations = it->second;
      for( size_t i = 0, size = operations.size(); i < size; i++ )
      {
        const unsigned long buffer_name_length = operations[i]->buffer.getName().size();
        if( buffer_name_length > max_buffer_name_length )
          max_buffer_name_length = buffer_name_length;
      }
    }

    const std::string header_current_state  = "Current state";
    const unsigned int current_state_length = header_current_state.length();
    const std::string vertical_separator    = this->minLength( "", max_buffer_name_length + current_state_length + 7, '-' );

    result << "Current maths buffers:" << std::endl;
    for( auto it = m_maths_buffers.begin(); it != m_maths_buffers.end(); ++it )
    {
      result << "\t" << vertical_separator << std::endl;
      result << "\t" << this->addTableLine( it->first, max_buffer_name_length, header_current_state, current_state_length ) << std::endl;
      result << "\t" << vertical_separator << std::endl;

      const std::vector<graphite_proxy::maths::MathOperation*>& operations = it->second;
      for( size_t i = 0, size = operations.size(); i < size; i++ )
      {
        const graphite_proxy::MessageBuffer& buffer              = operations[i]->buffer;
        unsigned long                        current_buffer_size = buffer.size();
        unsigned long                        buffer_max_size     = buffer.getBufferMaxSize();
        float                                percentage          = (buffer_max_size != 0) ? (current_buffer_size * 100.0 / buffer_max_size) : -1;

        // Compute the state
        std::stringstream state;
        state << current_buffer_size << '/' << buffer_max_size << " (" << std::setprecision(3) << percentage << "%)";
        std::string state_str = state.str();

        // Set color depending on percentage
        std::string colored_state = graphite_proxy::utils::logging::Logger::color(state_str, this->getColor( percentage ));

        // Write final result
        const unsigned int cell2_min_size = current_state_length + (colored_state.length() - state_str.length());
        result << "\t" << this->addTableLine( buffer.getName(), max_buffer_name_length, colored_state, cell2_min_size ) << std::endl;
      }

      result << "\t" << vertical_separator << std::endl << std::endl;
    }
  }

  return result.str();
}

std::string CurrentState::addTableLine( const std::string& cell1, unsigned int min_cell1_length, const std::string& cell2, unsigned int min_cell2_length ) const
{
  std::string begin("| ");
  std::string separator(" | ");
  std::string end(" |");
  std::string result;
  result.reserve( begin.length() + separator.length() + end.length() + min_cell1_length + min_cell2_length );

  result += begin;
  result += this->minLength( cell1, min_cell1_length );
  result += separator;
  result += this->minLength( cell2, min_cell2_length );
  result += end;

  return result;
}

std::string CurrentState::minLength( const std::string& text, const unsigned int length, const char character ) const
{
  std::string result = text;
  result.reserve(length);

  for( unsigned int i = result.size(); i < length; i++ )
    result += character;

  return result;
}

graphite_proxy::utils::logging::Color CurrentState::getColor( float percentage ) const
{
  if( percentage > 25 && percentage < 50 )
    return graphite_proxy::utils::logging::Color::Green;
  else if( percentage >= 50 && percentage < 75 )
    return graphite_proxy::utils::logging::Color::Yellow;
  else if( percentage >= 75 )
    return graphite_proxy::utils::logging::Color::Red;
  else
    return graphite_proxy::utils::logging::Color::Blue;
}

} // namespace utils
} // namespace server
