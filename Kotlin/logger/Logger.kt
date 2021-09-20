package logger

import logger.enums.LoggerColor
import logger.enums.LoggerOption
import logger.enums.LoggerOption.*
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

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/*
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
 */

/**
 * Format for console log
 */
internal const val console_format = "[%T]\t%C"

/**
 * Format for log file
 */
internal const val file_format = "[%n-%h-%t]\t[%T]\t%C"

/**
 * Format for additional output streams
 */
internal const val additional_format = "[%n-%t]\t[%T]\t%C"

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * The writer for the file
 */
internal var printWriter: PrintWriter? = null

/**
 * Additional output for the logs
 */
internal var additionalStreams = ArrayList<OutputStream>()

/**
 * The number of log
 */
internal var nbLog = 0

/**
 * The type of verbose
 */
internal var verbose = FILE_AND_CONSOLE

/**
 * The types of logs that be shown
 */
internal var showTypes = ArrayList<LoggerType>()

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * Initialisation
 */
fun init(
    verboseP: LoggerOption = FILE_AND_CONSOLE,
    showTypesP: ArrayList<LoggerType> = arrayListOf(INFO, SUCCESS, ERROR, WARNING, DEBUG)
) {
    if (printWriter == null) {
        verbose = verboseP
        showTypes = showTypesP

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

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * Add a new output for the logs
 */
fun addOutputStream(os: OutputStream) {
    additionalStreams.add(os)
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

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

    val traces = Thread.currentThread().stackTrace
    var trace = ""
    if (traces.size > 3) {
        val ste = traces[3]
        trace = ste.className + "." + ste.methodName
    }

    if (!options.contains(FILE_ONLY) && verbose != FILE_ONLY && showTypes.contains(type))
        println(
            type.color.toString() + constructMessage(
                message.toString(),
                trace,
                type.toString(),
                console_format
            ) + LoggerColor.DEFAULT
        )

    if (!options.contains(CONSOLE_ONLY) && verbose != CONSOLE_ONLY)
        writeToFile(constructMessage(message.toString(), trace, type.toString(), file_format))

    if (!options.contains(FILE_ONLY) && !options.contains(CONSOLE_ONLY) && verbose == FILE_AND_CONSOLE) {
        additionalStreams.forEach {
            try {
                it.write((constructMessage(message.toString(), trace, type.toString(), additional_format) + "\n").toByteArray())
                it.flush()
            } catch (e: IOException) {
                error(e, CONSOLE_ONLY)
            }
        }
    }

    nbLog++
}

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
 */
internal fun constructMessage(message: String, trace: String, logType: String, format: String): String {
    val res = StringBuilder()

    var i = 0
    while (i < format.length) {
        var c = format[i]
        if (c == '%') {
            i++
            c = format[i]

            val now = LocalDateTime.now()
            when (c) {
                'Y' -> res.append(now.year)
                'M' -> res.append(String.format("%02d", now.monthValue))
                'D' -> res.append(String.format("%02d", now.dayOfMonth))
                'H' -> res.append(String.format("%02d", now.hour))
                'm' -> res.append(String.format("%02d", now.minute))
                'S' -> res.append(String.format("%02d", now.second))
                'N' -> res.append(String.format("%03d", now.nano / 1000))
                'd' -> res.append(getDate())
                'h' -> res.append(getHour())
                'T' -> res.append(trace)
                'C' -> res.append(message)
                'n' -> res.append(nbLog)
                't' -> res.append(logType)
            }
        } else {
            res.append(c)
        }

        i++
    }

    return res.toString()
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

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * Write the log into the file
 */
@Synchronized
internal fun writeToFile(message: String) {
    if (printWriter != null) {
        printWriter!!.println(message)
        printWriter!!.flush()
    } else
        error("Please init Logger", CONSOLE_ONLY)
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

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