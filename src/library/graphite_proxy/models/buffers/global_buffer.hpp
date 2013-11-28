#ifndef GRAPHITE_PROXY_GLOBAL_BUFFER_HPP
#define GRAPHITE_PROXY_GLOBAL_BUFFER_HPP

#include <graphite_proxy/models/message.hpp>
#include <graphite_proxy/models/buffers/message_buffer.hpp>

#include <graphite_proxy/networking/client.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <map>

namespace graphite_proxy {

/*! The Global Buffer is a wrapper around messages buffers
 *  It stores incoming messages into seperated messages buffers according to the message type
 */
class GlobalBuffer
{
  public:

    /*! Constructor
     *  \param buffer_max_size is the maximum number of messages that can be stored into one messages buffer
     *  \param drop_oldest     means that incoming messages will override old ones if there is no more free space into the messages buffer
     *  \param client          to send messages
     */
    GlobalBuffer( unsigned long buffer_max_size, bool drop_oldest, networking::client_ptr client );

    /*! Add a message into the Global Buffer
     *  \param message is the message to store
     *  \return true if the message has been added to a messages buffer.
     */
    bool add( message_ptr message );

    /*! Get all messages from the Global Buffer
     *  \param result_messages is a container to stored returned messages
     *  \note this function remove the returned messages from the Global Buffer (and also the messages buffer)
     */
    void get( std::vector<message_ptr> &result_messages );

    /*! Get all messages of a specific type
     *  \param type if the specific type of messages to search for
     *  \param result_messages is a container for the returned results
     *  \note this function remove the returned messages from the buffers
     */
    void get( const std::string &type, std::vector<message_ptr> &result_messages );

    /*! Get the maximum number of messages contained at the same time among all subbuffers (usefull for statistics) */
    unsigned long getBuffersMaxMessages() const;

    /*! Do we drop oldest messages ?
     *  \return true if oldest messages are dropped
     */
    bool isDropOldest() const { return m_drop_oldest; }

    /*! Retrieve size of all sub buffers
     *  \return a map where a buffer name is associated to it's current size
     */
    std::map<std::string, unsigned long> getBufferSizes() const;

    /*! Getter for the buffers max size
     *  \return the buffers max size
     */
    unsigned long getBuffersMaxSize() const { return m_buffer_max_size; }

    /*! Remove a buffer
     *  \param buffer_name is the name of the buffer to remove
     */
    void remove( const std::string& buffer_name );

  private:

    /*! Maximum number of messages that can be stored into one messages buffer */
    const unsigned long                       m_buffer_max_size;

    /*! When a buffer reaches its maximum size, tells if we drop oldest messages to get free space */
    bool                                      m_drop_oldest;

    /*! Message buffers associated to a unique name */
    std::map<std::string, message_buffer_ptr> m_buffers;

    /*! An instance of the Client */
    networking::client_ptr                    m_client;

    /*! Mutex for thread safety */
    mutable boost::mutex                      m_buffers_mutex;
};

typedef boost::shared_ptr<GlobalBuffer> global_buffer_ptr;

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_GLOBAL_BUFFER_HPP
