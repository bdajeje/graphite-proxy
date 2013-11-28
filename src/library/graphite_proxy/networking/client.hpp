#ifndef GRAPHITE_PROXY_CLIENT_HPP
#define GRAPHITE_PROXY_CLIENT_HPP

#include <graphite_proxy/models/message.hpp>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>
#include <string>

namespace graphite_proxy {
namespace networking {

/*! Network client to send messages to Graphite */
class Client
{
  public:

    /*! Constructor
     *  \param host is the address to reach the Graphite Server
     *  \param port is the port to reach the Graphite Server
     */
    Client( const std::string &host, const std::string &port );

    /*! Send given messages (and pending ones)
     *  \param messages are some new messages to send
     */
    bool send( const std::vector<message_ptr> &messages );

    /*! Is the client ready to send messages
     *  \return true if the client is ready to send messages
     */
    bool isReady() const { return m_ready; }

  private:

    /*! Input / Output service */
    boost::asio::io_service                  m_io_service;

    /*! TCP iterator used by the socket */
    boost::asio::ip::tcp::resolver::iterator m_tcp_iterator;

    /*! Socket used to send messages */
    boost::asio::ip::tcp::socket             m_socket;

    /*! Is the client ready to send messages */
    bool                                     m_ready;

    /*! Mutex for thread safe */
    boost::mutex                             m_mutex;
};

typedef boost::shared_ptr<Client> client_ptr;

} // namespace networking
} // namespace graphite proxy

#endif // GRAPHITE_PROXY_CLIENT_HPP
