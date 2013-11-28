#include "message_buffer.hpp"

#include <graphite_proxy/utils/logging/log_headers.hpp>

namespace graphite_proxy {

MessageBuffer::MessageBuffer( const std::string &name, unsigned long max_size, bool drop_oldest )
  : m_name( name )
  , m_max_size( max_size )
  , m_override( drop_oldest )
  , m_max_messages_at_same_time(0)
{
  if ( m_max_size > m_message_list.max_size() )
  {
    LOG_WARNING( "The buffer max size is greater than the possible operating system list size possibilities, shrink it", utils::logging::LOG_HEADER_BUFFER );
    m_max_size = m_message_list.max_size();
  }
}

bool MessageBuffer::add( const message_ptr message )
{
  boost::mutex::scoped_lock lock( m_mutex );

  if ( m_message_list.size() >= m_max_size )
  {
    if ( m_override )
    {
      LOG_WARNING( "Buffer '" + m_name + "'" + " has reach its max size. Older messages will be override by new ones", utils::logging::LOG_HEADER_BUFFER );
      m_message_list.pop_front(); // Remove the first message of the list to make free space
    }
    else // Ignore messages if the message list is full and we don't want to drop/override oldest messages
    {
      LOG_WARNING( "Buffer '" + m_name + "'" + " is full, droping incoming message: " + message->serialize(), utils::logging::LOG_HEADER_BUFFER );
      return false;
    }
  }

  // Store the message into the message list
  m_message_list.push_back( message );

  // Update maximum number of messages that has been contained at the same time
  const unsigned long current_nbr_messages = m_message_list.size();
  if( current_nbr_messages > m_max_messages_at_same_time )
    m_max_messages_at_same_time = current_nbr_messages;

  return true;
}

void MessageBuffer::get( std::vector<message_ptr> &target_buffer, unsigned long nbr )
{
  boost::mutex::scoped_lock lock( m_mutex );

  if ( m_message_list.empty() )
    return;

  // Get maximum number of existing item or all if nbr == 0
  unsigned long list_size = m_message_list.size();
  if ( nbr == 0 || nbr > list_size )
    nbr = list_size;

  // Reserve the currently used space to prevent further memory reallocations
  target_buffer.reserve( nbr );

  // Fill the target buffer
  for ( unsigned long i = 0; i < nbr; i++ )
  {
    target_buffer.push_back( m_message_list.front() );
    m_message_list.pop_front();
  }
}

void MessageBuffer::getOlderThan( std::vector<message_ptr> &target_buffer, unsigned long max_timestamp )
{
  boost::mutex::scoped_lock lock( m_mutex );

  if ( m_message_list.empty() )
    return;

  //m_message_list.retrieveByTimestamp( target_buffer, max_timestamp );
  unsigned long size = m_message_list.size();
  for ( unsigned long i = 0; i < size; i++ )
  {
    message_ptr message = m_message_list.front();
    m_message_list.pop_front();
    if ( message->getTimestamp() <= max_timestamp )
      target_buffer.push_back( message );
    else m_message_list.push_back( message );
  }
}

} // namespace graphite_proxy


