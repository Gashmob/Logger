package logger

import (
	"container/list"
	"fmt"
	"io"
	"os"
	"runtime"
	"sync"
	"time"
)

/*
 * Logger
 *
 * Use logger.Init() to start the logger and logger.Exit() to close the logger
 *
 * Simple usage :
 * logger.Debug("Is it simple ? YES")
 * Write 'Is it simple ? YES' (without the quote) in the console and the file
 *
 * Complex usage :
 * logger.Info("Not too complex ?", "Maybe", logger.CONSOLE_ONLY)
 * Write 'Not too complex ? Maybe' (without the quote) only in the console
 */

/**
 * The log directory's path
 * Change it with your path
 */
const LogPath = "./logs"

/**
 * Project's name
 * Change it with your project's name
 */
const ProjectName = "project"

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
const ConsoleFormat = "[%T]\t%C"

/**
 * Format for log file
 */
const FileFormat = "[%n-%h-%t]\t[%T]\t%C"

/**
 * Format for additional output streams
 */
const AdditionalFormat = "[%n-%t]\t[%T]\t%C"

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

/**
 * If the logger is initialized
 */
var isInitialized = false

/**
 * The log file
 */
var file *os.File = nil

/**
 * Additional output for the logs
 */
var additionalOutputs = list.New()

/**
 * The number of log
 */
var nbLog = 0

/**
 * The type of verbose
 */
var verbose = FILE_AND_CONSOLE

/**
 * The types of log that be shown
 */
var showTypes *list.List = nil

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

func Init(verboseP LoggerOption, showTypesP []LoggerType) {
	if !isInitialized {
		verbose = verboseP
		showTypes = list.New()
		for i := 0; i < len(showTypesP); i++ {
			showTypes.PushBack(showTypesP[i])
		}

		var ok = true
		var dirCreated = false

		if verboseP != CONSOLE_ONLY {
			err := os.Mkdir(LogPath, os.ModePerm)
			if err != nil {
				if !os.IsExist(err) {
					ok = false
				}
			} else {
				dirCreated = true
			}

			file, err = os.Create(LogPath + "/" + ProjectName + "_log_" + getDate() + ".log")
			if err != nil {
				ok = false
			}

			if ok {
				isInitialized = true
				Info("Log start", FILE_ONLY)

				if dirCreated {
					Warning("Log directory created")
				}
			} else {
				Error("Log error", CONSOLE_ONLY)
			}
		}
	} else {
		Warning("Log already init")
	}
}

func Exit() {
	if isInitialized {
		Info("End log", FILE_ONLY)
		isInitialized = false

		if file != nil {
			err := file.Close()
			if err != nil {
				file = nil
				return
			}
		}
	} else {
		Error("Please init logger before exit", CONSOLE_ONLY)
	}
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

func AddOutputStream(stream io.Writer) {
	additionalOutputs.PushBack(stream)
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

func genericLog(args []interface{}, logType LoggerType) {
	var messages = list.New()
	var options = list.New()

	for i := 0; i < len(args); i++ {
		var arg = args[i]
		switch arg.(type) {
		case LoggerOption:
			options.PushBack(arg.(LoggerOption))
		default:
			messages.PushBack(arg)
		}
	}

	const separator = " "
	var message = ""
	for i := 0; i < messages.Len(); i++ {
		message += fmt.Sprintf("%v", messages.Remove(messages.Front()))
		if i < messages.Len()-1 {
			message += separator
		}
	}

	pc, _, line, _ := runtime.Caller(2)
	funcName := runtime.FuncForPC(pc).Name()
	var trace = fmt.Sprintf("%s:%d", funcName, line)

	if !contains(options, FILE_ONLY) && verbose != FILE_ONLY && contains(showTypes, logType) {
		fmt.Println(logType.color.String() + constructMessage(message, trace, logType, ConsoleFormat) + DEFAULT.String())
	}
	if !contains(options, CONSOLE_ONLY) && verbose != CONSOLE_ONLY {
		writeToFile(constructMessage(message, trace, logType, FileFormat))
	}
	if !contains(options, FILE_ONLY) && !contains(options, CONSOLE_ONLY) && verbose == FILE_AND_CONSOLE {
		for e := additionalOutputs.Front(); e != nil; e = e.Next() {
			var add = e.Value.(io.Writer)
			_, err := add.Write([]byte(constructMessage(message, trace, logType, AdditionalFormat) + "\n"))
			if err != nil {
				Error(e, CONSOLE_ONLY)
			}
		}
	}

	nbLog++
}

func constructMessage(message string, trace string, logType LoggerType, format string) string {
	var res = ""

	var i = 0
	for i < len(format) {
		var c = format[i]
		if c == '%' {
			i++
			c = format[i]

			dt := time.Now()
			switch c {
			case 'Y':
				res += fmt.Sprintf("%d", dt.Year())
			case 'M':
				res += fmt.Sprintf("%02d", dt.Month())
			case 'D':
				res += fmt.Sprintf("%02d", dt.Day())
			case 'H':
				res += fmt.Sprintf("%02d", dt.Hour())
			case 'm':
				res += fmt.Sprintf("%02d", dt.Minute())
			case 'S':
				res += fmt.Sprintf("%02d", dt.Second())
			case 'N':
				res += fmt.Sprintf("%02d", dt.Nanosecond())
			case 'd':
				res += getDate()
			case 'h':
				res += getHour()
			case 'T':
				res += trace
			case 'C':
				res += message
			case 'n':
				res += fmt.Sprintf("%d", nbLog)
			case 't':
				res += logType.String()
			}
		} else {
			res += string(c)
		}

		i++
	}

	return res
}

func Info(args ...interface{}) {
	genericLog(args, INFO)
}

func Success(args ...interface{}) {
	genericLog(args, SUCCESS)
}

func Error(args ...interface{}) {
	genericLog(args, ERROR)
}

func Warning(args ...interface{}) {
	genericLog(args, WARNING)
}

func Debug(args ...interface{}) {
	genericLog(args, DEBUG)
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

var mutex sync.Mutex = sync.Mutex{}

func writeToFile(message string) {
	if file != nil {
		mutex.Lock()
		defer mutex.Unlock()
		_, err := file.WriteString(message + "\n")
		if err != nil {
			return
		}
	} else if !isInitialized {
		Error("Please init logger", CONSOLE_ONLY)
	}
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

func getDate() string {
	dt := time.Now()

	return fmt.Sprintf("%d-%02d-%02d@%02d-%02d-%02d", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second())
}

func getHour() string {
	dt := time.Now()

	return fmt.Sprintf("%02d:%02d:%02d:%02d", dt.Hour(), dt.Minute(), dt.Second(), dt.Nanosecond())
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

func contains(l *list.List, v interface{}) bool {
	for e := l.Front(); e != nil; e = e.Next() {
		if e.Value == v {
			return true
		}
	}

	return false
}
