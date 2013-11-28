#include "udp_server.hpp"

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/bind.hpp>

#include <sstream>
#include <string>

namespace server {
namespace networking {

using boost::asio::ip::udp;

UDPServer::UDPServer(boost::asio::io_service& io_service, const graphite_proxy::router_ptr& router, unsigned short port)
  : m_socket(io_service, udp::endpoint(udp::v4(), port))
  , m_router(router)
  , m_port(port)
{
  LOG_INFO(std::string("Create UDP Server on port: ") + std::to_string(port), LOG_HEADER_UDPSERVER);
}

UDPServer::~UDPServer()
{
  // Nothing
}

void UDPServer::start()
{
  // Post buffer
  start_receive();
}

void UDPServer::start_receive()
{
  m_socket.async_receive(boost::asio::buffer(m_buffer), 0, boost::bind(&UDPServer::handle_receive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void UDPServer::handle_receive(const boost::system::error_code& error, size_t bytes_received)
{
  if(!error)
  {
    // Send the data to the router
    m_router->routeMessage(std::string(m_buffer.data(), bytes_received));
  }
  else
  {
    LOG_ERROR(std::string("An error occurred while receiving data: ") + error.message(), LOG_HEADER_UDPSERVER);
  }

  // Post buffer again
  start_receive();
}

} // namespace networking
} // namespace server

