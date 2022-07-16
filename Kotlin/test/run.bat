:: Compile tests
kotlinc -cp Logger_Kotlin.jar; .\test\*.kt -include-runtime

:: Run tests
kotlin -cp Logger_Kotlin.jar; MainKt
java -cp Logger_Java.jar; test.Main