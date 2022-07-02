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
 * Construct the message from the format
 * Different rules for the formats :
 * %Y -> Year
 * %M -> Month
 * %D -> Day
 * %H -> Hour
 * %m -> Minute
 * %S -> Second
 * %N -> Nano second
 * %d -> Date (%Y-%M-%D@%H-%m-%S)
 * %h -> Hour (%H:%m:%S:%N)
 * %T -> Trace
 * %C -> Content message
 * %n -> Log number
 * %t -> Log type
 *
 * @param message
 * @param trace
 * @param logType
 * @param format
 * @return
 */
char *constructMessage(const char *message, const char *trace, const char *logType, const char *format);

/**
 * Write the log into the file
 * @param message char*
 */
void writeToFile(char *message);

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
 * If the logger is initialized
 */
bool isInitialized = false;
/**
 * The log file
 */
FILE *file = NULL;
/**
 * Additional output for the logs
 */
FILE **additionalStreams;
/**
 * Number of additional stream added
 */
unsigned int nbAddStream = 0;
/**
 * If the file is open
 */
bool fileOpen = false;
/**
 * The number of log
 */
int nbLog = 0;
/**
 * A mutex to be thread safe when write to file
 */
pthread_mutex_t mutex;
/**
 * The type of verbose
 */
LoggerOption verbose;
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

void logger_init(LoggerOption verboseP, const LoggerType showTypesP[5]) {
    if (!isInitialized) {
        additionalStreams = calloc(MAX_ADDITIONAL_STREAM, sizeof(FILE));
        verbose = verboseP;
        for (int i = 0; i < 5; ++i) {
            showTypes[i] = showTypesP[i];
        }

        bool dirCreated = false;

        if (verboseP != CONSOLE_ONLY) {
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
        }

        if (pthread_mutex_init(&mutex, NULL) != 0) {
            ERROR_LOG(CONSOLE_ONLY, "Error mutex : %d\n", errno);
        }

        INFO_LOG(FILE_ONLY, "Log start\n");
        isInitialized = true;
        if (dirCreated)
            WARNING_LOG(FILE_AND_CONSOLE, "Log directory created\n");
    } else
        WARNING_LOG(FILE_AND_CONSOLE, "Log already init\n");
}

void logger_exit() {
    if (isInitialized) {
        INFO_LOG(FILE_ONLY, "End log\n");
        isInitialized = false;
        if (fileOpen) {
            fclose(file);
            file = NULL;
            fileOpen = false;
        }

        pthread_mutex_destroy(&mutex);
    } else
        ERROR_LOG(CONSOLE_ONLY, "Please init before exit\n");
}

void logger_addOutputStream(FILE *stream) {
    additionalStreams[nbAddStream++] = stream;
}

void genericLog(const char *function, char *message, LoggerType type, LoggerOption option) {
    if (option != FILE_ONLY && verbose != FILE_ONLY && showTypesContains(type))
        printf("%s%s%s", getTypeColor(type), constructMessage(message, function, getTypeName(type), CONSOLE_FORMAT),
               getColor(DEFAULT));

    if (option != CONSOLE_ONLY && verbose != CONSOLE_ONLY) {
        pthread_mutex_lock(&mutex);
        writeToFile(constructMessage(message, function, getTypeName(type), FILE_FORMAT));
        pthread_mutex_unlock(&mutex);
    }

    if (option != CONSOLE_ONLY && option != FILE_ONLY && verbose == FILE_AND_CONSOLE) {
        for (int i = 0; i < nbAddStream; i++) {
            pthread_mutex_lock(&mutex);
            char *m = constructMessage(message, function, getTypeName(type), ADDITIONAL_FORMAT);
            fwrite(m, sizeof(char), strlen(m), additionalStreams[i]);
            pthread_mutex_unlock(&mutex);
        }
    }

    nbLog++;
}

char *constructMessage(const char *message, const char *trace, const char *logType, const char *format) {
    char *res = malloc(sizeof(char) * MAX_MESSAGE);
    res[0] = '\0';

    int i = 0;
    while (i < strlen(format)) {
        char c = format[i];
        if (c == '%') {
            i++;
            c = format[i];

            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            struct tm *t = localtime(&now.tv_sec);
            char *temp = malloc(sizeof(char) * 10);
            switch (c) {
                case 'Y':
                    sprintf(temp, "%4d", t->tm_year + 1900);
                    strcat(res, temp);
                    break;
                case 'M':
                    sprintf(temp, "%02d", t->tm_mon + 1);
                    strcat(res, temp);
                    break;
                case 'D':
                    sprintf(temp, "%02d", t->tm_mday);
                    strcat(res, temp);
                    break;
                case 'H':
                    sprintf(temp, "%02d", t->tm_hour);
                    strcat(res, temp);
                    break;
                case 'm':
                    sprintf(temp, "%02d", t->tm_min);
                    strcat(res, temp);
                    break;
                case 'S':
                    sprintf(temp, "%02d", t->tm_sec);
                    strcat(res, temp);
                    break;
                case 'N':
                    sprintf(temp, "%03ld", now.tv_nsec);
                    strcat(res, temp);
                    break;
                case 'd':
                    strcat(res, getDate());
                    break;
                case 'h':
                    strcat(res, getHour());
                    break;
                case 'T':
                    strcat(res, trace);
                    break;
                case 'C':
                    strcat(res, message);
                    break;
                case 'n':
                    sprintf(temp, "%d", nbLog);
                    strcat(res, temp);
                    break;
                case 't':
                    strcat(res, logType);
                    break;
                default:
                    break;
            }
        } else {
            char *temp = malloc(sizeof(char) * 2);
            temp[1] = '\0';
            temp[0] = c;
            strcat(res, temp);
        }
        i++;
    }

    return res;
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

void writeToFile(char *message) {
    if (fileOpen) {
        fwrite(message, sizeof(char), strlen(message), file);
        fflush(file);
    } else if (!isInitialized)
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