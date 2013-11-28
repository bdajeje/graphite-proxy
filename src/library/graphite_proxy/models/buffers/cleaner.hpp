#ifndef CLEANER_HPP
#define CLEANER_HPP

#include <graphite_proxy/utils/iterations.hpp>
#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>

#include <map>
#include <string>

namespace graphite_proxy {

/*! This is an iterations class that clean the global buffer.
 *  It's watching for empty buffers that will remain empty for too long.
 *  Those buffer are deleted to clean the memory and the number of buffers */
class Cleaner : public Iterations
{
  public:

    /*! Constructor
     * \param global_buffer  is a helper pointer on the global buffer
     * \param math           is a helper pointer on the math module
     * \param sleep_time     amount of time to sleep between to iterations
     * \param max_empty_time max number of continuous time that a buffer is allow to be empty before destroy it
     * \note There is a security on the max_empty_time minimum value. If the given value is inferior to 2 the max_empty_time will be set to 2
     */
    Cleaner( global_buffer_ptr global_buffer, maths::maths_ptr math, unsigned long sleep_time, unsigned int max_empty_time );

    /*! Getter for max empty time
     *  \return the max empty time
     */
    unsigned int getMaxEmptyTime() const { return m_max_empty_time; }

  protected:

    void iteration();

    /*! Remove a buffer if it matches the remove conditions
     *  \param buffer_name   is the name of the buffer to check
     *  \param buffer_size   is the current size of the buffer
     *  \param global_buffer set to true means we are checking a buffer from the global buffer. Otherwise it's a math buffer
     */
    void clean( const std::string& buffer_name, unsigned long buffer_size, bool global_buffer );

  private:

    /*! Helper pointer on the global buffer */
    global_buffer_ptr                   m_global_buffer;

    /*! Helper pointer on the math module */
    maths::maths_ptr                    m_math;

    /*! Max number of continuous time that a buffer is allow to be empty before destroy it */
    unsigned int                        m_max_empty_time;

    /*! Association between buffer name and number of continuous time this buffer was empty */
    std::map<std::string, unsigned int> m_buffer_states;
};

} // namespace graphite_proxy

#endif // CLEANER_HPP
