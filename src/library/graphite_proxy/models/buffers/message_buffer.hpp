#ifndef GRAPHITE_PROXY_BUFFER_HPP
#define GRAPHITE_PROXY_BUFFER_HPP

#include <graphite_proxy/models/message.hpp>

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/thread/mutex.hpp>

#include <vector>
#include <string>
#include <list>

namespace graphite_proxy {

/*! Buffer to store messages */
class MessageBuffer
{
  public:

    /*! Constructor
     *  \param name is a name associated to the buffer instance
     *  \param max_size is the maximum number of items contained into the buffer at the same time
     *  \param drop_oldest allow to specify the behavior when the buffer is full. (1) true means we drop old messages to free space for new messages. (2) false drop new message don't touch the old ones
     */
    MessageBuffer( const std::string &name, unsigned long max_size, bool drop_oldest );

    /*! Add a message into the buffer
     *  \param message is the message to add into the buffer
     *  \return true if the message has been added
     *  \note a false return means the buffer is currently full and has m_override member set to false
     */
    bool add( const message_ptr message );

    /*! Get a specific amount of messages
     *  \param target_buffer is a target vector to store found messages
     *  \param nbr is the number of messages to retrieve, starting from the beginning to the end of the buffer.
     *  \note a nbr equal to 0 means get all messages into the buffer.
     *  \note this function remove the found messages from the buffer
     */
    void get( std::vector<message_ptr> &target_buffer, unsigned long nbr = 0 );

    /*! Get all messages older or equal than a specific timestamp
     *  \param target_buffer is a target container to store found messages
     *  \param max_timestamp is the limit timestamp to accept messages
     *  \note this function remove the found messages from the buffer
     */
    void getOlderThan( std::vector<message_ptr> &target_buffer, unsigned long max_timestamp );

    /*! Get the current size used into the buffer
     *  \return the current size used into the buffer
     */
    unsigned long size() const { return m_message_list.size(); }

    /*! Does the buffer currently empty ?
     *  \return true if the buffer is empty
     */
    bool empty() const { return m_message_list.empty(); }

    /*! Get the name of the buffer
     *  \return the name of the buffer
     */
    const std::string& getName() const { return m_name; }

    /*! Get the maximum number of messages that has been contained at the same time
     *  \return the name of the buffer
     */
    unsigned long getMaxMessages() const { return m_max_messages_at_same_time; }

    /*! Getter for the maximum number of messages into the buffer
     *  \return the the maximum number of messages into the buffer
     */
    unsigned long getBufferMaxSize() const { return m_max_size; }

  private:

    /*! Name associtated to the buffer instance */
    const std::string                    m_name;

    /*! Maximum messages that can be contained at the same time */
    unsigned long                        m_max_size;

    /*! Do we drop oldest messages when buffer is full ? */
    const bool                           m_override;

    /*! maximum number of messages that has been contained at the same time */
    unsigned long                        m_max_messages_at_same_time;

    /*! Forward list containing the messages */
    std::list<message_ptr>               m_message_list;

    /*! Mutex for thread safety */
    boost::mutex                         m_mutex;
};

typedef boost::shared_ptr<MessageBuffer> message_buffer_ptr;

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_BUFFER_HPP
