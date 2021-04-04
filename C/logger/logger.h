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
 * Use init() to start the logger and exit() to close the logger
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

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * Return the color code
 * @param color LoggerColor
 * @return char*
 */
char *getColor(LoggerColor color);

/**
 * Return the color code of the log type
 * @param type LoggerType
 * @return char*
 */
char *getTypeColor(LoggerType type);

/**
 * Return the name of the log type
 * @param type LoggerType
 * @return
 */
char *getTypeName(LoggerType type);

/**
 * Initialisation
 */
void logger_init();

/**
 * Quit the log and close the writer
 */
void logger_exit();

/**
 * Generic log use for all logs
 * @param args char**
 * @param type LoggerType
 * @param option LoggerOption
 */
void genericLog(char *message, LoggerType type, LoggerOption option);

/**
 * Info
 * @param option LoggerOption
 * @param arg char*
 * @param ...
 */
void info(LoggerOption option, char *arg, ...);

/**
 * Success
 * @param option LoggerOption
 * @param arg char*
 * @param ...
 */
void success(LoggerOption option, char *arg, ...);

/**
 * Error
 * @param option LoggerOption
 * @param arg char*
 * @param ...
 */
void error(LoggerOption option, char *arg, ...);

/**
 * Warning
 * @param option LoggerOption
 * @param arg char*
 * @param ...
 */
void warning(LoggerOption option, char *arg, ...);

/**
 * Debug
 * @param option LoggerOption
 * @param arg char*
 * @param ...
 */
void debug(LoggerOption option, char *arg, ...);

/**
 * Write the log into the file
 * @param message char*
 * @param type LoggerType
 */
void writeToFile(char *message, LoggerType type);

/**
 * The log's hour
 * hh:mm:ss:nnn
 * @return char*
 */
char *getHour();

/**
 * The log's date
 * yyyy-mm-dd@hh-mm-ss
 * @return char*
 */
char *getDate();

#endif //LOGGER_LOGGER_H
