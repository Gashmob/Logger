#!/bin/bash

javac ./logger/*.java ./logger/**/*.java

jar -cf Logger_Java.jar ./logger/*.class ./logger/**/*.class