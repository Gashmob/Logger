package test;

import logger.Logger;

import java.io.*;

/**
 * Test basique 1 :
 * Initialise le logger, utilise tout les niveaux de logs et exit le logger.
 * <p>
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 8 lignes de logs.
 */
public class BasicTest1 extends Test {
    public BasicTest1() {
        name = "BasicTest1";
    }

    @Override
    public boolean run() {

        Logger.init();

        Logger.info("Info message");
        Logger.success("Success message");
        Logger.error("Error message");
        Logger.warning("Warning message");
        Logger.debug("Debug message");

        Logger.exit();

        // ====================

        // Le dossier logs est créé.
        File logsDir = new File("logs");
        if (!logsDir.exists() || !logsDir.isDirectory()) {
            return false;
        }

        // Il contient un seul fichier .log.
        File[] logs = logsDir.listFiles();
        if (logs == null || logs.length != 1) {
            return false;
        }

        // Le fichier .log contient 8 lignes de logs.
        try {
            FileReader file = new FileReader(logs[0]);
            BufferedReader reader = new BufferedReader(file);
            int lines = 0;
            while (reader.readLine() != null) {
                lines++;
            }
            reader.close();
            file.close();
            if (lines != 8) {
                return false;
            }
        } catch (IOException e) {
            return false;
        }

        return true;
    }

    @Override
    public void after() {
        // Rm logs directory
        File logsDir = new File("logs");
        Utils.clearDir(logsDir);
    }
}
