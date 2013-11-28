#ifndef GRAPHITE_PROXY_UDP_SERVER_HPP
#define GRAPHITE_PROXY_UDP_SERVER_HPP

#include <graphite_proxy/models/router.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace server {
namespace networking {

/*! UDPServer log header */
static const std::string LOG_HEADER_UDPSERVER = "UDP_SERVER";

/*! UDPServer class defines a server that is listening to incoming Graphite packets on a specific UDP address and port.
 *  When a packet is received, the server just put it in the router queue.
 *  In order to run the UDPServer, the start method must be called before calling run() on the underlying io_service.
 */
class UDPServer : public boost::enable_shared_from_this<UDPServer>
{
  public:

    /*! Constructor
     *  \param io_service The underlying boost io_service
     *  \param router     A pointer to the router object where the incoming packets will be pushed
     *  \param port       The UDP port on which the server is listening on (the server listens on all interfaces)
     */
    UDPServer(boost::asio::io_service& io_service, const graphite_proxy::router_ptr& router, unsigned short port);

    /*! Destructor */
    virtual ~UDPServer();

    /*! Starts server listening process (by posting a buffer to receive data)
     * \note  To actually start the server listening loop the run() method must be called on the underlying io_service.
     */
    void start();

    /*! Getter for port to connec to
     *  \return port to connec to
     */
    unsigned short getPort() const { return m_port; }

  protected:

    /*! Posts a buffer for incoming messages */
    void start_receive();

    /*! Packet reception handler
     *  \param  error          The error code associated with the receive operation
     *  \param  bytes_received The number of bytes contained in the received packet
     */
    void handle_receive(const boost::system::error_code& error, size_t bytes_received);

  private:

    static const size_t               m_buffer_size = 1024; ///< Size of the reception buffer (in bytes)
    boost::array<char, m_buffer_size> m_buffer;             ///< Reception buffer
    boost::asio::ip::udp::socket      m_socket;             ///< UDP server socket
    graphite_proxy::router_ptr        m_router;             ///< Pointer to the message router
    unsigned short                    m_port;               ///< Port to connect to
};

typedef boost::shared_ptr<UDPServer> udp_server_ptr;

} // namespace networking
} // namespace server

#endif // GRAPHITE_PROXY_UDP_SERVER_HPP

