import java.io.OutputStream
import java.io.PrintStream
import kotlin.system.exitProcess


fun main() {
    // Prepare tests
    val tests = ArrayList<Test>()
    tests.add(BasicTest1())
    tests.add(ThreadTest1())
    tests.add(ThreadTest2())

    // ====================

    // Run tests
    println("Running " + tests.size + " tests\n")
    var passed = 0
    for (test in tests) {
        val toPrint = "Running test " + test.name + "..."
        print(toPrint)

        redirectOutput()

        test.before()
        val result = test.run()
        test.after()

        restoreOutput()

        // Erase last print
        for (i in toPrint.indices) {
            print("\b")
        }

        if (result) {
            println("\u001B[30;42m PASS \u001B[00m" + " " + test.name)
            passed++
        } else {
            println("\u001B[30;41m FAIL \u001B[00m" + " " + test.name)
        }
    }

    // ====================

    // Print summary
    println("\n" + passed + " tests passed out of " + tests.size + ".");
    if (passed == tests.size) {
        println("\u001B[30;42m ALL TESTS PASSED \u001B[00m");
    } else {
        println("\u001B[30;41m SOME TESTS FAILED \u001B[00m");
    }

    // Return exit code
    if (passed == tests.size) {
        exitProcess(0);
    } else {
        exitProcess(1);
    }
}

private var originalOut: PrintStream? = null

private fun redirectOutput() {
    originalOut = System.out
    System.setOut(PrintStream(object : OutputStream() {
        override fun write(b: Int) {
            // Do nothing
        }
    }))
}

private fun restoreOutput() {
    System.setOut(originalOut)
}