#include "time.hpp"

#include <graphite_proxy/utils/logging/log_headers.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

namespace graphite_proxy {
namespace utils {
namespace time {

ulong now()
{
  return boost::chrono::duration_cast<boost::chrono::seconds>( boost::chrono::system_clock::now().time_since_epoch() ).count();
}

std::string humanDateTime()
{
  return boost::posix_time::to_simple_string( boost::posix_time::microsec_clock::local_time() );
}

ulong parseTime( const std::string &sentence )
{
  ulong result = 0;
  std::string buffer;
  std::string::size_type i, length = sentence.size();

  for ( i = 0; i < length; i++ )
  {
    char character = sentence[i];

    if( character == 'h' || character == 'H' )
    {
      result += parseLong( buffer ) * 3600;
      buffer = "";
    }
    else if( character == 'm' || character == 'M' )
    {
      result += parseLong( buffer ) * 60;
      buffer = "";
    }
    else if( character == 's' || character == 'S' )
    {
      result += parseLong( buffer );
      buffer = "";
    }
    else buffer += character;
  }

  return result;
}

ulong parseLong( const std::string &sentence )
{
  try
  {
    return boost::lexical_cast<ulong>(sentence);
  }
  catch( const boost::bad_lexical_cast &e )
  {
    LOG_ERROR( std::string("Can't parse: ") + e.what(), utils::logging::LOG_HEADER_UTILS );
    return 0;
  }
}

} // namespace time
} // namespace utils
} // namespace graphite_proxy
