package logger.enums

import logger.enums.LoggerColor.*

/**
 * Log types
 */
enum class LoggerType(private val value: String, val color: LoggerColor) {
    INFO("INFO", BLUE),
    SUCCESS("SUCCESS", GREEN),
    ERROR("ERROR", RED),
    WARNING("WARNING", YELLOW),
    DEBUG("DEBUG", PURPLE);

    override fun toString() = value.toUpperCase()
}