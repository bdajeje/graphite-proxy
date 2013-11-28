#ifndef SYSTEM_HELPER_HPP
#define SYSTEM_HELPER_HPP

#include <string>
#include <unistd.h>

namespace server {
namespace utils {

/*! Helper class to retrieve some process/system informations */
class SystemHelper
{
  public:

    /*! Retrieve the process ID
     *  \return the process id
     */
    static pid_t getProcessID();

    /*! Retrieve the hostname of the machine
     *  \return this hostname of the machine
     */
    inline static const std::string& getHostname() { static std::string hostname = initHostname(); return hostname; }

  private:

    SystemHelper();

    /*! Retrieve the hostname of the system
     *  \return a string representing the hostname of the system
     */
    static std::string initHostname();
};

} // namespace utils
} // namespace server

#endif // SYSTEM_HELPER_HPP
