#include "server.hpp"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace server {
namespace networking {

Server::Server(boost::asio::io_service* io_service, const graphite_proxy::router_ptr router, const std::string &ip_address, const std::string &port)
  : m_io_service(io_service)
  , m_router(router)
{
  this->setIPAddress(ip_address, port);
}

Server::~Server()
{
  // Nothing
}

void Server::start()
{
  startAccept();
}

void Server::setIPAddress(const std::string &ip_address, const std::string &port)
{
  try
  {
    boost::asio::ip::tcp::resolver resolver(*m_io_service);
    boost::asio::ip::tcp::resolver::query query(ip_address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

    m_acceptor.reset(new boost::asio::ip::tcp::acceptor(*m_io_service, endpoint));
    LOG_INFO( std::string("Create TCP Server on ") + ip_address + ":" + port, LOG_HEADER_SERVER );
  }
  catch ( const std::exception &e )
  {
    LOG_ERROR( "Server can't set the given IP address (" + ip_address + ":" + port + "): " + e.what(), LOG_HEADER_SERVER );
  }
}

std::string Server::getAddress() const
{
  if (!m_acceptor)
    return std::string();

  return m_acceptor->local_endpoint().address().to_string();
}

unsigned short Server::getPort() const
{
  if (!m_acceptor)
    return 0;

  boost::system::error_code ec;
  boost::asio::ip::tcp::endpoint endpoint = m_acceptor->local_endpoint( ec );
  if (ec)
  {
    LOG_ERROR( "Port error: " + ec.message(), LOG_HEADER_SERVER );
    return 0;
  }
  else return endpoint.port();
}

void Server::startAccept()
{
  boost::shared_ptr<graphite_proxy::networking::Request> request(new graphite_proxy::networking::Request(*m_io_service, m_router));
  m_acceptor->async_accept(request->socket(), boost::bind(&Server::handlerAccept, shared_from_this(), request, boost::asio::placeholders::error));
}

void Server::handlerAccept(boost::shared_ptr<graphite_proxy::networking::Request> request, const boost::system::error_code &error)
{
  if (error)
  {
    LOG_ERROR( "Server handler accept error: " + error.message(), LOG_HEADER_SERVER );
    return;
  }
  else LOG_DEBUG( "Request accepted", LOG_HEADER_SERVER );

  request->asyncRead();

  this->startAccept();
}

} // namespace networking
} // namespace server

