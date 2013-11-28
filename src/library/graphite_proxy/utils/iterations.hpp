#ifndef GRAPHITE_PROXY_ITERATIONS_HPP
#define GRAPHITE_PROXY_ITERATIONS_HPP

#include <graphite_proxy/utils/logging/logger.hpp>

#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>

namespace graphite_proxy {

/*! This is an abstract class to handle time iterations */
class Iterations : public boost::enable_shared_from_this<Iterations>
{
  public:

    /*! Constructor
     *  \param sleep_time is the amount of time (in seconds) to sleep before awakening
     *  \param name is a name to give to this iteration class
     */
    Iterations( ulong sleep_time, const std::string &name );

    /*! Destructor */
    virtual ~Iterations();

    /*! Start the timer
     *  \return true if the timer has been started correctly
     */
    bool start();

    /*! Stop the timer */
    void stop();

    /*! Is the timer currently started
     *  \return true if the timer is currently started
     */
    bool isStarted() const { return m_started; }

    /*! Is this Iterations instance is valid and could be started safety
     *  \return true if this instance is ready to be used
     */
    bool isValid() const { return m_valid; }

    /*! Getter for the amount of time to sleep
     *  \return amount of time to sleep between two iterations
     */
    long getSleepTime() const { return m_sleep_time; }

    /*! Force an iteration
     *  \note This call doesn't care about the internal timer.
     */
    void iterate() { this->iteration(); }

  protected:

    /*! Start a new iteration */
    void launch();

    /*! Function that will be called at each iteration
     *  \note Has to be ovveride in children
     */
    virtual void iteration() = 0;

  protected:

    /*! Sleep time, the timer will wake up every m_sleep_time time */
    long                    m_sleep_time;

    /*! Name to identify the timer and also used for log headers */
    std::string             m_name;

    /*! Is the timer currently started */
    bool                    m_started;

    /*! Is this Iterations instance is valid and could be started safety */
    bool                    m_valid;

    /*! Thread handling the iteration */
    boost::thread           m_thread;
};

} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_ITERATIONS_HPP
