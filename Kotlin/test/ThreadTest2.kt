import java.io.BufferedReader
import java.io.File
import java.io.FileReader
import java.io.IOException
import java.util.concurrent.Semaphore


class ThreadTest2 : Test("ThreadTest2") {
    private var semaphore = Semaphore(0)

    override fun run(): Boolean {
        logger.init()

        val t1 = Thread {
            semaphore.acquire()
            logger.info("test")
        }
        val t2 = Thread {
            semaphore.acquire()
            logger.debug("test")
        }
        t1.start()
        t2.start()

        semaphore.release(2)

        try {
            t1.join()
            t2.join()
        } catch (e: InterruptedException) {
            return false
        }

        logger.exit()

        // ====================

        // Le dossier logs est créé
        val logs = File("logs")
        if (!logs.exists() || !logs.isDirectory) {
            return false
        }

        // Il contient un seul fichier .log.
        val files = logs.listFiles()
        if (files == null || files.size != 1) {
            return false
        }

        // Le fichier .log contient 5 lignes de logs.
        try {
            val file = FileReader(files[0])
            val reader = BufferedReader(file)
            var lines = 0
            while (reader.readLine() != null) {
                lines++
            }
            reader.close()
            file.close()
            if (lines != 5) {
                return false
            }
        } catch (e: IOException) {
            return false
        }

        // Les 3ème et 4ème lignes du fichier sont bien formées.
        try {
            val file = FileReader(files[0])
            val reader = BufferedReader(file)
            reader.readLine()
            reader.readLine()
            val line1 = reader.readLine()
            val line2 = reader.readLine()
            reader.close()
            file.close()
            val regex1 = Regex("(.*)-INFO\\]\t\\[ThreadTest2\\.run\\\$lambda-0\\]\ttest ")
            val regex2 = Regex("(.*)-DEBUG\\]\t\\[ThreadTest2\\.run\\\$lambda-1\\]\ttest ")
            if (!line1.matches(regex1) && !line2.matches(regex1)) {
                return false
            }
            if (!line1.matches(regex2) && !line2.matches(regex2)) {
                return false
            }
        } catch (e: IOException) {
            return false
        }

        return true
    }

    override fun after() {
        // Rm logs directory
        val logsDir = File("logs")
        clearDir(logsDir)
    }
}