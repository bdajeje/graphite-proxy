#include "math_computation.hpp"

#include <graphite_proxy/models/maths/properties.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace graphite_proxy {
namespace maths {

MathComputation::MathComputation( ComputationType type, bool iteration_time, ulong value, ulong init_compute_time )
  : m_type(type)
  , m_iteration_time(iteration_time)
  , m_value(value)
  , m_last_compute_time(init_compute_time)
{
  // Nothing
}

std::string MathComputation::getOption( const std::string &option_name ) const
{
  std::map<std::string, std::string>::const_iterator option = m_options.find(option_name);
  if( option == m_options.end() )
    return "";
  else
    return m_options.at(option_name);
}

bool MathComputation::isOnCount() const
{
  return !m_iteration_time;
}

bool MathComputation::isOnTimeIteration() const
{
  return m_iteration_time;
}

ComputationType MathComputation::getType() const
{
  return m_type;
}

std::string MathComputation::readType() const
{
  return MathComputation::computationTypeToString(m_type);
}

ulong MathComputation::getIterationTime() const
{
  return (m_iteration_time) ? m_value : 0;
}

ulong MathComputation::getCount() const
{
  return (m_iteration_time) ? 0 : m_value;
}

void MathComputation::incrementLastComputeTime()
{
  m_last_compute_time += m_value;
}

void MathComputation::setLastComputeTime( ulong compute_time )
{
  m_last_compute_time = compute_time;
}

ulong MathComputation::getLastComputeTime() const
{
  return m_last_compute_time;
}

std::string MathComputation::serialize() const
{
  std::stringstream serialization;
  serialization << "Message type='" << m_type << "', value='" << m_value << "', " << "on_time='" << m_iteration_time << '\'';
  return serialization.str();
}

ulong MathComputation::nextIterationTime() const
{
  return (m_iteration_time) ? m_last_compute_time + m_value : 0;
}

ComputationType MathComputation::stringToComputationType( const std::string &string_type )
{
  std::string lower_type = boost::algorithm::to_lower_copy( string_type );

  if (lower_type == NODE_SUM)
    return SUM;
  else if (lower_type == NODE_AVERAGE)
    return AVERAGE;
  else if (lower_type == NODE_MAX)
    return MAX;
  else if (lower_type == NODE_MIN)
    return MIN;
  else if (lower_type == NODE_MEDIAN)
    return MEDIAN;
  else if (lower_type == NODE_TILES)
    return TILES;
  else if (lower_type == NODE_VARIANCE)
    return VARIANCE;
  else if (lower_type == NODE_DEVIATION)
    return DEVIATION;
  else
  {
    LOG_WARNING( "Unknown mathematical operation: " + lower_type, utils::logging::LOG_HEADER_MATHS_COMPUTATION );
    return UNKNOWN;
  }
}

std::string MathComputation::computationTypeToString( ComputationType computation_type )
{
  switch( computation_type )
  {
    case SUM:       return NODE_SUM;
    case MAX:       return NODE_MAX;
    case MIN:       return NODE_MIN;
    case AVERAGE:   return NODE_AVERAGE;
    case MEDIAN:    return NODE_MEDIAN;
    case TILES:     return NODE_TILES;
    case VARIANCE:  return NODE_VARIANCE;
    case DEVIATION: return NODE_DEVIATION;
    default:        return XML_UNKNOWN;
  }
}

} // namespace maths
} // namespace graphite_proxy
