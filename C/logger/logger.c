#include "logger.h"

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
 * Generic log use for all logs
 * @param function const char*
 * @param message char*
 * @param type LoggerType
 * @param option LoggerOption
 */
void genericLog(const char *function, char *message, LoggerType type, LoggerOption option);

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

/**
 * Return true if showType contains type
 * @param type LoggerType
 * @return bool
 */
bool showTypesContains(LoggerType type);

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * The log file
 */
FILE *file = NULL;
/**
 * If the file is open
 */
bool fileOpen = false;
/**
 * The number of log
 */
int nbWrite = 0;
/**
 * A mutex to be thread safe when write to file
 */
pthread_mutex_t mutex;
/**
 * The type of verbose
 */
LoggerOption verbose;
/**
 * Show trace or not
 */
bool showTrace;
/**
 * The type of logs that be shown
 */
LoggerType showTypes[5];

char *getColor(LoggerColor color) {
    switch (color) {
        case DEFAULT:
            return "\x1B[00m";

        case BLACK:
            return "\x1B[30m";
        case RED:
            return "\x1B[31m";
        case GREEN:
            return "\x1B[32m";
        case YELLOW:
            return "\x1B[33m";
        case BLUE:
            return "\x1B[34m";
        case PURPLE:
            return "\x1B[35m";
        case CYAN:
            return "\x1B[36m";
        case WHITE:
            return "\x1B[37m";

        case BACKGROUND_BLACK:
            return "\x1B[40m";
        case BACKGROUND_RED:
            return "\x1B[41m";
        case BACKGROUND_GREEN:
            return "\x1B[42m";
        case BACKGROUND_YELLOW:
            return "\x1B[43m";
        case BACKGROUND_BLUE:
            return "\x1B[44m";
        case BACKGROUND_PURPLE:
            return "\x1B[45m";
        case BACKGROUND_CYAN:
            return "\x1B[46m";
        case BACKGROUND_WHITE:
            return "\x1B[47m";

        default:
            return "\x1B[00m";
    }
}

char *getTypeColor(LoggerType type) {
    switch (type) {
        case INFO:
            return getColor(BLUE);
        case SUCCESS:
            return getColor(GREEN);
        case ERROR:
            return getColor(RED);
        case WARNING:
            return getColor(YELLOW);
        case DEBUG:
            return getColor(PURPLE);

        default:
            return getColor(DEFAULT);
    }
}

char *getTypeName(LoggerType type) {
    switch (type) {
        case INFO:
            return "INFO";
        case SUCCESS:
            return "SUCCESS";
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARNING";
        case DEBUG:
            return "DEBUG";

        default:
            return "";
    }
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

void logger_init(LoggerOption verboseP, bool showTraceP, const LoggerType showTypesP[5]) {
    if (!fileOpen) {
        verbose = verboseP;
        showTrace = showTraceP;
        for (int i = 0; i < 5; ++i) {
            showTypes[i] = showTypesP[i];
        }

        bool dirCreated = false;

        if (mkdir(LOG_PATH, S_IRWXU) == 0) {
            dirCreated = true;
        }

        char fileName[100];
        sprintf(fileName, "%s/%s_log_%s.log", LOG_PATH, PROJECT_NAME, getDate());
        errno = -1;
        file = fopen(fileName, "w");
        if (errno != -1) {
            ERROR_LOG(CONSOLE_ONLY, "Error log file open : %d\n", errno);
            exit(EXIT_FAILURE);
        }
        fileOpen = true;

        if (pthread_mutex_init(&mutex, NULL) != 0) {
            ERROR_LOG(CONSOLE_ONLY, "Error mutex : %d\n", errno);
        }

        INFO_LOG(FILE_ONLY, "Log start\n");
        if (dirCreated)
            WARNING_LOG(FILE_AND_CONSOLE, "Log directory created\n");
    } else
        WARNING_LOG(FILE_AND_CONSOLE, "Log already init\n");
}

void logger_exit() {
    if (fileOpen) {
        INFO_LOG(FILE_ONLY, "End log\n");
        fclose(file);
        file = NULL;
        fileOpen = false;

        pthread_mutex_destroy(&mutex);
    } else
        ERROR_LOG(CONSOLE_ONLY, "Please init before exit\n");
}

void genericLog(const char *function, char *message, LoggerType type, LoggerOption option) {
    char *t = malloc(sizeof(char) * MAX_MESSAGE);
    if (showTrace) {
        sprintf(t, "[%s]\t%s", function, message);
    } else {
        sprintf(t, "%s", message);
    }

    if (option != FILE_ONLY && verbose != FILE_ONLY && showTypesContains(type))
        printf("%s%s%s", getTypeColor(type), t, getColor(DEFAULT));

    if (option != CONSOLE_ONLY && verbose != CONSOLE_ONLY) {
        pthread_mutex_lock(&mutex);
        writeToFile(t, type);
        pthread_mutex_unlock(&mutex);
    }
}

void info(const char *function, LoggerOption option, char *format, ...) {
    va_list vaList;
    va_start(vaList, format);
    char *message = malloc(sizeof(char) * MAX_MESSAGE);
    vsprintf(message, format, vaList);
    genericLog(function, message, INFO, option);
    va_end(vaList);
}

void success(const char *function, LoggerOption option, char *format, ...) {
    va_list vaList;
    va_start(vaList, format);
    char *message = malloc(sizeof(char) * MAX_MESSAGE);
    vsprintf(message, format, vaList);
    genericLog(function, message, SUCCESS, option);
    va_end(vaList);
}

void error(const char *function, LoggerOption option, char *format, ...) {
    va_list vaList;
    va_start(vaList, format);
    char *message = malloc(sizeof(char) * MAX_MESSAGE);
    vsprintf(message, format, vaList);
    genericLog(function, message, ERROR, option);
    va_end(vaList);
}

void warning(const char *function, LoggerOption option, char *format, ...) {
    va_list vaList;
    va_start(vaList, format);
    char *message = malloc(sizeof(char) * MAX_MESSAGE);
    vsprintf(message, format, vaList);
    genericLog(function, message, WARNING, option);
    va_end(vaList);
}

void debug(const char *function, LoggerOption option, char *format, ...) {
    va_list vaList;
    va_start(vaList, format);
    char *message = malloc(sizeof(char) * MAX_MESSAGE);
    vsprintf(message, format, vaList);
    genericLog(function, message, DEBUG, option);
    va_end(vaList);
}

void writeToFile(char *message, LoggerType type) {
    if (fileOpen) {
        char *toPrint = malloc(sizeof(char) * MAX_MESSAGE);
        sprintf(toPrint, "[%d-%s-%s]\t%s", nbWrite, getHour(), getTypeName(type), message);

        fwrite(toPrint, 1, strlen(toPrint), file);
        fflush(file);
        nbWrite++;
    } else
        ERROR_LOG(CONSOLE_ONLY, "Please init logger\n");
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

char *getHour() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    struct tm *t = localtime(&now.tv_sec);

    char *res = malloc(sizeof(char) * 100);
    sprintf(res, "%02d:%02d:%02d:%03ld", t->tm_hour, t->tm_min, t->tm_sec, now.tv_nsec);

    return res;
}

char *getDate() {
    time_t now;
    time(&now);

    struct tm *t = localtime(&now);

    char *res = malloc(sizeof(char) * 100);
    sprintf(res, "%4d-%02d-%02d@%02d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
            t->tm_sec);

    return res;
}

bool showTypesContains(LoggerType type) {
    for (int i = 0; i < 5; ++i)
        if (showTypes[i] == type)
            return true;

    return false;
}