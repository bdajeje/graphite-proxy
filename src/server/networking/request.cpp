#include "request.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include <sstream>

namespace graphite_proxy {
namespace networking {

Request::Request( boost::asio::io_service &io_service, router_ptr router )
  : m_socket( io_service )
  , m_router( router )
{
  // Nothing
}

Request::~Request()
{
  // Shutdown then close the socket
  if( m_socket.is_open() )
  {
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
  }
}

void Request::asyncRead()
{
  STATS_INCREMENT( stats::STATS_REQUESTS_INCOMING );

  boost::asio::async_read_until( m_socket, m_data_buffer, "\0",
          boost::bind( &Request::handleRead, shared_from_this(), boost::asio::placeholders::error ) );
}

void Request::handleRead( const boost::system::error_code &error )
{
  if (error)
  {
    LOG_ERROR( "Handler read error: " + error.message(), utils::logging::LOG_HEADER_REQUEST );
    return;
  }

  // Finish networking stuff, store informations into buffer
  this->store();
}

void Request::store()
{
  STATS_INCREMENT( stats::STATS_REQUESTS_ACCEPTED );

  std::ostringstream message_content;
  message_content << &m_data_buffer;
  m_router->routeMessage( message_content.str() );
}

} // namespace networking
} // namespace graphite proxy
