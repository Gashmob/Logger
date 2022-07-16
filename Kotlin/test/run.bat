:: Compile tests
dir test
::kotlinc -cp Logger_Kotlin.jar; .\test\*.kt -include-runtime

:: Run tests
::kotlin -cp Logger_Kotlin.jar; MainKt