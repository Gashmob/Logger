#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

/**
 * Logger
 * <p>
 * Use logger_init() to start the logger and logger_exit() to close the logger
 * <p>
 * Simple usage :
 * debug(FILE_AND_CONSOLE, "Is it simple ? YES");
 * Write 'Is it simple ? YES' (without the quote) in the console and the file
 * <p>
 * Complex usage :
 * info(CONSOLE_ONLY, "Not too complex ?", "Maybe");
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
 * Max size of a log message
 */
#define MAX_MESSAGE 100

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

/**
 * Initialisation
 * @param verboseP
 * @param showTraceP
 * @param showTypesP
 */
void logger_init(LoggerOption verboseP, bool showTraceP, const LoggerType showTypesP[5]);

/**
 * Quit the log and close the writer
 */
void logger_exit();

/**
 * Info
 * @param function const char*
 * @param option LoggerOption
 * @param format char*
 * @param ...
 */
void info(const char* function, LoggerOption option, char *format, ...);

/**
 * Success
 * @param function const char*
 * @param option LoggerOption
 * @param format char*
 * @param ...
 */
void success(const char* function, LoggerOption option, char *format, ...);

/**
 * Error
 * @param function const char*
 * @param option LoggerOption
 * @param format char*
 * @param ...
 */
void error(const char* function, LoggerOption option, char *format, ...);

/**
 * Warning
 * @param function const char*
 * @param option LoggerOption
 * @param format char*
 * @param ...
 */
void warning(const char* function, LoggerOption option, char *format, ...);

/**
 * Debug
 * @param function const char*
 * @param option LoggerOption
 * @param format char*
 * @param ...
 */
void debug(const char* function, LoggerOption option, char *format, ...);

#define INFO_LOG(option, format...) info(__func__, option, format);
#define SUCCESS_LOG(option, format...) success(__func__, option, format);
#define ERROR_LOG(option, format...) error(__func__, option, format);
#define WARNING_LOG(option, format...) warning(__func__, option, format);
#define DEBUG_LOG(option, format...) debug(__func__, option, format);

#endif //LOGGER_LOGGER_H
