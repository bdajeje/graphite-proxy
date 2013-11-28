#include "logger.hpp"

#include <graphite_proxy/utils/logging/log_headers.hpp>
#include <graphite_proxy/utils/time.hpp>

#include "properties.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace graphite_proxy {
namespace utils {
namespace logging {

logger_ptr Logger::s_instance;

void Logger::init( const std::string& destination, LogLevel level )
{
  s_instance.reset( new Logger( destination, level ) );
}

void Logger::init( const std::string& destination, const std::string& log_level )
{
  s_instance.reset( new Logger( destination, toLevel(log_level) ) );
}

Logger::Logger( const std::string& destination, LogLevel log_level )
  : m_level( log_level )
{
  // Open the log file to write if required
  if( !destination.empty() && destination != server::props::PROPERTIES_LOGS_DESTINATION_DEFAULT )
  {
    m_log_file.open(destination);

    if( !m_log_file.is_open() )
    {
      std::cerr << "Can't open file to write: " << destination << std::endl;
      std::cerr << "Logs will be written on standard output" << std::endl;
    }
  }
}

Logger::~Logger()
{
  // Close opened file
  if( m_log_file.is_open() )
    m_log_file.close();
}

Logger::LogLevel Logger::getLevel()
{
  logger_ptr logger = Logger::instance();
  if( logger )
    return logger->m_level;
  else return Logger::LogLevel::QUIET;
}

void Logger::setColor( const std::string &header, Color color_code )
{
  logger_ptr logger = Logger::instance();
  if( logger )
    logger->m_colors[header] = color_code;
}

void Logger::setLevel( LogLevel level )
{
  logger_ptr logger = Logger::instance();
  if( logger )
    logger->m_level = (level <= LogLevel::DEBUG) ? level : LogLevel::DEBUG;
}

void Logger::setLevel( std::string level )
{
  Logger::setLevel( Logger::toLevel(level) );
}

Logger::LogLevel Logger::toLevel( std::string level )
{
  boost::algorithm::to_lower( level );

  if(level == level::Warning)
    return LogLevel::WARNING;
  else if(level == level::Error)
    return LogLevel::ERROR;
  else if(level == level::Info)
    return LogLevel::INFO;
  else if(level == level::Debug)
    return LogLevel::DEBUG;
  else if(level == level::Quiet)
    return LogLevel::QUIET;
  else
  {
    // Let's say to the user his configuration is not good
    LOG_WARNING( "Specify log level '" + level + "' doesn't exist, setting log level to default mode: warning", LOG_HEADER_LOGGER );
    return LogLevel::DEBUG;
  }
}

bool Logger::isLogging( LogLevel level )
{
  logger_ptr logger = Logger::instance();
  return (logger && level <= logger->m_level) ? true : false;
}

void Logger::log( LogLevel level, const std::string &log_message, const std::string &header ) const
{
  // Is this log message accepted by the log level
  if(!this->isLogging( level ))
    return;

  std::stringstream message;

  // Add message level
  static const char separator = '\t';
  if( level == LogLevel::INFO )
  {
    static const std::string info_header = boost::algorithm::to_upper_copy( level::Info ) + separator;
    message << info_header;
  }
  else if( level == LogLevel::WARNING )
  {
    static const std::string warning_header = boost::algorithm::to_upper_copy( level::Warning ) + separator;
    message << warning_header;
  }
  else if( level == LogLevel::ERROR )
  {
    static const std::string error_header = boost::algorithm::to_upper_copy( level::Error ) + separator;
    message << error_header;
  }
  else if( level == LogLevel::DEBUG )
  {
    static const std::string debug_header = boost::algorithm::to_upper_copy( level::Debug ) + separator;
    message << debug_header;
  }

  // Add message header
  if( !header.empty() )
  {
    message << '[';
    message << header;
    message << "]\t";
  }

  // Add timestamp
  message << utils::time::humanDateTime();
  message << '\t';

  // Add message content
  message << log_message;

  // Write either on file or stdout
  // Add color only for stdout mode
  if( !m_log_file.is_open() )
  {
    // Find message color
    auto color = m_colors.find( header );
    if( color != m_colors.end() )
    {
      std::cout << Logger::color( message.str(), color->second ) << std::endl;
    }
    else std::cout << message.str() << std::endl;
  }
  else
  {
    // Write in file
    m_log_file << message.str() << std::endl;
  }
}

std::string Logger::color( const std::string& message, Color color )
{
  if( color == Color::None )
    return message;

  const std::string color_str = std::to_string(static_cast<short>(color));
  return std::string("\033[0;") + color_str + 'm' + message + "\033[0m";
}

} // namespace logging
} // namespace utils
} // namespace graphite_proxy

