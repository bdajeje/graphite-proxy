#include "statistics.hpp"

#include <graphite_proxy/utils/time.hpp>

#include <graphite_proxy/models/message.hpp>

#include <boost/make_shared.hpp>

namespace graphite_proxy {

statistics_ptr Statistics::s_instance;

boost::shared_ptr<Statistics> Statistics::init( global_buffer_ptr buffer, maths::maths_ptr math, router_ptr router, long sleep_time, const std::string& hostname )
{
  if (!s_instance)
    s_instance.reset( new Statistics( buffer, math, router, sleep_time, hostname ) );

  return s_instance;
}

Statistics::Statistics( global_buffer_ptr buffer, maths::maths_ptr math, router_ptr router, long sleep_time, const std::string& hostname )
  : Iterations( sleep_time, utils::logging::LOG_HEADER_STATISTICS )
  , m_buffer( buffer )
  , m_math( math )
  , m_router( router )
  , m_hostname(hostname)
{
  // Nothing
}

void Statistics::iteration()
{
  LOG_DEBUG( "Statistics iteration", m_name );
  const ulong timestamp = utils::time::now();

  // Get other statistics values than those stored into m_metrics
  const unsigned long global_buffer_messages_max = m_buffer->getBuffersMaxMessages();
  unsigned long math_buffer_messages_max = 0;
  if(m_math)
    math_buffer_messages_max = m_math->getBuffersMaxMessages();

  {
    boost::mutex::scoped_lock lock( m_mutex );

    // Some global buffers stats
    m_metrics[stats::STATS_GLOBAL_BUFFER_MESSAGES_MAX] = global_buffer_messages_max;

    // Some math buffers stats
    m_metrics[stats::STATS_MATH_BUFFER_MESSAGES_MAX] = math_buffer_messages_max;

    // Stats of stats ;-)
    m_metrics[stats::STATS_STATS_MESSAGES] = m_metrics.size();
  }

  static const std::string stats_header = "graphite_proxy." + m_hostname + ".stats.";

  // Create each message and give it to the router so it can eventually go to the MathsPipeline or directly to the client
  for( auto it = m_metrics.begin(); it != m_metrics.end(); ++it )
    m_router->routeMessage( boost::make_shared<Message>( stats_header + it->first, it->second, timestamp ) );

  {
    // Reset statistic metrics
    boost::mutex::scoped_lock lock( m_mutex );
    m_metrics.clear();
  }
}

void Statistics::raise( const std::string &metric_name, long value )
{
  boost::mutex::scoped_lock lock( m_mutex );

  if ( m_metrics.count(metric_name) > 0 )
    m_metrics[metric_name] += value;
  else m_metrics[metric_name] = value;
}

} // namespace graphite_proxy
