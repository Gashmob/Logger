package logger;

import logger.enums.LoggerColor;
import logger.enums.LoggerOption;
import logger.enums.LoggerType;

import java.io.*;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Arrays;

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
    private final static String console_format = "[%T]\t%C";
    /**
     * Format for log file
     */
    private final static String file_format = "[%n-%h-%t]\t[%T]\t%C";
    /**
     * Format for additional output streams
     */
    private final static String additional_format = "[%n-%t]\t[%T]\t%C";

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

    /**
     * The writer for the file
     */
    private static PrintWriter printWriter = null;
    /**
     * Additional output for the logs
     */
    private static ArrayList<OutputStream> additionalStreams;
    /**
     * The number of log
     */
    private static int nbLog = 0;
    /**
     * The type of verbose
     */
    private static LoggerOption verbose;
    /**
     * The types of logs that be shown
     */
    private static ArrayList<LoggerType> showTypes;

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

    public static void init() {
        init(FILE_AND_CONSOLE, new LoggerType[]{INFO, SUCCESS, ERROR, WARNING, DEBUG});
    }

    public static void init(LoggerOption verbose) {
        init(verbose, new LoggerType[]{INFO, SUCCESS, ERROR, WARNING, DEBUG});
    }

    public static void init(LoggerType[] showTypes) {
        init(FILE_AND_CONSOLE, showTypes);
    }

    /**
     * Initialisation
     *
     * @param verbose   LoggerOption
     * @param showTypes LoggerType
     */
    public static void init(LoggerOption verbose, LoggerType[] showTypes) {
        if (printWriter == null) {
            Logger.verbose = verbose;
            Logger.showTypes = new ArrayList<>(Arrays.asList(showTypes));
            additionalStreams = new ArrayList<>();

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

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

    /**
     * Add a new output for the logs
     *
     * @param os OutputStream
     */
    public static void addOutputStream(OutputStream os) {
        additionalStreams.add(os);
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

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

        for (String s : messages) {
            message.append(s);
            message.append(separator);
        }

        StackTraceElement[] traces = Thread.currentThread().getStackTrace();
        String trace = "";
        if (traces.length > 3) {
            StackTraceElement ste = traces[3];
            trace = ste.getClassName() + "." + ste.getMethodName();
        }

        if (!options.contains(FILE_ONLY) && verbose != FILE_ONLY && showTypes.contains(type)) {
            System.out.println(type.getColor() + constructMessage(message.toString(), trace, type.toString(), console_format) + LoggerColor.DEFAULT);
        }

        if (!options.contains(CONSOLE_ONLY) && verbose != CONSOLE_ONLY) {
            writeToFile(constructMessage(message.toString(), trace, type.toString(), file_format), type);
        }

        if (!options.contains(FILE_ONLY) && !options.contains(CONSOLE_ONLY) && verbose == FILE_AND_CONSOLE) {
            for (OutputStream additionalStream : additionalStreams) {
                try {
                    additionalStream.write(constructMessage(message.toString(), trace, type.toString(), additional_format).getBytes());
                    additionalStream.flush();
                } catch (IOException e) {
                    error(e, CONSOLE_ONLY);
                }
            }
        }

        nbLog++;
    }

    /**
     * Construct the message from the format
     * <p>
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
     * @param message String
     * @param trace   String
     * @param logType String
     * @param format  String
     * @return String
     */
    private static String constructMessage(String message, String trace, String logType, String format) {
        StringBuilder res = new StringBuilder();

        int i = 0;
        while (i < format.length()) {
            char c = format.charAt(i);
            if (c == '%') {
                i++;
                c = format.charAt(i);

                LocalDateTime now = LocalDateTime.now();
                switch (c) {
                    case 'Y' -> res.append(now.getYear());
                    case 'M' -> res.append(String.format("%02d", now.getMonthValue()));
                    case 'D' -> res.append(String.format("%02d", now.getDayOfMonth()));
                    case 'H' -> res.append(String.format("%02d", now.getHour()));
                    case 'm' -> res.append(String.format("%02d", now.getMinute()));
                    case 'S' -> res.append(String.format("%02d", now.getSecond()));
                    case 'N' -> res.append(String.format("%03d", now.getNano() / 1000));
                    case 'd' -> res.append(getDate());
                    case 'h' -> res.append(getHour());
                    case 'T' -> res.append(trace);
                    case 'C' -> res.append(message);
                    case 'n' -> res.append(nbLog);
                    case 't' -> res.append(logType);
                }
            } else {
                res.append(c);
            }

            i++;
        }

        return res.toString();
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

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

    /**
     * Write the log into the file
     *
     * @param message String
     * @param type    LoggerType
     */
    private synchronized static void writeToFile(String message, LoggerType type) {
        if (printWriter != null) {
            String toPrint = "["
                    + nbLog + "-"
                    + getHour() + "-"
                    + type.toString() + "]\t"
                    + message;

            printWriter.println(toPrint);
            printWriter.flush();
        } else {
            error("Please init Logger", CONSOLE_ONLY);
        }
    }

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

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

    // _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

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
