package test;

import logger.Logger;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

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
public class ThreadTest1 extends Test {
    public ThreadTest1() {
        name = "ThreadTest1";
    }

    @Override
    public boolean run() {
        Logger.init();

        Thread t1 = new Thread(() -> {
            Logger.info("test");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });
        Thread t2 = new Thread(() -> {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Logger.debug("test");
        });
        t1.start();
        t2.start();

        try {
            t1.join();
            t2.join();
        } catch (InterruptedException e) {
            return false;
        }

        Logger.exit();

        // ====================

        // Le dossier logs est créé
        File logs = new File("logs");
        if (!logs.exists() || !logs.isDirectory()) {
            return false;
        }

        // Il contient un seul fichier .log.
        File[] files = logs.listFiles();
        if (files == null || files.length != 1) {
            return false;
        }

        // Le fichier .log contient 5 lignes de logs.
        try {
            FileReader file = new FileReader(files[0]);
            BufferedReader reader = new BufferedReader(file);
            int lines = 0;
            while (reader.readLine() != null) {
                lines++;
            }
            reader.close();
            file.close();
            if (lines != 5) {
                return false;
            }
        } catch (IOException e) {
            return false;
        }

        // La 3ème ligne du fichier .log contient le message "test" en info.
        try {
            FileReader file = new FileReader(files[0]);
            BufferedReader reader = new BufferedReader(file);
            reader.readLine();reader.readLine();
            String line = reader.readLine();
            reader.close();
            file.close();
            if (!line.contains("test") && !line.contains("[INFO]")) {
                return false;
            }
        } catch (IOException e) {
            return false;
        }

        // La 4ème ligne du fichier .log contient le message "test" en debug.
        try {
            FileReader file = new FileReader(files[0]);
            BufferedReader reader = new BufferedReader(file);
            reader.readLine();reader.readLine();reader.readLine();
            String line = reader.readLine();
            reader.close();
            file.close();
            if (!line.contains("test") && !line.contains("[DEBUG]")) {
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
