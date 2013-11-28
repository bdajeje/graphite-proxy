#ifndef GRAPHITE_PROXY_MATHS_PIPELINE_HPP
#define GRAPHITE_PROXY_MATHS_PIPELINE_HPP

#include <graphite_proxy/utils/iterations.hpp>

#include <graphite_proxy/models/message.hpp>

#include <graphite_proxy/models/maths/properties.hpp>
#include <graphite_proxy/models/maths/math_computation.hpp>
#include <graphite_proxy/models/maths/math_category.hpp>

#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/buffers/message_buffer.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <string>
#include <vector>
#include <istream>

namespace graphite_proxy {
namespace maths {

struct MathOperation
{
    MathOperation( const MathComputation& _computation, const std::string &name, unsigned long max_size )
      : computation(_computation)
      , buffer(name, max_size, false)
    {}

    MathComputation computation;

    MessageBuffer   buffer;
};

/*! A maths pipeline is the container for messages which need to be computed by some maths operation like sum, max, min, etc
 *  The pipeline is like a Global Buffer but it changes the messages according to rules set in maths.xml file
 *  When messages has been computed they are given to the global buffer to be send to Graphite
 */
class MathsPipeline : public Iterations
{
  public:

    /*! Constructor
     *  \param conf_filepath   is the path to the maths configuration file which hold all mathematical operations to do on incoming messages
     *  \param global_buffer   is an instance of the Global Buffer to send back the new computed messages
     *  \param sleep_time      is the amount of time that the pipeline has to sleep before calling a new computing iteration
     *  \param buffer_max_size is the maximum size of internal messages buffer
     */
    MathsPipeline( const std::string &conf_filepath, global_buffer_ptr global_buffer, unsigned long sleep_time, unsigned long buffer_max_size );

    /*! Destructor */
    virtual ~MathsPipeline();

    /*! Add a message into the MathsPipeline. The message will be linked to a MathComputation or dropped if not wanted
     *  \param message          is the message to add
     *  \param computation_type if this param is empty the message will be added for each computations. If not the message will be added for only the matched computation
     *  \return true if everything went well (on this version of the project, always return true)
     */
    bool add( message_ptr message, const std::string& computation_type = "" );

    /*! Load a message into a buffer
     *  \param message          to load
     *  \param computation_type the type of computation for this message
     *  \return true if the message has been accepted and stored. False otherwise
     */
    bool loadMessage( message_ptr message, const std::string& computation_type );

    /*! Reload configurations from the maths configurations file
     *  \param conf_filepath is the path to access the maths configuration file
     *  \return true if configurations have been reloaded correctly
     *  \note when reloading math configuration it's possible than some old math categories are now useless (the config file changed)
     *        this is not a problem because the cleaner will automaticly remove the unused math buffers.
     */
    bool reloadConfigurations( const std::string &conf_filepath );

    /*! Does the maths module expect a message of the given type
     *  \param message_type is the type of the message (could be a simple string or a regex)
     *  \return the MathsCategory which requires the message or null is no category wants it
     */
    MathsCategory* isWanted( const std::string &message_type ) const;

    /*! Get some messages and sum their values
     *  \param messages are the messages to sum
     *  \return a new message holding the computation result
     */
    message_ptr sum( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and average their values
     *  \param messages are the messages to average
     *  \return a new message holding the computation result
     */
    message_ptr average( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and make a variance with their values
     *  \param messages are the messages to use to calculate the variance
     *  \return a new message holding the computation result
     *  \note standard deviation: http://en.wikipedia.org/wiki/Variance
     */
    message_ptr variance( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and make a standard deviation with their values
     *  \param messages are the messages to use for the standard deviation
     *  \return a new message holding the computation result
     *  \note standard deviation: http://en.wikipedia.org/wiki/Standard_deviation
     */
    message_ptr deviation( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and find the max value
     *  \param messages are the messages to sum
     *  \return a new message holding the computation result
     */
    message_ptr max( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and find the min value
     *  \param messages are the messages to sum
     *  \return a new message holding the computation result
     */
    message_ptr min( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and find the median value
     *  \param messages are the messages to sum
     *  \return a new message holding the computation result
     */
    message_ptr median( const std::vector<message_ptr> &messages ) const;

    /*! Get some messages and get a tiles of their values
     *  \note the mathmatical tiles equation is: ( (nbr_values_below + 0.5 * nbr_values_equal) / nbr_values ) * multiplicator
     *  \param messages are the messages to sum
     *  \param value is the mathematical tiles value
     *  \param strictly_below means if we want values strictly below the given value or also equal to it
     *  \param multiplicator is the multiplication value for the tiles operation
     *  \return a new message holding the computation result
     */
    message_ptr tiles( const std::vector<message_ptr> &messages, double value, bool strictly_below, double multiplicator ) const;

    /*! Retrieve all pending messages (remove them from the Pipeline)
     *  \param target_container is the messages container
     */
    void get( std::vector<message_ptr> &target_container );

    /*! Getter for the number of buffers
     *  \return the number of buffers
     */
    size_t getNbrBuffers() const { return m_buffers.size(); }

    /*! Getter for the buffer max size
     *  \return the buffer max size
     */
    unsigned long getBuffersMaxSize() const { return m_buffer_max_size; }

    /*! Getter for the categories
     *  \return the categories
     */
    const std::list<MathsCategory*>& getCategories() const { return m_categories; }

    /*! Getter for the buffers
     *  \return the buffers
     */
    const std::map<std::string, std::vector<MathOperation*>>& getBuffers() const { return m_buffers; }

    /*! Get the number of messages from the most filled buffer
     *  \return the number of messages from the most filled buffer
     */
    unsigned long getBuffersMaxMessages() const;

    /*! Remove a buffer
     *  \param buffer_name is the name of the buffer to remove
     */
    void remove(const std::string& buffer_name);

  protected:

    /*! Load configurations from a math config file
     *  \param conf_filepath is the path to the maths configuration file which hold all mathematical operations to do on incoming messages
     *  \return true if the configurations have been correctly loaded and at least one MathCategory has been created
     */
    bool loadConfigurations( const std::string &conf_filepath );

    /*! Reset categories */
    void clearCategories();

    /*! Function called at each new iteration */
    void iteration();

    /*! Compute the given messages with a given computation
     *  \param messages are the messages to compute
     *  \param computation is the computation to apply on the messages
     *  \note new created message from the computation will be given to the Global Buffer
     */
    void compute( const std::vector<message_ptr> &messages, const MathComputation& computation );

    /*! Internal compute function. Algorithm for onCount computations.
     *  \param computation    is the computation to apply
     *  \param message_buffer is the buffer to use
     */
    void computeOnCount( MathComputation& computation, MessageBuffer& message_buffer );

    /*! Internal compute function. Algorithm for onTime computations.
     *  \param computation    is the computation to apply
     *  \param message_buffer is the buffer to use
     */
    void computeOnTime( MathComputation& computation, MessageBuffer& message_buffer, unsigned long now );

  private:

    /*! Global Buffer instance */
    global_buffer_ptr                      m_buffer;

    /*! Internal representation of the maths.xml configuration file */
    std::list<MathsCategory*>              m_categories;

    /*! A metric name associated with several maths computations */
    std::map<std::string, std::vector<MathOperation*>> m_buffers;

    /*! The maximum size of internal messages buffer */
    unsigned long                          m_buffer_max_size;

    /*! A mutex for thread safety */
    mutable boost::mutex                   m_mutex;
};

typedef boost::shared_ptr<MathsPipeline> maths_ptr;

} // namespace maths
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_MATHS_PIPELINE_HPP
