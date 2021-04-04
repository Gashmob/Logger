#include "Logger.hpp"

std::ofstream Logger::file("");

int Logger::nbWrite = 0;

pthread_mutex_t Logger::mutex;


std::string Logger::getColor(LoggerColor color) {
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

std::string Logger::getTypeColor(LoggerType type) {
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

std::string Logger::getTypeName(LoggerType type) {
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

void Logger::init() {
    if (!file.is_open()) {
        bool dirCreated = false;

        if (mkdir(LOG_PATH, S_IRWXU) == 0)
            dirCreated = true;

        std::string fileName = (LOG_PATH "/" PROJECT_NAME "_log_") + getDate() + ".log";
        file.open(fileName, std::ios::out);

        nbWrite = 0;

        if (pthread_mutex_init(&mutex, nullptr) != 0)
            error(CONSOLE_ONLY, "Error mutex : %d\n", errno);

        info(FILE_ONLY, "Log start\n");
        if (dirCreated)
            warning(FILE_AND_CONSOLE, "Log directory created\n");
    } else
        warning(FILE_AND_CONSOLE, "Log already init\n");
}

void Logger::exit() {
    if (file.is_open()) {
        info(FILE_ONLY, "End log\n");
        file.close();

        pthread_mutex_destroy(&mutex);
    } else
        error(CONSOLE_ONLY, "Please init before exit\n");
}

void Logger::genericLog(const std::string &message, LoggerType type, LoggerOption option) {
    if (option != FILE_ONLY)
        std::cout << getTypeColor(type) << message << getColor(DEFAULT);

    if (option != CONSOLE_ONLY) {
        pthread_mutex_lock(&mutex);
        writeToFile(message, type);
        pthread_mutex_unlock(&mutex);
    }
}

void Logger::writeToFile(const std::string &message, LoggerType type) {
    if (file.is_open()) {
        std::string toPrint =
                "[" + std::to_string(nbWrite) + "-" + getHour() + "-" + getTypeName(type) + "] " + message;
        file << toPrint;
        file.flush();
        nbWrite++;
    } else
        error(CONSOLE_ONLY, "Please init logger\n");
}

std::string Logger::getHour() {
    struct timespec now{};
    clock_gettime(CLOCK_REALTIME, &now);
    struct tm *t = localtime(&now.tv_sec);

    return std::to_string(t->tm_hour) + ":"
           + std::to_string(t->tm_min) + ":"
           + std::to_string(t->tm_sec) + ":"
           + std::to_string(now.tv_nsec);
}

std::string Logger::getDate() {
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);

    return std::to_string(t->tm_year + 1900) + "-"
           + std::to_string(t->tm_mon + 1) + "-"
           + std::to_string(t->tm_mday) + "@"
           + std::to_string(t->tm_hour) + "-"
           + std::to_string(t->tm_min) + "-"
           + std::to_string(t->tm_sec);
}