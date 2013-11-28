#ifndef GRAPHITE_PROXY_TIMER_HPP
#define GRAPHITE_PROXY_TIMER_HPP

#include <graphite_proxy/utils/iterations.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>

#include <graphite_proxy/networking/client.hpp>

#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <boost/shared_ptr.hpp>

namespace graphite_proxy {

/*! Timer class is the link between the Global Buffer and the Client.
 *  It will give buffered messages to the client when needed
 */
class Timer : public Iterations
{
  public:

    /*! Constructor
     *  \param buffer is a GlobalBuffer instance to pick messages
     *  \param client is a Client instance to give messages
     *  \param buffer_flush_time is the amount of time to wait before calling a new send messages iteration
     */
    Timer( global_buffer_ptr buffer, networking::client_ptr client, long buffer_flush_time );

  protected:

    /*! Get messages from the Global Buffer and give them to the Client */
    void iteration();

  private:

    /*! An instance of the Global Buffer */
    global_buffer_ptr      m_buffer;

    /*! An instance of the Client */
    networking::client_ptr m_client;
};

typedef boost::shared_ptr<Timer> timer_ptr;

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_TIMER_HPP
