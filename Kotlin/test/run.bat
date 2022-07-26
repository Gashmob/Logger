:: Compile tests
kotlinc -cp Logger_Kotlin.jar test\Test.kt
kotlinc -cp Logger_Kotlin.jar test\utils.kt

dir
::kotlinc -cp Logger_Kotlin.jar test\main.kt test\BasicTest1.kt test\Test.kt test\ThreadTest1.kt test\ThreadTest2.kt test\utils.kt -include-runtime

:: Run tests
::kotlin -cp Logger_Kotlin.jar; MainKt
