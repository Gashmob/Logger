#include "test.h"

#include "../logger/Logger.hpp"

/**
 * Test basique 1 :
 * Initialise le logger, utilise tout les niveaux de logs et exit le logger.
 *
 * Conditions de réussite :
 * - Le dossier logs est créé.
 * - Il contient un seul fichier .log.
 * - Le fichier .log contient 8 lignes de logs.
 */
Test BasicTest1 = {
        "BasicTest1",
        []() {
            // Do nothing
        },
        []() {
            Logger::init();

            INFO_LOG(FILE_AND_CONSOLE, "Info message");
            SUCCESS_LOG(FILE_AND_CONSOLE, "Success message");
            ERROR_LOG(FILE_AND_CONSOLE, "Error message");
            WARNING_LOG(FILE_AND_CONSOLE, "Warning message");
            DEBUG_LOG(FILE_AND_CONSOLE, "Debug message");

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

            // Le fichier .log contient 8 lignes de logs.
            std::ifstream file("logs/" + fileName);
            if (!file.is_open()) {
                return false;
            }
            int nbLines = 0;
            std::string line;
            while (std::getline(file, line)) {
                nbLines++;
            }
            file.close();
            if (nbLines != 8) {
                return false;
            }

            return true;
        },
        []() {
            rmDir("logs");
        }
};