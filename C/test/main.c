#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "test.h"

void redirectOutput();

void restoreOutput();

int main() {
    // Prepare tests
#define NB_TEST 3
    Test test[NB_TEST];
    extern Test BasicTest1;
    extern Test ThreadTest1;
    extern Test ThreadTest2;
    test[0] = BasicTest1;
    test[1] = ThreadTest1;
    test[2] = ThreadTest2;

    // ====================

    // Run tests
    printf("Running %d tests...\n", NB_TEST);
    int passed = 0;
    for (int i = 0; i < NB_TEST; ++i) {
        Test t = test[i];
        char *toPrint = malloc(sizeof(char) * (strlen(t.name) + 20));
        sprintf(toPrint, "Running test %s...", t.name);
        printf("%s", toPrint);
        fflush(stdout);

        redirectOutput();

        if (t.before != NULL) {
            t.before();
        }
        bool result = t.run();
        if (t.after != NULL) {
            t.after();
        }

        restoreOutput();

        // Erase last print
        for (int j = 0; j < strlen(toPrint); ++j) {
            printf("\b");
        }

        if (result) {
            printf("\x1B[30;42m PASS \x1B[00m %s\n", t.name);
            passed++;
        } else {
            printf("\x1B[30;41m FAIL \x1B[00m %s\n", t.name);
        }
    }

    // ====================

    // Print summary
    printf("\n%d tests passed out of %d.\n", passed, NB_TEST);
    if (passed == NB_TEST) {
        printf("\x1B[30;42m ALL TESTS PASSED \x1B[00m\n");
    } else {
        printf("\x1B[30;41m SOME TESTS FAILED \x1B[00m");
    }

    // Return exit code
    if (passed == NB_TEST) {
        return 0;
    } else {
        return 1;
    }
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

void redirectOutput() {

}

void restoreOutput() {

}