#ifndef GRAPHITE_PROXY_SERVER_HPP
#define GRAPHITE_PROXY_SERVER_HPP

#include <graphite_proxy/models/router.hpp>
#include <graphite_proxy/utils/logging/logger.hpp>

#include <networking/request.hpp>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>

namespace server {
namespace networking {

static const std::string LOG_HEADER_SERVER = " SERVER ";

/*! Server is handling incoming connection
 *  It's creating a new Request object when receiving a new connection
 *  The Request object is responsible of reading the incoming data, not the Server
 */
class Server : public boost::enable_shared_from_this<Server>
{
 public:

  /*! Constructor
   *  \param io_service Boost I/O service object that will manage server socket I/O
   *  \param router is the a Router instance. It dispatches requests to those which want them (Global Buffer or Maths Pipeline).
   *  \param ip_address is the server ip address
   *  \param port is the server port
   */
  Server(boost::asio::io_service* io_service, const graphite_proxy::router_ptr router, const std::string &ip_address, const std::string &port);

  /*! Destructor */
  virtual ~Server();

  /*! Starts the server */
  void start();

  /*! Get the IP address of this server
   *  \return the ip address of the server
   *  \note return empty string if no ip address has been defined
   */
  std::string getAddress() const;

  /*! Get the port used by the server
   *  \return the port where he server is running
   *  \note return 0 if no port has been defined
   */
  unsigned short getPort() const;

 protected:

  /*! Set the IP address and the port number of the server
   *  \param ip_address is the server ip address to reach it
   *  \param port is the server port to reach it
   */
  void setIPAddress(const std::string &ip_address, const std::string &port);

  /*! Server request handler
   *  \param request is the object which will handle the incoming connection
   *  \param error is possible error which can occurs during the process
   */
  void handlerAccept(boost::shared_ptr<graphite_proxy::networking::Request> request, const boost::system::error_code &error);

  /*! Start accepting new connection */
  void startAccept();

 private:

  boost::asio::io_service*                          m_io_service; ///< Boost I/O service object that will manage server socket I/O
  const graphite_proxy::router_ptr                  m_router;     ///< Router instance to route incoming requests
  boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;   ///< TCP acceptor to handle incoming TCP requests
};

typedef boost::shared_ptr<Server> tcp_server_ptr;

} // namespace networking
} // namespace server

#endif // GRAPHITE_PROXY_SERVER_HPP

