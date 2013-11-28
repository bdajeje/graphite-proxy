#ifndef GRAPHITE_PROXY_STATISTICS_HPP
#define GRAPHITE_PROXY_STATISTICS_HPP

#include <graphite_proxy/utils/iterations.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <graphite_proxy/models/statistics/statistics_metrics.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/router.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <map>

namespace graphite_proxy {

/*! Class to hold programm statistics
 *  \note this class is a singleton
 */
class Statistics : public Iterations
{
  public:

    /*! Constructor
     *  \param buffer     is a GlobalBuffer instance needed to get some statistics
     *  \param router     is a Router instance to give created statistics messages
     *  \param sleep_time is the amount of time to wait before calling a new iteration
     *  \param hostname   of the system
     *  \return the statistics instance
     */
    static boost::shared_ptr<Statistics> init( global_buffer_ptr buffer, maths::maths_ptr math, router_ptr router, long sleep_time, const std::string& hostname );

    /*! Gets the unique statistics instance
     *  \return The unique statistics instance (or an empty shared_ptr if Statistics::init was not called)
     */
    static const boost::shared_ptr<Statistics> instance() { return s_instance; }

    /*! Is the statistics module enabled
     *  \return true if the module is enabled (actually if the instance is initialized)
     */
    static bool enabled() { return (bool)s_instance; }

    /*! Increment a metric by a specific value
     *  \param metric_name is the name of the metric to increment
     *  \param value is the amount to add to the metric value (1 by default)
     *  \note value could perfectly be a negative number
     *  \note If the metric doesn't exist it will be created with a value equal to the given one
     */
    void raise( const std::string &metric_name, long value = 1 );

  protected:

    /*! Hidden constructor
     *  \param buffer     is a GlobalBuffer instance
     *  \param math       is a Math Pipeline instance
     *  \param router     is a Router instance
     *  \param sleep_time is the amount of time to wait before calling a new iteration
     *  \param hostname   of the system
     */
    Statistics( global_buffer_ptr buffer, maths::maths_ptr math, router_ptr router, long sleep_time, const std::string& hostname );

    /*! New statistics iteration */
    void iteration();

  private:

    /*! Unique instance of statistics singleton class */
    static boost::shared_ptr<Statistics> s_instance;

    /*! A reference to the Global Buffer instance */
    global_buffer_ptr                    m_buffer;

    /*! A reference to the Math Pipeline instance */
    maths::maths_ptr                     m_math;

    /*! A reference to the Router instance */
    router_ptr                           m_router;

    /*! Statistics metrics representing by a name and a value */
    std::map<std::string, long>          m_metrics;

    /*! Hostname of the system */
    std::string                          m_hostname;

    /*! Internal mutex for thread safety operations */
    boost::mutex                         m_mutex;
};

typedef boost::shared_ptr<Statistics> statistics_ptr;

} // namespace graphite_proxy

#define STATS_INCREMENT( message )\
{\
  if ( Statistics::enabled() )\
    Statistics::instance()->raise( message );\
}

#define STATS_RAISE( message, value )\
{\
  if ( Statistics::enabled() )\
    Statistics::instance()->raise( message, value );\
}

#endif // GRAPHITE_PROXY_STATISTICS_HPP
