#include "global_buffer.hpp"

#include <graphite_proxy/utils/logging/log_headers.hpp>
#include <graphite_proxy/utils/cast.hpp>

#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>

namespace graphite_proxy {

GlobalBuffer::GlobalBuffer( unsigned long buffer_max_size, bool drop_oldest, networking::client_ptr client )
  : m_buffer_max_size( buffer_max_size )
  , m_drop_oldest( drop_oldest )
  , m_client( client )
{
  LOG_DEBUG( "Max buffer size: " + std::to_string(m_buffer_max_size), utils::logging::LOG_HEADER_GLOBALBUFFER );
  LOG_DEBUG( "Dropping oldest: " + utils::cast::toString(m_drop_oldest, true), utils::logging::LOG_HEADER_GLOBALBUFFER );
}

bool GlobalBuffer::add( message_ptr message )
{
  if(!message || !message->isValid())
  {
    LOG_WARNING( "Invalid message: " + message->serialize(), utils::logging::LOG_HEADER_GLOBALBUFFER );
    return false;
  }

  message_buffer_ptr message_buffer;
  std::string message_type = message->getType();

  boost::mutex::scoped_lock lock( m_buffers_mutex );

  // Is the buffer already created ? If not create it
  if (m_buffers.count( message_type ) <= 0)
    message_buffer = m_buffers[message_type] = boost::make_shared<MessageBuffer>( message_type, m_buffer_max_size, m_drop_oldest );
  else message_buffer = m_buffers[message_type];

  // Try to add the message content to the buffer
  if( message_buffer->add( message ) )
  {
    LOG_DEBUG( "Message added: " + message->serialize(), utils::logging::LOG_HEADER_GLOBALBUFFER );

    // Check if the buffer is now full
    if( message_buffer->size() >= m_buffer_max_size )
    {
      // The buffer is full, send messages
      LOG_DEBUG( "Buffer '" +  message_buffer->getName() + "' is full, sending messages", utils::logging::LOG_HEADER_GLOBALBUFFER );

      // Retrieve messages to send
      std::vector<message_ptr> messages_to_send;
      message_buffer->get( messages_to_send );

      if( !m_client->send( messages_to_send ) )
      {
        // Messages can't be send, back them back into the buffer
        for( message_ptr message : messages_to_send )
          message_buffer->add(message);
      }
    }

    return true;
  }
  else
  {
    LOG_DEBUG( "Message dropped: " + message->serialize(), utils::logging::LOG_HEADER_GLOBALBUFFER );
    return false;
  }
}

void GlobalBuffer::get( std::vector<message_ptr> &result_messages )
{
  boost::mutex::scoped_lock lock( m_buffers_mutex );

  for (std::map<std::string, message_buffer_ptr>::iterator it = m_buffers.begin(); it != m_buffers.end(); ++it)
    it->second->get( result_messages );
}

void GlobalBuffer::get( const std::string &type, std::vector<message_ptr> &result_messages )
{
  // Find the buffer corresponding to the given type
  message_buffer_ptr message_buffer;
  {
    boost::mutex::scoped_lock lock( m_buffers_mutex );

    if (m_buffers.count( type ) > 0)
    {
      message_buffer = m_buffers[type];
    }
    else return;
  }

  message_buffer->get( result_messages );
}

unsigned long GlobalBuffer::getBuffersMaxMessages() const
{
  unsigned long size, max = 0;

  {
    boost::mutex::scoped_lock lock( m_buffers_mutex );

    for( std::map<std::string, message_buffer_ptr>::const_iterator it = m_buffers.begin(); it != m_buffers.end(); ++it )
    {
      size = it->second->getMaxMessages();
      if( size > max )
        max = size;
    }
  }

  return max;
}

std::map<std::string, unsigned long> GlobalBuffer::getBufferSizes() const
{
  std::map<std::string, unsigned long> result;

  boost::mutex::scoped_lock lock( m_buffers_mutex );

  for( std::map<std::string, message_buffer_ptr>::const_iterator it = m_buffers.begin(); it != m_buffers.end(); ++it )
  {
    result[it->first] = it->second->size();
  }

  return result;
}

void GlobalBuffer::remove( const std::string& buffer_name )
{
  boost::mutex::scoped_lock lock( m_buffers_mutex );
  m_buffers.erase(buffer_name);
}

} // namespace graphite_proxy
