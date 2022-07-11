package test;

import java.io.File;
import java.util.Objects;

public final class Utils {
    private Utils() {}

    public static void clearDir(File dir) {
        if (dir.isDirectory()) {
            for (File f : Objects.requireNonNull(dir.listFiles())) {
                clearDir(f);
            }
        }
        dir.delete();
    }
}
