#!/bin/bash

# Compile tests
javac -cp Logger_Java.jar ./test/*.java

# Run tests
java test.Main