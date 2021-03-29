package logger

import logger.enums.LoggerColor
import logger.enums.LoggerOption
import logger.enums.LoggerOption.CONSOLE_ONLY
import logger.enums.LoggerOption.FILE_ONLY
import logger.enums.LoggerType
import logger.enums.LoggerType.*
import java.io.*
import java.time.LocalDateTime

/**
 * Logger
 * <p>
 * Use init() to start the logger and exit() to close the logger
 * <p>
 * Simple usage :
 * debug("Is it simple ? YES")
 * Write 'Is it simple ? YES' (without the quote) in the console and the file
 * <p>
 * Complex usage :
 * info("Not too complex ?", "Maybe", LoggerOption.CONSOLE_ONLY)
 * Write 'Not toot complex ? Maybe' (without the quote) only in the console
 */

/**
 * The log directory's path
 * Change it with your path
 */
internal const val logPath = "./logs"

/**
 * Project's name
 * Change it with your project's name
 */
internal const val projectName = "project"

/**
 * The writer for the file
 */
internal var printWriter: PrintWriter? = null

/**
 * The number of log
 */
internal var nbWrite = 0

/**
 * Initialisation
 */
fun init() {
    if (printWriter == null) {
        var dirCreated = false
        var bufferedWriter: BufferedWriter? = null

        val ok = try {
            dirCreated = File(logPath).mkdir()
            val fileWriter = FileWriter(logPath + "/" + projectName + "_log_" + getDate() + ".log")
            bufferedWriter = BufferedWriter(fileWriter)

            true
        } catch (e: IOException) {
            error(e, CONSOLE_ONLY)
            false
        }

        if (ok) {
            printWriter = PrintWriter(bufferedWriter)

            info("Log start", FILE_ONLY)

            if (dirCreated)
                warning("Log directory created")
        } else
            error("Log error")
    } else
        warning("Log already init")
}

/**
 * Quit the log and close the file
 */
fun exit() {
    if (printWriter != null) {
        info("End log", FILE_ONLY)
        printWriter!!.close()
        printWriter = null
    } else
        error("Please init before exit", CONSOLE_ONLY)
}

/**
 * Generic log use for all logs
 */
internal fun genericLog(args: Array<out Any>, type: LoggerType) {
    val messages = ArrayList<String>()
    val options = ArrayList<LoggerOption>()

    for (arg in args) {
        if (arg is LoggerOption)
            options.add(arg)
        else
            messages.add(arg.toString())
    }

    val separator = " "

    val message = StringBuilder()

    for (i in 0 until messages.size) {
        message.append(messages[i])

        if (i < messages.size)
            message.append(separator)
    }

    if (!options.contains(FILE_ONLY))
        println(type.color.toString() + message.toString() + LoggerColor.DEFAULT)

    if (!options.contains(CONSOLE_ONLY))
        writeToFile(message.toString(), type)
}

/**
 * Info
 */
fun info(vararg args: Any) = genericLog(args, INFO)

/**
 * Success
 */
fun success(vararg args: Any) = genericLog(args, SUCCESS)

/**
 * Error
 */
fun error(vararg args: Any) = genericLog(args, ERROR)

/**
 * Warning
 */
fun warning(vararg args: Any) = genericLog(args, WARNING)

/**
 * Debug
 */
fun debug(vararg args: Any) = genericLog(args, DEBUG)

/**
 * Write the log into the file
 */
@Synchronized internal fun writeToFile(message: String, type: LoggerType) {
    if (printWriter != null) {
        val toPrint = ("["
                + nbWrite + "-"
                + getHour() + "-"
                + type.toString() + "] "
                + message)

        printWriter!!.println(toPrint)
        nbWrite++
        printWriter!!.flush()
    } else
        error("Please init Logger", CONSOLE_ONLY)
}

/**
 * The log's hour
 * hh:mm:ss:nnn
 */
internal fun getDate(): String {
    val now = LocalDateTime.now()

    return (now.year.toString()
            + "-" + String.format("%02d", now.monthValue)
            + "-" + String.format("%02d", now.dayOfMonth)
            + "@" + String.format("%02d", now.hour)
            + "-" + String.format("%02d", now.minute)
            + "-" + String.format("%03d", now.second))
}

/**
 * The log's date
 * yyyy-mm-dd@hh-mm-ss
 */
internal fun getHour(): String {
    val now = LocalDateTime.now()

    return (String.format("%02d", now.hour)
            + ":" + String.format("%02d", now.minute)
            + ":" + String.format("%02d", now.second)
            + ":" + String.format("%02d", now.nano / 1000))
}