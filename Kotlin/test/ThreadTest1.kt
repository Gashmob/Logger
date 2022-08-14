import java.io.BufferedReader
import java.io.File
import java.io.FileReader
import java.io.IOException

/**
 * Test thread 1 :
 * Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
 * Le thread 1 log en info puis attend 1 seconde
 * Le thread 2 attend 1 seconde puis log en debug
 * <p>
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 5 lignes de logs.
 * - La 3ème ligne du fichier .log contient le message "test" en info.
 * - La 4ème ligne du fichier .log contient le message "test" en debug.
 */
class ThreadTest1 : Test("ThreadTest1") {
    override fun run(): Boolean {
        logger.init()

        val t1 = Thread {
            logger.info("test")
            try {
                Thread.sleep(1000)
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
        }
        val t2 = Thread {
            try {
                Thread.sleep(1000)
            } catch (e: InterruptedException) {
                e.printStackTrace()
            }
            logger.debug("test")
        }
        t1.start()
        t2.start()

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

        // La 3ème ligne du fichier .log contient le message "test" en info.
        try {
            val file = FileReader(files[0])
            val reader = BufferedReader(file)
            reader.readLine()
            reader.readLine()
            val line = reader.readLine()
            reader.close()
            file.close()
            if (!line.contains("test") && !line.contains("[INFO]")) {
                return false
            }
        } catch (e: IOException) {
            return false
        }

        // La 4ème ligne du fichier .log contient le message "test" en debug.
        try {
            val file = FileReader(files[0])
            val reader = BufferedReader(file)
            reader.readLine()
            reader.readLine()
            reader.readLine()
            val line = reader.readLine()
            reader.close()
            file.close()
            if (!line.contains("test") && !line.contains("[DEBUG]")) {
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