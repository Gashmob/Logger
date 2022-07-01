package test;

import logger.Logger;
import org.junit.jupiter.api.BeforeAll;

import java.io.File;
import java.util.Objects;

import static org.junit.jupiter.api.Assertions.*;

class LoggerTest {

    @BeforeAll
    static void beforeAll() {
        // Rm logs dir
        File logsDir = new File("./logs");
        if (logsDir.exists()) {
            for (File file : Objects.requireNonNull(logsDir.listFiles())) {
                file.delete();
            }
            logsDir.delete();
        }
    }

    @org.junit.jupiter.api.Test
    void normalRun() {
        // Run
        Logger.init();
        Logger.info("Hello World");
        Logger.exit();

        // Test
        File dir = new File("./logs");
        assertTrue(dir.isDirectory());
        assertNotNull(dir.listFiles());
        assertEquals(1, Objects.requireNonNull(dir.listFiles()).length);
        File f = Objects.requireNonNull(dir.listFiles())[0];
        assertEquals("log", f.getName().split("\\.")[1]);
    }
}