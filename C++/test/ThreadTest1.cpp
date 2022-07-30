#include "test.h"
#include <thread>

#include "../logger/Logger.hpp"

/**
 * Test thread 1 :
 * Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
 * Le thread 1 log en info puis attend 1 seconde
 * Le thread 2 attend 1 seconde puis log en debug
 * <p>
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 5 lignes de logs.
 * - La 3ème ligne du fichier .log contient le message "test" en info.
 * - La 4ème ligne du fichier .log contient le message "test" en debug.
 */
Test ThreadTest1 = {
        "ThreadTest1",
        []() {
            // Do nothing
        },
        []() {
            Logger::init();

            std::thread t1([]() {
                INFO_LOG(FILE_AND_CONSOLE, "test");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            });
            std::thread t2([]() {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                DEBUG_LOG(FILE_AND_CONSOLE, "test");
            });

            t1.join();
            t2.join();

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

            // La 3ème ligne du fichier .log contient le message "test" en info.
            if (line3.find("test") == std::string::npos || line3.find("INFO") == std::string::npos) {
                return false;
            }

            // La 4ème ligne du fichier .log contient le message "test" en debug.
            if (line4.find("test") == std::string::npos || line4.find("DEBUG") == std::string::npos) {
                return false;
            }

            return true;
        },
        []() {
            rmDir("logs");
        }
};