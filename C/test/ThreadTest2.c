#include "test.h"
#include <regex.h>

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else

#include <semaphore.h>

#endif

#include "../logger/logger.h"

#ifdef __APPLE__
static dispatch_semaphore_t    sem;
#else
static sem_t sem;
#endif

void *run_thread2_1(void *arg) {
    (void) arg;

#ifdef __APPLE__
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
    sem_wait(&sem);
#endif
    INFO_LOG(FILE_AND_CONSOLE, "test");

    return 0;
}

void *run_thread2_2(void *arg) {
    (void) arg;

#ifdef __APPLE__
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
    sem_wait(&sem);
#endif
    DEBUG_LOG(FILE_AND_CONSOLE, "test");

    return 0;
}

/**
 * Test thread 2 :
 * Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
 * Le thread 1 log en info
 * Le thread 2 log en debug
 * Les 2 threads attendent que le sémaphore soit libéré avant de log en info et debug
 *
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 5 lignes de logs.
 * - Les 3ème et 4ème lignes du fichier sont bien formées.
 */
bool runThreadTest2() {
    LoggerType types[] = {INFO, SUCCESS, ERROR, WARNING, DEBUG};
    logger_init(FILE_AND_CONSOLE, types);

#ifdef __APPLE__
    sem = dispatch_semaphore_create(0);
#else
    sem_init(&sem, 0, 0);
#endif

    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, run_thread2_1, NULL) != 0)
        return false;
    if (pthread_create(&thread2, NULL, run_thread2_2, NULL) != 0)
        return false;

#ifdef __APPLE__
    dispatch_semaphore_signal(sem);
    dispatch_semaphore_signal(sem);
#else
    sem_post(&sem);
    sem_post(&sem);
#endif

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

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

    // Le fichier .log contient 5 lignes de logs.
    char *filePath = malloc(sizeof(char) * (strlen("logs/") + strlen(fileName) + 1));
    sprintf(filePath, "logs/%s", fileName);
    FILE *file = fopen(filePath, "r");
    if (!file) {
        return false;
    }
    int nbLines = 0;
    char *line;
    char *line3 = malloc(sizeof(char) * 100);
    char *line4 = malloc(sizeof(char) * 100);
    size_t len = 0;
    while (getline(&line, &len, file) != -1) {
        if (nbLines == 2) {
            strcpy(line3, line);
        } else if (nbLines == 3) {
            strcpy(line4, line);
        }
        nbLines++;
    }
    fclose(file);
    if (nbLines != 5) {
        return false;
    }

    // Les 3ème et 4ème lignes du fichier sont bien formées.
    regex_t regex1;
    if (regcomp(&regex1, "(.*)-INFO\\]\\t\\[run_thread2_1\\]\\ttest", REG_EXTENDED) != 0) {
        return false;
    }
    regex_t regex2;
    if (regcomp(&regex2, "(.*)-DEBUG\\]\\t\\[run_thread2_2\\]\\ttest", REG_EXTENDED) != 0) {
        return false;
    }
    if (!regexec(&regex1, line3, 0, NULL, 0) || !regexec(&regex1, line4, 0, NULL, 0)) {
        return false;
    }
    if (!regexec(&regex2, line3, 0, NULL, 0) || !regexec(&regex2, line4, 0, NULL, 0)) {
        return false;
    }

    return true;
}

Test ThreadTest2 = {
        .name = "ThreadTest2",
        .before = nothing,
        .run = runThreadTest2,
        .after = clearDir,
};