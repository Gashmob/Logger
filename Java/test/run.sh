#!/bin/bash

# Compile tests
./jar.sh
javac -cp Logger_Java.jar ./test/*.java

# Run tests
java -cp ./logger: test.Main