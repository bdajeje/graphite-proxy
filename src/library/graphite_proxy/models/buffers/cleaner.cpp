#include "cleaner.hpp"

#include <graphite_proxy/utils/logging/log_headers.hpp>

namespace graphite_proxy {

Cleaner::Cleaner( global_buffer_ptr global_buffer, maths::maths_ptr math, unsigned long sleep_time, unsigned int max_empty_time )
  : Iterations(sleep_time, "Cleaner")
  , m_global_buffer(global_buffer)
  , m_math(math)
{
  // Security on mimimum value for max_empty_time
  const unsigned int min_value = 2;
  m_max_empty_time = (max_empty_time > min_value) ? max_empty_time : min_value;
}

void Cleaner::iteration()
{
  LOG_DEBUG( "Cleaner iteration", utils::logging::LOG_HEADER_CLEANER );

  // Iterate over global buffer's buffers
  std::map<std::string, unsigned long> buffer_sizes = m_global_buffer->getBufferSizes();
  for( auto it = buffer_sizes.begin(); it != buffer_sizes.end(); ++it )
    this->clean( it->first, it->second, true );

  // Iterate over math buffers
//  if( m_math )
//  {
//    const std::map<std::string, std::vector<maths::MathOperation*>>& buffers = m_math->getBuffers();
//    for( auto it = buffers.begin(); it != buffers.end(); ++it )
//    {
//      const std::vector<maths::MathOperation*>& operations = it->second;
//      for( size_t i = 0, size = operations.size(); i < size; i++ )
//      {
//        const MessageBuffer& buffer = operations[i]->buffer;
//        this->clean( buffer.getName(), buffer.size(), false );
//      }
//    }
//  }
}

void Cleaner::clean( const std::string& buffer_name, unsigned long buffer_size, bool global_buffer )
{
  const std::string name = ((global_buffer) ? "gb_" : "math_") + buffer_name;
  auto buffer_state      = m_buffer_states.find(name);

  // Buffer empty, let's see if we can delete it
  if( buffer_size == 0 )
  {
    if( buffer_state != m_buffer_states.end() )
    {
      unsigned int &buffer_number = m_buffer_states[name];

      // Check maximum number of allowed time before deleting the buffer
      if( buffer_number + 1 == m_max_empty_time )
      {
        // Delete the internal state
        m_buffer_states.erase(buffer_state);

        // Delete the buffer
        if( global_buffer )
        {
          LOG_DEBUG( std::string("Removing '") + name + "' buffer from Global Buffer", utils::logging::LOG_HEADER_CLEANER );
          m_global_buffer->remove( buffer_name );
        }
        else if( m_math )
        {
          m_math->remove( buffer_name );
          LOG_DEBUG( std::string("Removing '") + name + "' buffer from Math Pipeline", utils::logging::LOG_HEADER_CLEANER );
        }
      }
      else buffer_number++;
    }
    else
    {
      // This entry doesn't exist, let's create it
      m_buffer_states[name] = 1;
    }
  }
  else
  {
    // If the buffer exists into the buffer states, remove it because it's not empty
    if( buffer_state != m_buffer_states.end() )
      m_buffer_states.erase(buffer_state);
  }
}

} // namespace graphite_proxy
