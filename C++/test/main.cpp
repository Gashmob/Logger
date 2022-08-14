#include <vector>
#include <iostream>
#include <sstream>
#include "test.h"

using namespace std;

streambuf *original_out;

void redirectOutput();

void restoreOutput();

int main() {
    // Prepare tests
    vector<Test> tests;
    extern Test BasicTest1;
    extern Test ThreadTest1;
    extern Test ThreadTest2;
    tests.push_back(BasicTest1);
    tests.push_back(ThreadTest1);
    tests.push_back(ThreadTest2);

    // ====================

    // Run tests
    cout << "Running " << tests.size() << " tests..." << endl;
    int passed = 0;
    for (const auto &test: tests) {
        string toPrint = "Running test " + test.name + "...";
        cout << toPrint << flush;

        redirectOutput();

        if (test.before) {
            test.before();
        }
        bool result = test.run();
        if (test.after) {
            test.after();
        }

        restoreOutput();

        // Erase last print
        cout << string(toPrint.size(), '\b');

        if (result) {
            cout << "\u001B[30;42m PASS \u001B[00m " << test.name << endl;
            passed++;
        } else {
            cout << "\u001B[30;41m FAIL \u001B[00m " << test.name << endl;
        }
    }

    // ====================

    // Print summary
    cout << endl << passed << " tests passed out of " << tests.size() << "." << endl;
    if (passed == tests.size()) {
        cout << "\u001B[30;42m ALL TESTS PASSED \u001B[00m" << endl;
    } else {
        cout << "\u001B[30;41m SOME TESTS FAILED \u001B[00m" << endl;
    }

    // Return exit code
    if (passed == tests.size()) {
        return 0;
    } else {
        return 1;
    }
}

// _.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-.

void redirectOutput() {
    original_out = cout.rdbuf();
    auto *ss = new stringstream();
    cout.rdbuf(ss->rdbuf());
}

void restoreOutput() {
    cout.rdbuf(original_out);
}