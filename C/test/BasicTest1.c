#include "test.h"

#include "../logger/logger.h"

/**
 * Test basique 1 :
 * Initialise le logger, utilise tout les niveaux de logs et exit le logger.
 *
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 8 lignes de logs.
 */
bool runBasicTest1() {
    LoggerType types[] = {INFO, SUCCESS, ERROR, WARNING, DEBUG};
    logger_init(FILE_AND_CONSOLE, types);

    INFO_LOG(FILE_AND_CONSOLE, "Info message");
    SUCCESS_LOG(FILE_AND_CONSOLE, "Success message");
    ERROR_LOG(FILE_AND_CONSOLE, "Error message");
    WARNING_LOG(FILE_AND_CONSOLE, "Warning message");
    DEBUG_LOG(FILE_AND_CONSOLE, "Debug message");

    logger_exit();

    // ====================

    // Le dossier logs est créé.
    struct stat buffer;
    if (stat("logs", &buffer) != 0 || !S_ISDIR(buffer.st_mode)) {
        return false;
    }

    // Il contient un seul fichier .log.
    DIR *dir = opendir("logs");
    if (dir == NULL) {
        return false;
    }
    struct dirent *ent;
    int nbFiles = 0;
    char *fileName;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            nbFiles++;
            fileName = ent->d_name;
        }
    }
    closedir(dir);
    if (nbFiles != 1) {
        return false;
    }

    // Le fichier .log contient 8 lignes de logs.
    char *filePath = malloc(sizeof(char) * (strlen("logs/") + strlen(fileName) + 1));
    sprintf(filePath, "logs/%s", fileName);
    FILE *file = fopen(filePath, "r");
    if (!file) {
        return false;
    }
    int nbLines = 0;
    char *line;
    size_t len = 0;
    while (getline(&line, &len, file) != -1) {
        nbLines++;
    }
    fclose(file);
    if (nbLines != 8) {
        return false;
    }

    return true;
}

Test BasicTest1 = {
        .name = "BasicTest1",
        .before = nothing,
        .run = runBasicTest1,
        .after = clearDir,
};