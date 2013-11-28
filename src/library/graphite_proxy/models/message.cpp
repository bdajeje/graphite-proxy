#include "message.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>

namespace graphite_proxy {

Message::Message()
 : m_type( std::string() )
 , m_value( 0.0 )
 , m_timestamp( 0 )
{
  /* Nothing to do */
}

Message::Message( const std::string &type, double value, ulong timestamp, ulong received_timestamp )
  : m_type( type )
  , m_value( value )
  , m_timestamp( timestamp )
  , m_received_timestamp( received_timestamp )
{
  /* Nothing to do */
}

boost::shared_ptr<Message> Message::createMessage( const std::string &input )
{
  // Split message into parts
  const std::string trimmed_input = boost::trim_copy( input );
  std::vector<std::string> message_parts;
  message_parts.reserve(3);
  boost::split( message_parts, trimmed_input, boost::is_any_of(" ") );

  // Does the syntax appears to be correct ?
  if ( message_parts.size() != 3 || message_parts[0].empty() || message_parts[1].empty() || message_parts[2].empty() )
    return boost::shared_ptr<Message>();

  const std::string message_type = message_parts[0];

  try
  {
    // Try to parse the message_value (double)
    const double message_value = boost::lexical_cast<double>( message_parts[1] );

    // Try to parse the message_time (integer)
    const ulong message_time = boost::lexical_cast<ulong>( message_parts[2] );

    // Everything good, create the message
    return boost::make_shared<Message>( message_type, message_value, message_time );
  }
  catch(...)
  { /* Nothing */ }

  return boost::shared_ptr<Message>();
}

bool Message::createMessages( const std::string &input, std::vector<boost::shared_ptr<Message> > &messages_container )
{
  std::vector<std::string> parts;
  boost::split( parts, input, boost::is_any_of("\n") );
  const size_t parts_size = parts.size();
  messages_container.reserve( parts_size );

  for ( size_t i = 0; i < parts_size; i++ )
  {
    message_ptr message = Message::createMessage( parts[i] );
    if( message )
      messages_container.push_back( message );
  }

  return ( messages_container.empty() ) ? false : true;
}

const std::string& Message::serialize()
{
  if (m_serialized.empty() && !m_type.empty())
  {
    // No need to catch boost::bad_lexical cast here, values are obviously correct
    std::string string_value = std::to_string( m_value );
    std::string string_time  = std::to_string( m_timestamp );

    m_serialized.reserve( m_type.size() + string_value.size() + string_time.size() + 2 );
    m_serialized = m_type + " " + string_value + " " + string_time;
  }

  return m_serialized;
}

ulong Message::length()
{
  if (m_serialized.empty())
    this->serialize();

  return m_serialized.size();
}

bool Message::isValid() const
{
  return !m_type.empty();
}

} // namespace graphite_proxy
