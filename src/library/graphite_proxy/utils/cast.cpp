#include "cast.hpp"

namespace graphite_proxy {
namespace utils {
namespace cast {

std::string toString( bool value, bool english_mode )
{
  if(value)
    return english_mode ? "yes" : "true";
  else
    return english_mode ? "no" : "false";
}

} // namespace cast
} // namespace utils
} // namespace graphite_proxy
