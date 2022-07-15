import java.io.BufferedReader
import java.io.File
import java.io.FileReader
import java.io.IOException

/**
 * Test basique 1 :
 * Initialise le logger, utilise tout les niveaux de logs et exit le logger.
 * <p>
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 8 lignes de logs.
 */
class BasicTest1 : Test("BasicTest1") {
    override fun run(): Boolean {
        logger.init()

        logger.info("Info message")
        logger.success("Success message")
        logger.error("Error message")
        logger.warning("Warning message")
        logger.debug("Debug message")

        logger.exit()

        // ====================

        // Le dossier logs est créé.
        val logsDir = File("logs")
        if (!logsDir.exists() || !logsDir.isDirectory) {
            return false
        }

        // Il contient un seul fichier .log.
        val logs = logsDir.listFiles()
        if (logs == null || logs.size != 1) {
            return false
        }

        // Le fichier .log contient 8 lignes de logs.
        try {
            val file = FileReader(logs[0])
            val reader = BufferedReader(file)
            var lines = 0
            while (reader.readLine() != null) {
                lines++
            }
            reader.close()
            file.close()
            if (lines != 8) {
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