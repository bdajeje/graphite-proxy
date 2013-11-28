#include "iterations.hpp"

namespace graphite_proxy {

Iterations::Iterations( ulong sleep_time, const std::string &name )
  : m_sleep_time( sleep_time )
  , m_name( name )
  , m_started( false )
  , m_valid( true )
{

}

Iterations::~Iterations()
{
  this->stop();
}

bool Iterations::start()
{
  if ( !m_valid )
  {
    LOG_ERROR( "Timer is invalid, not allowed to start", m_name );
    return false;
  }

  if (this->isStarted())
  {
    LOG_WARNING( "Timer is already started, ignoring call to start()", m_name );
    return false;
  }
  else
  {
    LOG_DEBUG( "Starting timer", m_name );
  }

  m_started = true;
  m_thread = boost::thread( boost::bind( &Iterations::launch, shared_from_this() ) );

  return true;
}

void Iterations::stop()
{
  if (!this->isStarted())
    return;

  m_started = false;
  LOG_INFO( "Stopping timer", m_name );
  m_thread.interrupt();
}

void Iterations::launch()
{
  while (m_started)
  {
    boost::this_thread::sleep( boost::posix_time::seconds( m_sleep_time ) ); // Sleep for the requested time
    this->iteration(); // Wake up, do what you have to do, and sleep again!
  }
}

} // namespace graphite_proxy
