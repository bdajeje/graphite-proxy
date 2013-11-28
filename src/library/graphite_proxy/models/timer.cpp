#include "timer.hpp"

#include <graphite_proxy/models/message.hpp>

#include <vector>

namespace graphite_proxy {

Timer::Timer( global_buffer_ptr buffer, networking::client_ptr client, long buffer_flush_time )
  : Iterations( buffer_flush_time, utils::logging::LOG_HEADER_TIMER )
  , m_buffer( buffer )
  , m_client( client )
{
  // Nothing
}

void Timer::iteration()
{
  // Get messages from the Global Buffer
  std::vector<message_ptr> messages_to_send;
  m_buffer->get( messages_to_send );

  LOG_DEBUG( std::to_string( messages_to_send.size() ) + " messages to send", utils::logging::LOG_HEADER_TIMER );

  // Try to send the messages to the Graphite Server
  if( !m_client->send( messages_to_send ) )
  {
    // Messages can't be send, back them back into the Global Buffer
    for( message_ptr message : messages_to_send )
      m_buffer->add(message);
  }
}

} // namespace graphite_proxy
