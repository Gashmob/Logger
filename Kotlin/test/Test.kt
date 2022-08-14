abstract class Test(val name: String) {
    abstract fun run(): Boolean

    open fun before() {
        // do nothing
    }

    open fun after() {
        // do nothing
    }
}