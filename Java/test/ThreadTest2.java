package test;

import logger.Logger;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.concurrent.Semaphore;

/**
 * Test thread 2 :
 * Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
 * Le thread 1 log en info
 * Le thread 2 log en debug
 * Les 2 threads attendent qu'une variable soit à false pour logger
 * <p>
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 5 lignes de logs.
 * - Les 3ème et 4ème lignes du fichier sont bien formées.
 */
public class ThreadTest2 extends Test {
    public ThreadTest2() {
        name = "ThreadTest2";
    }

    public Semaphore semaphore = new Semaphore(0);

    @Override
    public boolean run() {
        Logger.init();

        Thread t1 = new Thread(() -> {
            try {
                semaphore.acquire();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            Logger.info("test");
        });
        Thread t2 = new Thread(() -> {
            try {
                semaphore.acquire();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            Logger.debug("test");
        });
        t1.start();
        t2.start();

        semaphore.release(2);

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

        // Les 3ème et 4ème lignes du fichier sont bien formées.
        try {
            FileReader file = new FileReader(files[0]);
            BufferedReader reader = new BufferedReader(file);
            reader.readLine();
            reader.readLine();
            String line1 = reader.readLine();
            String line2 = reader.readLine();
            reader.close();
            file.close();
            String regex1 = "(.*)-INFO\\]\t\\[test\\.ThreadTest2\\.lambda\\$run\\$0\\]\ttest ";
            String regex2 = "(.*)-DEBUG\\]\t\\[test\\.ThreadTest2\\.lambda\\$run\\$1\\]\ttest ";
            if (!line1.matches(regex1) && !line2.matches(regex1)) {
                return false;
            }
            if (!line1.matches(regex2) && !line2.matches(regex2)) {
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
