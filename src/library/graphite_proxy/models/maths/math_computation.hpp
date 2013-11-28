#ifndef GRAPHITE_PROXY_MATH_COMPUTATION_HPP
#define GRAPHITE_PROXY_MATH_COMPUTATION_HPP

#include <graphite_proxy/utils/logging/logger.hpp>
#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <sys/types.h>
#include <map>

namespace graphite_proxy {
namespace maths {

/*! Possible math operations */
enum ComputationType { SUM = 0, AVERAGE = 1, MIN = 2, MAX = 3, MEDIAN = 4, TILES = 5, VARIANCE = 6, DEVIATION = 7, UNKNOWN = 8 };

/*! A MathComputation represents a node from the maths.xml file
 *  It's a mathematical operation to do when a specific amount of messages or time has been reach
 */
class MathComputation
{
  public:

    /*! Constructor
     *  \param type is the computation type
     *  \param iteration_time is a boolean to know if this computation wait for a specific amout of time or messages
     *  \param value is the specific amout of messages or time to each before computing
     *  \param init_compute_time set a started time for the computation
     */
    MathComputation( ComputationType type, bool iteration_time, ulong value, ulong init_compute_time = 0 );

    /*! Add an option to the computation
     *  \param option_name is the name of the option to add
     *  \param value is the value of the option to add
     */
    void addOption( const std::string &option_name, const std::string &value ) { m_options[option_name] = value; }

    /*! Get a specific option
     *  \param option_name is the name of the option to retrieve
     *  \return the value for the option or an empty string if the option couldn't be found
     */
    std::string getOption( const std::string &option_name ) const;

    /*! Is this computation waiting for a specific amount of messages to compute
     *  \return true if this computation is waiting for a specific amount of messages to compute
     */
    bool isOnCount() const;

    /*! Is this computation waiting for a specific time to compute
     *  \return true if this computation is waiting for a specific time to compute
     */
    bool isOnTimeIteration() const;

    /*! Get the mathematical type of the computation
     *  \return the mathematical type of the computation
     */
    ComputationType getType() const;

    /*! Get the mathematical type of the computation in a human readable string
     *  \return the mathematical type of the computation in a human readable string
     */
    std::string readType() const;

    /*! Get the amount of time to wait before computing
     *  \return the amount of time to wait before computing
     */
    ulong getIterationTime() const;

    /*! Get the amount of messages to reach before computing
     *  \return the amount of messages to reach before computing
     */
    ulong getCount() const;

    /*! Increment the last compute time by the amount of time to wait before each computing */
    void incrementLastComputeTime();

    /*! Set the last compute time
     *  \param compute_time is the time to set
     */
    void setLastComputeTime( ulong compute_time );

    /*! Retrieve the last compute time
     *  \return the last compute time
     */
    ulong getLastComputeTime() const;

    /*! Get a string representation of this MathComputation
     *  \return a string representation of this MathComputation
     */
    std::string serialize() const;

    /*! Retrieve the next time the computation has to run
     *  \return a timestamp in seconds
     */
    ulong nextIterationTime() const;

    /*! Transform a string to a ComputationType
     *  \param string_type is the string to parse
     *  \return a ComputationType according to the given string or UNKNOWN is impossible to recognize
     */
    static ComputationType stringToComputationType( const std::string &string_type );

    /*! Transform a ComputationType to a string
     *  \param computation_type is the one to transform
     *  \return a representation of the given ComputationType to a string
     */
    static std::string computationTypeToString( ComputationType computation_type );

  private:

    /*! Math operation type (sum, average, min, max, ...) */
    ComputationType                    m_type;

    /*! Is this computation time depends on time or number of messages to compute */
    bool                               m_iteration_time;

    /*! Value is either a number of seconds or a number of messages to reach before computing (it depends on m_iteration_time) */
    ulong                              m_value;

    /*! When is the last computed time for this computation (in seconds)
     *  \note this variable will always be equal to zero if m_iteration_time == false
     */
    ulong                              m_last_compute_time;

    /*! Options for the Math Computation (like 'below' or 'multiplicator') */
    std::map<std::string, std::string> m_options;
};

} // namespace maths
} // namespace graphite_proxy

#endif // GRAPHITE_PROXY_MATH_COMPUTATION_HPP
