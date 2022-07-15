import java.io.File

fun clearDir(dir: File) {
    if (dir.isDirectory) {
        for (file in dir.listFiles()!!) {
            clearDir(file)
        }
    }
    dir.delete()
}