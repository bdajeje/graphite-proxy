#ifndef GRAPHITE_PROXY_ROUTER_HPP
#define GRAPHITE_PROXY_ROUTER_HPP

#include <graphite_proxy/models/message.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace graphite_proxy {

/*! Route messages to the Global Buffer or to the Maths Pipeline */
class Router
{
  public:

    /*! Constructor
     *  \param global_buffer  is a GlobalBuffer instance
     *  \param maths_pipeline is a MathsPipeline instance
     */
    Router( global_buffer_ptr global_buffer, boost::shared_ptr<maths::MathsPipeline> maths_pipeline );

    /*! Route a message depending on it's type
     *  \param message to route
     *  \return false if the message couldn't be routed
     */
    bool routeMessage( const message_ptr message ) const;

    /*! Route a message depending on it's type
     *  \param message to route
     *  \return false if the message couldn't be routed
     */
    bool routeMessage( const std::string& message ) const;

    /*! Save all pending messages from the Global Buffer
     *  \param  pass_through_messages_filepath is the name of the file where the pass through messages will be save
     *  \param  maths_messages_filepath        is the name of the file where the maths messages will be save
     *  \return the number of saved messages (-1 if error)
     */
    long int serialize( const std::string& pass_through_messages_filepath, const std::string& maths_messages_filepath );

    /*! Load messages from a text file
     *  \param  pass_through_messages_filepath is the name of the file where the pass through messages will be loaded from
     *  \param  maths_messages_filepath        is the name of the file where the maths messages will be loaded from
     *  \return the number of loaded messages (-1 if error)
     */
    long int load( const std::string& pass_through_messages_filepath, const std::string& maths_messages_filepath );

  protected:

    /*! Function called when a bad message is received
     *  \param message is the syntax incorrect message
     */
    void badSyntax( const message_ptr message ) const;

    /*! Load messages from a file
     *  \param filename is the file path to access the file
     *  \param math     is the messages to load are math messages
     *  \return the number of loaded messages (-1 if error)
     */
    long int loadMessages( const std::string& filename, bool math ) const;

    /*! Serialize messages into a file
     *  \param messages to serialized
     *  \param filename is the filepath where to save the messages
     *  \param suffixes are some suffixes to add at the end of a message line (default empty vector). For example: The message at position '2' will be suffixed by the suffix at position '2' and so on.
     *  \return the number of serialized messages
     */
    long int serializeMessages( const std::vector<message_ptr>& messages, const std::string& filename, const std::vector<std::string>& suffixes = std::vector<std::string>() ) const;

  private:

    /*! A GlobalBuffer instance */
    global_buffer_ptr                       m_global_buffer;

    /*! A MathsPipeline instance */
    boost::shared_ptr<maths::MathsPipeline> m_maths_pipeline;
};

typedef boost::shared_ptr<graphite_proxy::Router> router_ptr;

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_ROUTER_HPP
