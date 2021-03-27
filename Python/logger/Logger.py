import os
from _datetime import datetime
from enum import Enum

"""Logger
Use init() to start the logger and exit() to close the logger

Simple usage :
debug("Is it simple ? YES")
Write 'Is it simple ? YES' (without the quote) in the console and the file

Complex usage :
info("Not too complex ?", "Maybe", LoggerOption.CONSOLE_ONLY)
Write 'Not toot complex ? Maybe' (without the quote) only in the console"""

logPath = "./logs"
"""The log directory's path
Change it with your path"""

projectName = "project"
"""The log directory's path
Change it with your path"""


class LoggerColor(Enum):
    """Log colors
    Use DEFAULT for reset color"""
    DEFAULT = "\u001B[00m",

    BLACK = "\u001B[30m",
    RED = "\u001B[31m",
    GREEN = "\u001B[32m",
    YELLOW = "\u001B[33m",
    BLUE = "\u001B[34m",
    PURPLE = "\u001B[35m",
    CYAN = "\u001B[36m",
    WHITE = "\u001B[37m",

    BACKGROUND_BLACK = "\u001B[40m",
    BACKGROUND_RED = "\u001B[41m",
    BACKGROUND_GREEN = "\u001B[42m",
    BACKGROUND_YELLOW = "\u001B[43m",
    BACKGROUND_BLUE = "\u001B[44m",
    BACKGROUND_PURPLE = "\u001B[45m",
    BACKGROUND_CYAN = "\u001B[46m",
    BACKGROUND_WHITE = "\x1B[47m"


class LoggerOption(Enum):
    """Log options"""
    FILE_ONLY = 0,
    CONSOLE_ONLY = 1,
    FILE_AND_CONSOLE = 2


class LoggerType(Enum):
    """Log type"""
    INFO = ("INFO", LoggerColor.BLUE),
    SUCCESS = ("SUCCESS", LoggerColor.GREEN),
    ERROR = ("ERROR", LoggerColor.RED),
    WARNING = ("WARNING", LoggerColor.YELLOW),
    DEBUG = ("DEBUG", LoggerColor.PURPLE)


class Logger:
    """The class used for logs"""

    __file = None
    """The file where we write logs"""
    __nbWrite = 0
    """The number of log"""

    @classmethod
    def init(cls):
        """Initialisation"""
        if cls.__file is None:
            dir_created = False

            if not os.path.exists(logPath):
                os.mkdir(logPath)
                dir_created = True
            os.chdir(logPath)

            cls.__file = open(projectName + "_log_" + cls.__getDate() + ".log", "w")
            cls.info("Log start", LoggerOption.FILE_ONLY)

            if dir_created:
                cls.warning("Log directory created")
        else:
            cls.warning("Log already init")

    @classmethod
    def exit(cls):
        """Quit the log and close the file"""
        if cls.__file is not None:
            cls.info("End log", LoggerOption.FILE_ONLY)
            cls.__file.close()
            cls.__file = None
        else:
            cls.error("Please init before exit", LoggerOption.CONSOLE_ONLY)

    @classmethod
    def __genericLog(cls, args, log_type):
        """Generic log use for all logs"""
        messages = []
        options = []

        for arg in args:
            if type(arg) == LoggerOption:
                options.append(arg)
            else:
                messages.append(arg)

        message = ""
        for m in messages:
            message += str(m) + " "

        if LoggerOption.FILE_ONLY not in options:
            if len(log_type.value) == 1:
                name, color = log_type.value[0]
            else:
                name, color = log_type.value
            print(color.value[0] + message + LoggerColor.DEFAULT.value[0])

        if LoggerOption.CONSOLE_ONLY not in options:
            cls.__write_to_file(message, log_type)

    @classmethod
    def info(cls, *args):
        """Info"""
        cls.__genericLog(args, LoggerType.INFO)

    @classmethod
    def success(cls, *args):
        """Success"""
        cls.__genericLog(args, LoggerType.SUCCESS)

    @classmethod
    def error(cls, *args):
        """Error"""
        cls.__genericLog(args, LoggerType.ERROR)

    @classmethod
    def warning(cls, *args):
        """Warning"""
        cls.__genericLog(args, LoggerType.WARNING)

    @classmethod
    def debug(cls, *args):
        """Debug"""
        cls.__genericLog(args, LoggerType.DEBUG)

    @classmethod
    def __write_to_file(cls, message, log_type):
        """Write the into the file"""
        if cls.__file is not None:
            cls.__file.write("[" + str(cls.__nbWrite) + "-" + cls.__getHour() + "-" + log_type.name + "] " + message)
            cls.__nbWrite += 1
            cls.__file.flush()
        else:
            cls.error("Please init logger", LoggerOption.CONSOLE_ONLY)

    @classmethod
    def __getHour(cls):
        """The log's hour
        hh:mm:ss:mmm"""
        date = datetime.now()

        return str(date.hour) + ":" + str(date.minute) + ":" + str(date.second) + ":" + str(date.microsecond)

    @classmethod
    def __getDate(cls):
        """The log's date
        yyyy-mm-dd@hh-mm-ss"""
        date = datetime.now()

        return str(date.year) + "-" + str(date.month) + "-" + str(date.day) + "@" + str(date.hour) + "-" + str(
            date.minute) + "-" + str(date.second)
