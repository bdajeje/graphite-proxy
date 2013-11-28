#include <graphite_proxy/utils/files.hpp>

namespace graphite_proxy {
namespace utils {
namespace files {

std::string findFilePath( const std::string &application_path )
{
  std::string::size_type last_occurence = application_path.find_last_of( '/' );

  if (last_occurence > 2 && last_occurence < application_path.npos)
  {
    std::string access_path = application_path.substr( 0, last_occurence );
    if ( !access_path.empty() && access_path[access_path.length()-1] != '/' )
      access_path += '/';

    return access_path;
  }
  else return std::string();
}

} // namespace files
} // namespace utils
} // namespace graphite_proxy
