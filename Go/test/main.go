package main

import (
	"fmt"
	"os"
)

func main() {
	// Prepare tests
	var tests [3]Test
	tests[0] = BasicTest1
	tests[1] = ThreadTest1
	tests[2] = ThreadTest2

	// ====================

	// Run tests
	fmt.Printf("Running %d tests\n", len(tests))
	var passed = 0
	for _, test := range tests {
		var toPrint = "Running test " + test.Name + "..."
		fmt.Print(toPrint)

		temp := os.Stdout
		os.Stdout = nil

		test.Before()
		var result = test.Run()
		test.After()

		os.Stdout = temp

		// Erase last print
		fmt.Print("\r")
		for i := 0; i < len(toPrint); i++ {
			fmt.Print(" ")
		}
		fmt.Print("\r")

		if result {
			fmt.Println("\u001B[30;42m PASS \u001B[00m" + " " + test.Name)
			passed++
		} else {
			fmt.Println("\u001B[30;41m FAIL \u001B[00m" + " " + test.Name)
		}
	}

	// ====================

	// Print summary
	fmt.Printf("\n%d tests passed out of %d\n", passed, len(tests))
	if passed == len(tests) {
		fmt.Println("\u001B[30;42m ALL TESTS PASSED \u001B[00m")
	} else {
		fmt.Println("\u001B[30;41m SOME TESTS FAILED \u001B[00m")
	}

	// Return exit code
	if passed == len(tests) {
		os.Exit(0)
	} else {
		os.Exit(1)
	}
}
