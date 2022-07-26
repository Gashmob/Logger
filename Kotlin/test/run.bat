:: Compile tests
kotlinc -cp Logger_Kotlin.jar test\main.kt test\BasicTest1.kt test\Test.kt test\ThreadTest1.kt test\ThreadTest2.kt test\utils.kt -include-runtime -d test.jar

:: Run tests
kotlin -cp Logger_Kotlin.jar; test.jar
