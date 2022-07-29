import os.path
import re
import shutil
import sys
from abc import abstractmethod
from threading import Thread, Semaphore
from time import sleep
from pathlib import Path

path_root = Path(__file__).parents[2]
sys.path.append(str(path_root))
print(sys.path)

from logger import Logger


# _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
# Utils

def clearDir(dirName):
    shutil.rmtree(dirName)


oldStdout = sys.stdout


def redirectOutput():
    sys.stdout = None


def restoreOutput():
    sys.stdout = oldStdout


# _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
# Tests

class Test:
    def __init__(self, name):
        self.name = name

    @abstractmethod
    def run(self):
        pass

    def before(self):
        pass

    def after(self):
        pass


class BasicTest1(Test):
    """Test basique 1 :
       Initialise le logger, utilise tout les niveaux de logs et exit le logger.

       Conditions de réussite :
       - Le dossier logs est créé.
       - Il contient un seul fichier .log.
       - Le fichier .log contient 8 lignes de logs."""

    def __init__(self):
        super().__init__('BasicTest1')

    def run(self):
        Logger.init()

        Logger.info("Info message")
        Logger.success("Success message")
        Logger.error("Error message")
        Logger.warning("Warning message")
        Logger.debug("Debug message")

        Logger.exit()

        # ====================

        # Le dossier logs est créé.
        if not os.path.exists('logs') or not os.path.isdir('logs'):
            return False

        # Il contient un seul fichier .log.
        if len(os.listdir('logs')) != 1:
            return False

        # Le fichier .log contient 8 lignes de logs.
        with open('logs/' + os.listdir('logs')[0], 'r') as f:
            lines = f.readlines()
            if len(lines) != 8:
                return False

        return True

    def after(self):
        clearDir('./logs')


class ThreadTest1(Test):
    """Test thread 1 :
       Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
       Le thread 1 log en info puis attend 1 seconde
       Le thread 2 attend 1 seconde puis log en debug

       Conditions de réussite :
       - Le dossier logs est créé.
       - Il contient un seul fichier .log.
       - Le fichier .log contient 5 lignes de logs.
       - La 3ème ligne du fichier .log contient le message "test" en info.
       - La 4ème ligne du fichier .log contient le message "test" en debug."""

    def __init__(self):
        super().__init__('ThreadTest1')

    class Thread1(Thread):
        def run(self):
            Logger.info("test")
            sleep(1)

    class Thread2(Thread):
        def run(self):
            sleep(1)
            Logger.debug("test")

    def run(self):
        Logger.init()

        t1 = ThreadTest1.Thread1()
        t2 = ThreadTest1.Thread2()

        t1.start()
        t2.start()

        t1.join()
        t2.join()

        Logger.exit()

        # ====================

        # Le dossier logs est créé.
        if not os.path.exists('logs') or not os.path.isdir('logs'):
            return False

        # Il contient un seul fichier .log.
        if len(os.listdir('logs')) != 1:
            return False

        # Le fichier .log contient 5 lignes de logs.
        with open('logs/' + os.listdir('logs')[0], 'r') as f:
            lines = f.readlines()
            if len(lines) != 5:
                return False

        # La 3ème ligne du fichier .log contient le message "test" en info.
        if not lines[2].__contains__("test") and not lines[2].__contains__("INFO"):
            return False

        # La 4ème ligne du fichier .log contient le message "test" en debug.
        if not lines[3].__contains__("test") and not lines[3].__contains__("DEBUG"):
            return False

        return True

    def after(self):
        clearDir('./logs')


class ThreadTest2(Test):
    """Test thread 2 :
       Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
       Le thread 1 log en info
       Le thread 2 log en debug
       Les 2 threads attendent que le sémaphore soit libéré avant de log en info et debug

       Conditions de réussite :
       - Le dossier logs est créé.
       - Il contient un seul fichier .log.
       - Le fichier .log contient 5 lignes de logs.
       - Les 3ème et 4ème lignes du fichier sont bien formées."""

    sem = Semaphore(0)

    def __init__(self):
        super().__init__('ThreadTest2')

    class Thread1(Thread):
        def run(self):
            ThreadTest2.sem.acquire()
            Logger.info("test")

    class Thread2(Thread):
        def run(self):
            ThreadTest2.sem.acquire()
            Logger.debug("test")

    def run(self):
        Logger.init()

        t1 = ThreadTest2.Thread1()
        t2 = ThreadTest2.Thread2()

        t1.start()
        t2.start()

        ThreadTest2.sem.release()
        ThreadTest2.sem.release()

        t1.join()
        t2.join()

        Logger.exit()

        # ====================

        restoreOutput()
        # Le dossier logs est créé.
        if not os.path.exists('logs') or not os.path.isdir('logs'):
            return False

        # Il contient un seul fichier .log.
        if len(os.listdir('logs')) != 1:
            return False

        # Le fichier .log contient 5 lignes de logs.
        with open('logs/' + os.listdir('logs')[0], 'r') as f:
            lines = f.readlines()
            if len(lines) != 5:
                return False

        # Les 3ème et 4ème lignes du fichier sont bien formées.
        regex1 = "(.*)-INFO\\]\t\\[run]\ttest "
        regex2 = "(.*)-DEBUG\\]\t\\[run]\ttest "
        line1 = lines[2]
        line2 = lines[3]
        if not re.match(regex1, line1) and not re.match(regex1, line2):
            return False
        if not re.match(regex2, line1) and not re.match(regex2, line2):
            return False

        return True

    def after(self):
        clearDir('./logs')


# _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.
# Main script

tests = list()
tests.append(BasicTest1())
tests.append(ThreadTest1())
tests.append(ThreadTest2())

if __name__ == '__main__':
    print(f'Running {len(tests)} tests')
    passed = 0
    for test in tests:
        toPrint = f'Running test {test.name}'
        print(toPrint, end='')

        redirectOutput()

        test.before()
        result = test.run()
        test.after()

        restoreOutput()

        # Erase last print
        print('\b' * len(toPrint), end='')

        if result:
            print('\u001B[30;42m PASS \u001B[00m ' + test.name)
            passed += 1
        else:
            print('\u001B[30;41m FAIL \u001B[00m ' + test.name)

    # Print summary
    print(f'\n{passed} tests passed out of {len(tests)}.')
    if passed == len(tests):
        print('\u001B[30;42m ALL TESTS PASSED \u001B[00m')
    else:
        print('\u001B[30;41m SOME TESTS FAILED \u001B[00m')

    # Return exit code
    if passed == len(tests):
        sys.exit(0)
    else:
        sys.exit(1)
