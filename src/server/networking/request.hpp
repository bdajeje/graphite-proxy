#ifndef GRAPHITE_PROXY_REQUEST_HPP
#define GRAPHITE_PROXY_REQUEST_HPP

#include <graphite_proxy/models/router.hpp>

#include <graphite_proxy/models/statistics/statistics_metrics.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>

namespace graphite_proxy {
namespace networking {

/*! Incoming network request
 *  This class has two goals: (1) read the request content (2) parse the request content to create only valid messages
 */
class Request : public boost::enable_shared_from_this<Request>
{
  public:

    /*! Constructor
     *  \param io_service is an Input / Output service
     *  \param router is a Router instance to route the created message
     */
    Request( boost::asio::io_service &io_service, router_ptr router );

    ~Request();

    /*! Socket getter
     *  \return the socket used by the Request
     */
    boost::asio::ip::tcp::socket& socket() { return m_socket; }

    /*! Asyncronously read the incoming data  */
    void asyncRead();

  protected:

    /*! Read handler
     *  \param error is the possible errors which can occured
     */
    void handleRead( const boost::system::error_code &error );

    /*! Send the message to the router to store it */
    void store();

  private:

    /*! Socket used to send/receive data */
    boost::asio::ip::tcp::socket  m_socket;

    /*! Buffer to store incoming data */
    boost::asio::streambuf        m_data_buffer;

    /*! Router to route the created message */
    router_ptr                    m_router;
};

typedef boost::shared_ptr<graphite_proxy::networking::Request> request_ptr;

} // namespace networking
} // namespace graphite proxy

#endif // GRAPHITE_PROXY_REQUEST_HPP
