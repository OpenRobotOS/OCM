// log_anywhere.hpp
#pragma once

#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <set>
#include <string>

namespace openrobot::ocm {

/**
 * @brief Concatenates elements of a set into a single string separated by a delimiter.
 *
 * This function takes a `std::set<std::string>` and a delimiter string,
 * then joins all elements of the set into one string, separated by the specified delimiter.
 *
 * @param s The set of strings to concatenate.
 * @param delimiter The string used to separate the elements.
 * @return A single concatenated string with elements separated by the delimiter.
 *
 * @example
 * @code
 * std::set<std::string> my_set = {"apple", "banana", "cherry"};
 * std::string result = JointStrSet(my_set, ", ");
 * // result: "apple, banana, cherry"
 * @endcode
 */
inline std::string JointStrSet(const std::set<std::string>& s, const std::string& delimiter) {
  std::string result;
  for (auto it = s.begin(); it != s.end(); ++it) {
    if (it != s.begin()) {
      result += delimiter;
    }
    result += *it;
  }
  return result;
}

/**
 * @enum ColorEnum
 * @brief Enumeration of colors for colored output.
 *
 * Defines various colors that can be used to colorize strings in the console.
 */
enum class ColorEnum : uint8_t {
  RED = 0, /**< Red color */
  GREEN,   /**< Green color */
  YELLOW,  /**< Yellow color */
  BLUE,    /**< Blue color */
  MAGENTA, /**< Magenta color */
  CYAN,    /**< Cyan color */
  WHITE    /**< White color */
};

/**
 * @brief Applies ANSI color codes to a string for colored console output.
 *
 * This function takes a string and a color enum, then wraps the string with the corresponding
 * ANSI color codes to display colored text in supported terminals.
 *
 * @param str The input string to colorize.
 * @param color The color to apply, specified by the `ColorEnum` enumeration.
 * @return A new string wrapped with ANSI color codes.
 *
 * @example
 * @code
 * std::string colored_text = ColorPrint("Success!", ColorEnum::GREEN);
 * // Output will display "Success!" in green color in the console.
 * @endcode
 */
inline std::string ColorPrint(const std::string& str, ColorEnum color) {
  std::string color_code;

  // Set ANSI color codes based on ColorEnum values
  switch (color) {
    case ColorEnum::RED:
      color_code = "\033[31m";  // Red
      break;
    case ColorEnum::GREEN:
      color_code = "\033[32m";  // Green
      break;
    case ColorEnum::YELLOW:
      color_code = "\033[33m";  // Yellow
      break;
    case ColorEnum::BLUE:
      color_code = "\033[34m";  // Blue
      break;
    case ColorEnum::MAGENTA:
      color_code = "\033[35m";  // Magenta
      break;
    case ColorEnum::CYAN:
      color_code = "\033[36m";  // Cyan
      break;
    case ColorEnum::WHITE:
      color_code = "\033[37m";  // White
      break;
    default:
      color_code = "\033[0m";  // Reset
      break;
  }

  // Reset color
  std::string reset_code = "\033[0m";

  // Return the colorized string
  return color_code + str + reset_code;
}

/**
 * @struct LoggerConfig
 * @brief Configuration settings for the LogAnywhere logger.
 *
 * This structure holds the configuration parameters required to initialize the logger,
 * including log file path, queue size for asynchronous logging, and the number of backend threads.
 */
struct LoggerConfig {
  std::string log_file = "logs/log.txt"; /**< Path to the log file */
  size_t queue_size = 8192;              /**< Size of the asynchronous logging queue */
  size_t thread_count = 1;               /**< Number of backend threads for logging */
};

/**
 * @class LogAnywhere
 * @brief Singleton class managing logging using spdlog.
 *
 * The LogAnywhere class initializes and manages the spdlog logger with both file and console sinks.
 * It ensures that logging is set up correctly with asynchronous capabilities and provides a global
 * access point to the logger.
 */
class LogAnywhere {
 public:
  /**
   * @brief Constructs the LogAnywhere singleton with the given configuration.
   *
   * Initializes the spdlog logger with file and console sinks, sets the logging level and format,
   * and registers the logger for global access.
   *
   * @param config Configuration settings for the logger.
   */
  LogAnywhere(const LoggerConfig& config);

  /**
   * @brief Default destructor.
   *
   * Shuts down the spdlog logger, ensuring all logs are flushed and resources are cleaned up.
   */
  ~LogAnywhere();

  // Delete copy constructor and assignment operator to prevent copying
  LogAnywhere(const LogAnywhere&) = delete;
  LogAnywhere& operator=(const LogAnywhere&) = delete;

 private:
  std::shared_ptr<spdlog::logger> logger_; /**< Shared pointer to the spdlog logger instance */
};

/**
 * @brief Retrieves the logger instance for "openrobot_ocm_logger".
 *
 * This function checks if a logger named "openrobot_ocm_logger" exists.
 * If the logger is not found, a default logger is created and returned.
 * The default logger writes logs to a file named "logs/default.log".
 *
 * @return A shared pointer to the logger instance.
 * @throws std::runtime_error If the logger cannot be initialized or found.
 */
inline std::shared_ptr<spdlog::logger> GetLogger() {
  auto logger = spdlog::get("openrobot_ocm_logger");
  if (!logger) {
    // If the logger is not found, create a default logger
    logger = spdlog::basic_logger_mt("openrobot_ocm_logger", "logs/default.log");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
  }
  return logger;
}

}  // namespace openrobot::ocm
