#include "test.h"
#include <thread>
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif
#include <regex>

#include "../logger/Logger.hpp"

#ifdef __APPLE__
static dispatch_semaphore_t    sem;
#else
static sem_t                   sem;
#endif

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
//  * - Les 3ème et 4ème lignes du fichier sont bien formées.
 */
Test ThreadTest2 = {
        "ThreadTest2",
        []() {
            // Do nothing
        },
        []() {
            Logger::init();

            #ifdef __APPLE__
            sem = *dispatch_semaphore_create(value);
#else
            sem_init(&sem, 0, 0);
#endif

            std::thread t1([]() {
#ifdef __APPLE__
                dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
                sem_wait(&sem);
#endif
                INFO_LOG(FILE_AND_CONSOLE, "test");
            });
            std::thread t2([]() {
#ifdef __APPLE__
                dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
                sem_wait(&sem);
#endif
                DEBUG_LOG(FILE_AND_CONSOLE, "test");
            });

#ifdef __APPLE__
            dispatch_semaphore_signal(sem);
            dispatch_semaphore_signal(sem);
#else
            sem_post(&sem);
            sem_post(&sem);
#endif

            t1.join();
            t2.join();

            //sem_destroy(&sem);

            Logger::exit();

            // ====================

            // Le dossier logs est créé.
            struct stat buffer{};
            if (stat("logs", &buffer) != 0 || !S_ISDIR(buffer.st_mode)) {
                return false;
            }

            // Il contient un seul fichier .log.
            DIR *dir = opendir("logs");
            if (dir == nullptr) {
                return false;
            }
            struct dirent *ent;
            int nbFiles = 0;
            std::string fileName;
            while ((ent = readdir(dir)) != nullptr) {
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
            std::ifstream file("logs/" + fileName);
            if (!file.is_open()) {
                return false;
            }
            int nbLines = 0;
            std::string line;
            std::string line3;
            std::string line4;
            while (std::getline(file, line)) {
                if (nbLines == 2) {
                    line3 = line;
                } else if (nbLines == 3) {
                    line4 = line;
                }
                nbLines++;
            }
            file.close();
            if (nbLines != 5) {
                return false;
            }

            // Les 3ème et 4ème lignes du fichier sont bien formées.
            // [2-12:12:16:1116084-INFO]	[operator()]	test
            // [3-12:12:16:1326229-DEBUG]	[operator()]	test
            std::regex regex1("(.*)-INFO\\]\t\\[operator\\(\\)\\]\ttest");
            std::regex regex2("(.*)-DEBUG\\]\t\\[operator\\(\\)\\]\ttest");
            bool a1 = std::regex_match(line3, regex1);
            bool a2 = std::regex_match(line4, regex1);
            bool b1 = std::regex_match(line3, regex2);
            bool b2 = std::regex_match(line4, regex2);
            if (!std::regex_match(line3, regex1) && !std::regex_match(line4, regex1)) {
                return false;
            }
            if (!std::regex_match(line3, regex2) && !std::regex_match(line4, regex2)) {
                return false;
            }

            return true;
        },
        []() {
            rmDir("logs");
        }
};