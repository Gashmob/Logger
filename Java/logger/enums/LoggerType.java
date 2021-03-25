package logger.enums;

import static logger.enums.LoggerColor.*;

/**
 * Log types
 */
public enum LoggerType {
    INFO("INFO", BLUE),
    SUCCESS("SUCCESS", GREEN),
    ERROR("ERROR", RED),
    WARNING("WARNING", YELLOW),
    DEBUG("DEBUG", PURPLE),
    ;

    private final String name;
    private final LoggerColor color;

    LoggerType(String name, LoggerColor color) {
        this.name = name;
        this.color = color;
    }

    @Override
    public String toString() {
        return name.toUpperCase();
    }

    public LoggerColor getColor() {
        return color;
    }
}
