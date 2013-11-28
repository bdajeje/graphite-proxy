#ifndef GRAPHITE_PROXY_LOGGER_HPP
#define GRAPHITE_PROXY_LOGGER_HPP

#include <boost/shared_ptr.hpp>

#include <string>
#include <map>
#include <fstream>

namespace graphite_proxy {
namespace utils {
namespace logging {

namespace level {
  static const std::string Quiet   = "quiet";
  static const std::string Debug   = "debug";
  static const std::string Info    = "info";
  static const std::string Warning = "warning";
  static const std::string Error   = "error";
}

/*! Possible colors for logging */
enum class Color : short { None = 0, Black = 30, Red = 31, Green = 32, Yellow = 33, Blue = 34, Purple = 35, Cyan = 36 };

/*! Logger class
 *  \note this class is a singleton
 */
class Logger
{
  public:

    /*! Log levels
     *  \note QUIET: show no logs
     *  \note ERROR: show only error logs
     *  \note WARNING: show error and warning logs
     *  \note INFO: show all logs except debug
     *  \note DEBUG: show all logs
     */
    enum class LogLevel { QUIET, ERROR, WARNING, INFO, DEBUG };

    /*! Destructor */
    ~Logger();

    /*! Init the instance of logger with a specific log level
     *  \param destination where to log messages
     *  \param log_level   is a specific log level to set for the application
     */
    static void init( const std::string& destination, LogLevel log_level );

    /*! Init the instance of logger with a specific log level
     *  \param destination where to log messages
     *  \param log_level   is a specific log level to set for the application
     */
    static void init( const std::string& destination, const std::string& log_level );

    /*! Get the logger unique instance
     *  \return a logger instance
     */
    static const boost::shared_ptr<Logger> instance() { return s_instance; }

    /*! Helper function to know if the logger current configuration allows logging a specific level
     *  \param level is the asking level
     *  \return true if the given level is currently loggable, false if not
     */
    static bool isLogging( LogLevel level );

    /*! Set log level
     *  \param level is the new log level to set
     *  \note in case of unrecognized level set value to DEBUG
     */
    static void setLevel( LogLevel level );

    /*! Set log level (helper function to set log level with a string, not a LogLevel )
     *  \param level is the new log level to set
     *  \note in case of unrecognized level set value to DEBUG
     */
    static void setLevel( std::string level );

    /*! Helper method to convert a string to a log level
     *  \param level string to converft
     *  \return the matching log level. In case of error, returns LogLevel::DEBUG
     */
    static LogLevel toLevel( std::string level );

    /*! Get the current log level
     *  \return the current log level
     */
    static LogLevel getLevel();

    /*! Set a color code to a specific header
     *  \param header is the logging header entity to apply the color
     *  \param color_code is the color code to apply
     */
    static void setColor( const std::string &header, Color color_code );

    /*! Log a message at desired level
     *  \param level is the message log level
     *  \param log_message is the message to log
     *  \param header is a optional header to put at the beginning of the log message. This header will be surround by '[' and ']' characters and followed by a tabulation (default empty string)
     */
    void log( LogLevel level, const std::string &log_message, const std::string &header = std::string() ) const;

    /*! Get a colored string
     *  \param message to color
     *  \param color   to set
     *  \note The color has to be a unix command line color, like "0;31"
     */
    static std::string color( const std::string& message, Color color );

  protected:

    /*! Protected constructor
     *  \param destination  where to log messages
     *  \param filter_level is the log level to set (defaul DEBUG)
     */
    Logger( const std::string& destination, LogLevel filter_level );

  private:

    /*! Unique instance of logger class */
    static boost::shared_ptr<Logger>   s_instance;

    /*! Logger message level */
    LogLevel                           m_level;

    /*! Associated a logging header to a specific color */
    std::map<std::string, Color>       m_colors;

    /*! File where to write logs if destination set to file */
    mutable std::ofstream              m_log_file;
};

typedef boost::shared_ptr<Logger> logger_ptr;

} // namespace logging
} // namespace utils
} // namespace graphite_proxy

#define LOG_MESSAGE( level, message, header )\
{\
  boost::shared_ptr<graphite_proxy::utils::logging::Logger> logger = graphite_proxy::utils::logging::Logger::instance();\
  if ( logger && graphite_proxy::utils::logging::Logger::isLogging( level ) )\
    logger->log( level, message, header );\
}

#define LOG_ERROR( message, header )\
{\
  LOG_MESSAGE( graphite_proxy::utils::logging::Logger::LogLevel::ERROR, message, header );\
}

#define LOG_WARNING( message, header )\
{\
  LOG_MESSAGE( graphite_proxy::utils::logging::Logger::LogLevel::WARNING, message, header );\
}

#define LOG_INFO( message, header )\
{\
  LOG_MESSAGE( graphite_proxy::utils::logging::Logger::LogLevel::INFO, message, header );\
}

#define LOG_DEBUG( message, header )\
{\
  LOG_MESSAGE( graphite_proxy::utils::logging::Logger::LogLevel::DEBUG, message, header );\
}

#endif // GRAPHITE_PROXY_LOGGER_HPP
