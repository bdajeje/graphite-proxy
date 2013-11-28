#ifndef CURRENT_STATE_HPP
#define CURRENT_STATE_HPP

#include <graphite_proxy/models/buffers/global_buffer.hpp>
#include <graphite_proxy/networking/client.hpp>
#include <graphite_proxy/models/timer.hpp>
#include <graphite_proxy/models/statistics/statistics.hpp>
#include <graphite_proxy/models/maths/pipeline.hpp>
#include <networking/server.hpp>
#include <networking/udp_server.hpp>

#include <string>
#include <map>
#include <list>
#include <vector>

namespace server {
namespace utils {

namespace logs = graphite_proxy::utils::logging;

/*! This class retrieves current informations about the program (number of buffers, pending messages, etc)
 *  and save them into a file
 */
class CurrentState
{
  public:

    /*! Constructor
     *  \param filepath is the filepath where to save the results
     */
    CurrentState(const std::string& filepath, const graphite_proxy::networking::client_ptr client
                                            , const graphite_proxy::global_buffer_ptr global_buffer
                                            , const graphite_proxy::timer_ptr timer
                                            , const graphite_proxy::statistics_ptr stats
                                            , const graphite_proxy::maths::maths_ptr maths
                                            , const server::networking::tcp_server_ptr tcp_server
                                            , const server::networking::udp_server_ptr udp_server);

    /*! Write the current state into the save file
     *  \return a string containing what's been written into the save file
     */
    std::string save() const;

  private:

    // Helper functions to write
    std::string writeHeader( const std::string& header_name ) const;

    std::string minLength(  const std::string& text, const unsigned int length, const char character = ' ' ) const;

    std::string addTableLine( const std::string& cell1, unsigned int min_cell1_length, const std::string& cell2, unsigned int min_cell2_length ) const;

    logs::Color getColor( float percentage ) const;

    // Functions to show current state
    std::string showClient() const;
    std::string showGlobalBuffer() const;
    std::string showTimer() const;
    std::string showStats() const;
    std::string showMaths() const;
    std::string showServer() const;

  private:

    struct ServerInformation
    {
      ServerInformation()
        : address("none")
        , port(0)
      {}

      ServerInformation(const std::string& address_, unsigned short port_)
        : address(address_)
        , port(port_)
      {}

      std::string    address;
      unsigned short port;
    };

    std::string                                                                m_filepath;

    bool                                                                       m_client_is_ready;

    bool                                                                       m_gbuffer_drop_oldest;

    unsigned long                                                              m_gbuffer_max_size;

    std::map<std::string, unsigned long>                                       m_gbuffer_buffer_sizes;

    long                                                                       m_timer_flush_time;

    bool                                                                       m_stats_enabled;

    unsigned long                                                              m_maths_buffer_max_size;

    std::list<graphite_proxy::maths::MathsCategory*>                           m_maths_categories;

    std::map<std::string, std::vector<graphite_proxy::maths::MathOperation*>>  m_maths_buffers;

    ServerInformation                                                          m_tcp_server_info;

    ServerInformation                                                          m_udp_server_info;
};

} // namespace utils
} // namespace server

#endif // CURRENT_STATE_HPP
