package test;

import java.io.OutputStream;
import java.io.PrintStream;
import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {
        // Prepare tests
        ArrayList<Test> tests = new ArrayList<>();
        tests.add(new BasicTest1());
        tests.add(new ThreadTest1());
        tests.add(new ThreadTest2());

        // ====================

        // Run tests
        System.out.println("Running " + tests.size() + " tests\n");
        int passed = 0;
        for (Test test : tests) {
            String toPrint = "Running test " + test.name + "...";
            System.out.print(toPrint);

            redirectOutput();

            test.before();
            boolean result = test.run();
            test.after();

            restoreOutput();

            // Erase last print
            for (int i = 0; i < toPrint.length(); i++) {
                System.out.print("\b");
            }

            if (result) {
                System.out.println("\u001B[30;42m PASS \u001B[00m" + " " + test.name);
                passed++;
            } else {
                System.out.println("\u001B[30;41m FAIL \u001B[00m" + " " + test.name);
            }
        }

        // ====================

        // Print summary
        System.out.println("\n" + passed + " tests passed out of " + tests.size() + ".");
        if (passed == tests.size()) {
            System.out.println("\u001B[30;42m ALL TESTS PASSED \u001B[00m");
        } else {
            System.out.println("\u001B[30;41m SOME TESTS FAILED \u001B[00m");
        }

        // Return exit code
        if (passed == tests.size()) {
            System.exit(0);
        } else {
            System.exit(1);
        }
    }

    private static PrintStream originalOut;

    private static void redirectOutput() {
        originalOut = System.out;
        System.setOut(new PrintStream(new OutputStream() {
            @Override
            public void write(int b) {
                // Do nothing
            }
        }));
    }

    private static void restoreOutput() {
        System.setOut(originalOut);
    }
}
