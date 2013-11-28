#include "client.hpp"

#include <graphite_proxy/models/statistics/statistics_metrics.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <sstream>

namespace graphite_proxy {
namespace networking {

Client::Client( const std::string &host, const std::string &port )
 : m_socket( m_io_service )
 , m_ready( false )
{
  LOG_INFO( "Creating client to: " + host + ":" + port, utils::logging::LOG_HEADER_CLIENT );

  try
  {
    boost::asio::ip::tcp::resolver resolver( m_io_service );
    boost::asio::ip::tcp::resolver::query query( boost::asio::ip::tcp::v4(), host, port );
    m_tcp_iterator = resolver.resolve( query );
    m_ready = true;
  }
  catch (...)
  {
    LOG_ERROR( "Client could not start", utils::logging::LOG_HEADER_CLIENT );
  }
}

bool Client::send( const std::vector<message_ptr> &messages )
{
  if( messages.empty() )
  {
    LOG_DEBUG( "Nothing to send", utils::logging::LOG_HEADER_CLIENT );
    return false;
  }
  else if( !this->isReady() )
  {
    // Not ready to communicate with Graphite, abort
    LOG_WARNING( "Not ready to send", utils::logging::LOG_HEADER_CLIENT );
    return false;
  }

  boost::mutex::scoped_lock lock( m_mutex );

  // Try to connect to the Graphite Server
  try
  {
    boost::asio::connect( m_socket, m_tcp_iterator );
  }
  catch( const boost::system::system_error &e )
  {
    // One more failed connection attempt
    STATS_INCREMENT( stats::STATS_CLIENT_CONNECTIONFAILED );
    LOG_ERROR( (e.code() == boost::asio::error::connection_refused) ? "Connection refused" : std::string("Connection problem: ") + e.what(), utils::logging::LOG_HEADER_CLIENT );
    return false;
  }

  // Build the message to send
  size_t messages_count = messages.size();
  std::stringstream computed_message;
  for(size_t messages_offset = 0; messages_offset < messages_count; messages_offset++)
    computed_message << messages[messages_offset]->serialize() << '\n';

  // Send the data
  const std::string graphite_message = computed_message.str();
  boost::asio::write( m_socket, boost::asio::buffer( graphite_message, graphite_message.size() ) );

  LOG_INFO( "Sending " + std::to_string(messages_count) + " messages", utils::logging::LOG_HEADER_CLIENT );
  LOG_DEBUG( "Send content:\n" + graphite_message, utils::logging::LOG_HEADER_CLIENT );
  STATS_INCREMENT( stats::STATS_REQUESTS_SEND );
  STATS_RAISE( stats::STATS_REQUESTS_SEND_CONTENT, messages_count );
  return true;
}

} // namespace networking
} // namespace graphite proxy

