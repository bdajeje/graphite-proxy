#include "system_helper.hpp"

#include <string.h>

namespace server {
namespace utils {

SystemHelper::SystemHelper()
{
  // Nothing
}

pid_t SystemHelper::getProcessID()
{
  return getpid();
}

std::string SystemHelper::initHostname()
{
  const unsigned int size = 1024;
  char result[size];

  if( gethostname( result, size ) != 0 )
    strncpy( result, "unknown\0", 8 );

  return result;
}

} // namespace utils
} // namespace server
