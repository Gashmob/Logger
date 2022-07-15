#!/bin/bash

# Compile tests
javac ./logger/*.java
javac -cp ./logger: ./test/*.java

# Run tests
java -cp ./logger: test.Main