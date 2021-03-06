#ifndef LOGGER_LOGGER_HPP
#define LOGGER_LOGGER_HPP

#include <string>
#include <fstream>
#include <sys/stat.h>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>
#include <vector>
#include <algorithm>

/**
 * Logger
 * <p>
 * Use Logger::init() to start the logger and Logger::exit() to close the logger
 * <p>
 * Simple usage :
 * DEBUG_LOG(FILE_AND_CONSOLE, "Is it simple ? YES");
 * Write 'Is it simple ? YES' (without the quote) in the console and the file
 * <p>
 * Complex usage :
 * INFO_LOG(CONSOLE_ONLY, "Not too complex ? ", "Maybe");
 * Write 'Not toot complex ? Maybe' (without the quote) only in the console
 *
 * <!> WARNING <!>
 * Need -lphtread option for compilation
 * <!> WARNING <!>
 */

/**
 * The log directory's path
 * Change it with your path
 */
#define LOG_PATH "./logs"
/**
 * Project's name
 * Change it with your project's name
 */
#define PROJECT_NAME "project"

/**
 * Log colors
 * Use DEFAULT for reset color
 */
typedef enum LoggerColor {
    DEFAULT,

    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,

    BACKGROUND_BLACK,
    BACKGROUND_RED,
    BACKGROUND_GREEN,
    BACKGROUND_YELLOW,
    BACKGROUND_BLUE,
    BACKGROUND_PURPLE,
    BACKGROUND_CYAN,
    BACKGROUND_WHITE
} LoggerColor;

/**
 * Log types
 */
typedef enum LoggerType {
    INFO,
    SUCCESS,
    ERROR,
    WARNING,
    DEBUG
} LoggerType;

/**
 * Log options
 */
typedef enum LoggerOption {
    FILE_ONLY,
    CONSOLE_ONLY,
    FILE_AND_CONSOLE
} LoggerOption;

class Logger {
private:
    /**
     * Return the color code
     * @param color LoggerColor
     * @return std::string
     */
    static std::string getColor(LoggerColor color);

    /**
     * Return the color code of the log type
     * @param type LoggerType
     * @return std::string
     */
    static std::string getTypeColor(LoggerType type);

    /**
     * Return the name of the log type
     * @param type LoggerType
     * @return std::string
     */
    static std::string getTypeName(LoggerType type);

public:
    /**
     * Initialisation
     */
    static void init(LoggerOption verboseP = FILE_AND_CONSOLE,
                     bool showTraceP = true,
                     const std::vector<LoggerType> &showTypesP = {INFO, SUCCESS, ERROR, WARNING, DEBUG});

    /**
     * Quit the log and close the writer
     */
    static void exit();

public:
    /**
     * Info
     * @param function std::string
     * @param option LoggerOption
     * @param arg const char*
     * @param ...
     */
    template<typename... Ts>
    static void info(const std::string &function, LoggerOption option, Ts const &... args) {
        genericLog(function, stringify(args...), INFO, option);
    }

    /**
     * Success
     * @param function std::string
     * @param option LoggerOption
     * @param arg const char*
     * @param ...
     */
    template<typename... Ts>
    static void success(const std::string &function, LoggerOption option, Ts const &... args) {
        genericLog(function, stringify(args...), SUCCESS, option);
    }

    /**
     * Error
     * @param function std::string
     * @param option LoggerOption
     * @param arg const char*
     * @param ...
     */
    template<typename... Ts>
    static void error(const std::string &function, LoggerOption option, Ts const &... args) {
        genericLog(function, stringify(args...), ERROR, option);
    }

    /**
     * Warning
     * @param function std::string
     * @param option LoggerOption
     * @param arg const char*
     * @param ...
     */
    template<typename... Ts>
    static void warning(const std::string &function, LoggerOption option, Ts const &... args) {
        genericLog(function, stringify(args...), WARNING, option);
    }

    /**
     * Debug
     * @param function std::string
     * @param option LoggerOption
     * @param arg const char*
     * @param ...
     */
    template<typename... Ts>
    static void debug(const std::string &function, LoggerOption option, Ts const &... args) {
        genericLog(function, stringify(args...), DEBUG, option);
    }

private:
    /**
     * Generic log use for all logs
     * @param function std::string
     * @param message std::string
     * @param type LoggerType
     * @param option LoggerOption
     */
    static void genericLog(const std::string &function, const std::string &message, LoggerType type, LoggerOption option);

    /**
     * Write the log into the file
     * @param message std::string
     * @param type LoggerType
     */
    static void writeToFile(const std::string &message, LoggerType type);

    /**
     * The log's hour
     * hh:mm:ss:nnn
     * @return std::string
     */
    static std::string getHour();

    /**
     * The log's date
     * yyyy-mm-dd@hh-mm-ss
     * @return
     */
    static std::string getDate();

private:
    /**
     * The log file
     */
    static std::ofstream file;
    /**
     * The number of log
     */
    static int nbWrite;
    /**
     * A mutex for writeToFile(), to be thread-safe
     */
    static pthread_mutex_t mutex;
    /**
     * The type of verbose
     */
    static LoggerOption verbose;
    /**
     * Show trace or not
     */
    static bool showTrace;
    /**
     * The types of logs that be shown
     */
    static std::vector<LoggerType> showTypes;

private: // Disallow to instance this class
    Logger() = default;

    ~Logger() = default;

private: // Methods used for variadic functions
    template<typename... Ts>
    static std::string stringify(Ts const &... vals) {
        std::string res;

        /*
         * Fill unused array with count(vals)+1 0
         * The syntax (A,B) affect B value to array, but also do A due to comma operator
         */
        int unused[] = {0, (res += toString(vals), 0)...};

        return res;
    }

    template<typename T>
    static typename std::enable_if<false == std::is_convertible<T, std::string>::value, std::string>::type
    toString(T const &val) {
        return std::to_string(val);
    }

    static std::string toString(std::string const &val) {
        return val;
    }
};

#define INFO_LOG(option, msg...) Logger::info(__FUNCTION__, option, msg)
#define SUCCESS_LOG(option, msg...) Logger::success(__FUNCTION__, option, msg)
#define ERROR_LOG(option, msg...) Logger::error(__FUNCTION__, option, msg)
#define WARNING_LOG(option, msg...) Logger::warning(__FUNCTION__, option, msg)
#define DEBUG_LOG(option, msg...) Logger::debug(__FUNCTION__, option, msg)

#endif //LOGGER_LOGGER_HPP
