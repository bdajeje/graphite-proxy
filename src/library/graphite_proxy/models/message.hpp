#ifndef GRAPHITE_PROXY_MESSAGE_HPP
#define GRAPHITE_PROXY_MESSAGE_HPP

#include <graphite_proxy/utils/time.hpp>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#define MAX_MESSAGE_LENGTH 2048

namespace graphite_proxy {

/*! Network incoming message representation of type Graphite plain text (name value timestamp) */
class Message
{
  public:

    /*! Constructor */
    Message();

    /*! Constructor
     *  \param type is an identifier to categorise the message,
     *  \param value is the value number of the message
     *  \param timestamp is a timestamp for the message
     *  \param received_timestamp is the time when the message has been received (in seconds)
     */
    Message( const std::string &type, double value,
             ulong timestamp          = utils::time::now(),
             ulong received_timestamp = utils::time::now() );

    /*! Create a message from an input string
     *  \param input is a string representing a message
     *  \param messages_container is the target container to put created messages
     *  \return true if at least one message could be created
     */
    static bool createMessages( const std::string &input, std::vector<boost::shared_ptr<Message> > &messages_container );

    /*! Create a message from an input string
     *  \param input is a string representing a message
     *  \param messages_container is the target container to put created messages
     *  \return the created message (null if an error occured)
     */
    static boost::shared_ptr<Message> createMessage( const std::string &input );

    /*! Message type getter
     *  \return the type of the message
     */
    const std::string& getType() const { return m_type; }

    /*! Message value getter
     *  \return the value of the message
     */
    double getValue() const { return m_value; }

    /*! Message timestamp getter
     *  \return the timestamp of the message
     */
    ulong getTimestamp() const { return m_timestamp; }

    /*! Message received timestamp getter
     *  \return the received timestamp of the message (in seconds)
     */
    ulong getReceivedTimestamp() const { return m_received_timestamp; }

    /*! Serialize the message by returning the string expected by Graphite
     *  \return a representation of the message
     *  \note this function initialize the m_serialized variable only once. After the first call the value has already been computed, so directly returned it
     */
    const std::string& serialize();

    /*! Get the length of the serialized representation of the message
     *  \return the length of the string representation of the message
     */
    ulong length();

    /*! Is this message valid? It means it has a non empty type
     *  \return true if the message is considered valid
     */
    bool isValid() const;

  private:

    /*! Message content type (category) */
    std::string m_type;

    /*! Message content value */
    double      m_value;

    /*! Message content time */
    ulong       m_timestamp;

    /*! Message receiving time (in seconds) */
    ulong       m_received_timestamp;

    /*! This variable old the entire content of the message. It's empty at the message creation. It's computed when needed */
    std::string m_serialized;
};

typedef boost::shared_ptr<Message> message_ptr;

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_MESSAGE_HPP
