#include <graphite_proxy/utils/command_line.hpp>

namespace graphite_proxy {
namespace utils {
namespace cl {

bool argsContain( int argc, char **argv, const std::string aliases[], unsigned int aliases_size )
{
  unsigned int i;
  int j;

  for( i = 0; i < aliases_size; i++ )
  {
    for( j = 0; j < argc; j++ )
    {
      if ( aliases[i] == argv[j] )
        return true;
    }
  }

  return false;
}

} // namespace cl
} // namespace utils
} // namespace graphite_proxy
