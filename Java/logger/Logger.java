package logger;

import logger.enums.LoggerColor;
import logger.enums.LoggerOption;
import logger.enums.LoggerType;

import java.io.*;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.ArrayList;

import static logger.enums.LoggerOption.*;
import static logger.enums.LoggerType.*;

/**
 * Logger
 * <p>
 * Use init() to start the logger and exit() to close the logger
 * <p>
 * Simple usage :
 * Logger.debug("Is it simple ? YES");
 * Write 'Is it simple ? YES' (without the quote) in the console and the file
 * <p>
 * Complex usage :
 * Logger.info("Not too complex ?", "Maybe", LoggerOption.CONSOLE_ONLY);
 * Write 'Not toot complex ? Maybe' (without the quote) only in the console
 */
public abstract class Logger {
    /**
     * The log directory's path
     * Change it with your path
     */
    private final static String logPath = "./logs";
    /**
     * Project's name
     * Change it with your project's name
     */
    private final static String projectName = "project";

    /**
     * The writer for the file
     */
    private static PrintWriter printWriter = null;
    /**
     * The number of log
     */
    private static int nbWrite = 0;
    /**
     * The type of verbose
     */
    private static LoggerOption verbose;
    /**
     * Show trace or not
     */
    private static Boolean showTrace;

    public static void init() {
        init(FILE_AND_CONSOLE, true);
    }

    public static void init(LoggerOption verbose) {
        init(verbose, true);
    }

    public static void init(Boolean showTrace) {
        init(FILE_AND_CONSOLE, showTrace);
    }

    /**
     * Initialisation
     */
    public static void init(LoggerOption verbose, Boolean showTrace) {
        Logger.verbose = verbose;
        Logger.showTrace = showTrace;

        if (printWriter == null) {
            boolean ok;
            boolean dirCreated = false;
            BufferedWriter bufferedWriter = null;

            try {
                dirCreated = new File(logPath).mkdir();
                FileWriter fileWriter = new FileWriter(logPath + "/" + projectName + "_log_" + getDate() + ".log");
                bufferedWriter = new BufferedWriter(fileWriter);
                ok = true;
            } catch (IOException e) {
                error(e, CONSOLE_ONLY);
                ok = false;
            }

            if (ok) {
                printWriter = new PrintWriter(bufferedWriter);

                info("Log start", FILE_ONLY);

                if (dirCreated) {
                    warning("Log directory created");
                }
            } else {
                error("Log error");
            }
        } else {
            warning("Log already init");
        }
    }

    /**
     * Generic log use for all logs
     *
     * @param args Object[]
     * @param type LoggerType
     */
    private synchronized static void genericLog(Object[] args, LoggerType type) {
        ArrayList<String> messages = new ArrayList<>();
        ArrayList<LoggerOption> options = new ArrayList<>();

        for (Object arg : args) {
            if (arg instanceof LoggerOption) {
                options.add((LoggerOption) arg);
            } else {
                messages.add(String.valueOf(arg));
            }
        }

        final String separator = " ";

        StringBuilder message = new StringBuilder();

        for (int i = 0; i < messages.size(); i++) {
            message.append(messages.get(i));

            if (i < messages.size()) {
                message.append(separator);
            }
        }

        StackTraceElement[] traces = Thread.currentThread().getStackTrace();
        if (traces.length > 3 && showTrace) {
            StackTraceElement trace = traces[3];
            String t = "[" + trace.getClassName() + "." + trace.getMethodName() + "]\t";
            message.insert(0, t);
        }

        if (!options.contains(FILE_ONLY) && verbose != FILE_ONLY) {
            System.out.println(type.getColor() + message.toString() + LoggerColor.DEFAULT);
        }

        if (!options.contains(CONSOLE_ONLY) && verbose != CONSOLE_ONLY) {
            writeToFile(message.toString(), type);
        }
    }

    /**
     * Info
     *
     * @param args Object...
     */
    public static void info(Object... args) {
        genericLog(args, INFO);
    }

    /**
     * Success
     *
     * @param args Object...
     */
    public static void success(Object... args) {
        genericLog(args, SUCCESS);
    }

    /**
     * Error
     *
     * @param args Object...
     */
    public static void error(Object... args) {
        genericLog(args, ERROR);
    }

    /**
     * Warning
     *
     * @param args Object...
     */
    public static void warning(Object... args) {
        genericLog(args, WARNING);
    }

    /**
     * Debug
     *
     * @param args Object...
     */
    public static void debug(Object... args) {
        genericLog(args, DEBUG);
    }

    /**
     * Write the log into the file
     *
     * @param message String
     * @param type    LoggerType
     */
    private synchronized static void writeToFile(String message, LoggerType type) {
        if (printWriter != null) {
            String toPrint = "["
                    + nbWrite + "-"
                    + getHour() + "-"
                    + type.toString() + "]\t"
                    + message;

            printWriter.println(toPrint);
            nbWrite++;
            printWriter.flush();
        } else {
            error("Please init Logger", CONSOLE_ONLY);
        }
    }

    /**
     * The log's hour
     * hh:mm:ss:nnn
     *
     * @return String
     */
    private static String getHour() {
        LocalTime now = LocalTime.now();

        return String.format("%02d", now.getHour())
                + ":" + String.format("%02d", now.getMinute())
                + ":" + String.format("%02d", now.getSecond())
                + ":" + String.format("%03d", now.getNano() / 1000);
    }

    /**
     * The log's date
     * yyyy-mm-dd@hh-mm-ss
     *
     * @return String
     */
    private static String getDate() {
        LocalDateTime now = LocalDateTime.now();

        return now.getYear()
                + "-" + String.format("%02d", now.getMonthValue())
                + "-" + String.format("%02d", now.getDayOfMonth())
                + "@" + String.format("%02d", now.getHour())
                + "-" + String.format("%02d", now.getMinute())
                + "-" + String.format("%02d", now.getSecond());
    }

    /**
     * Quit the log and close the writer
     */
    public static void exit() {
        if (printWriter != null) {
            info("End log", FILE_ONLY);
            printWriter.close();
            printWriter = null;
        } else {
            error("Please init before exit", CONSOLE_ONLY);
        }
    }
}
