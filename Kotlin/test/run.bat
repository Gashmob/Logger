:: Compile tests
kotlinc -cp Logger_Kotlin.jar test\Test.kt
kotlinc -cp Logger_Kotlin.jar test\utils.kt
kotlinc -cp Logger_Kotlin.jar;. test\BasicTest1.kt
kotlinc -cp Logger_Kotlin.jar;. test\ThreadTest1.kt
kotlinc -cp Logger_Kotlin.jar;. test\ThreadTest2.kt
kotlinc -cp Logger_Kotlin.jar;. test\main.kt -include-runtime

dir
::kotlinc -cp Logger_Kotlin.jar test\main.kt test\BasicTest1.kt test\ThreadTest1.kt test\ThreadTest2.kt -include-runtime

:: Run tests
::kotlin -cp Logger_Kotlin.jar; MainKt
