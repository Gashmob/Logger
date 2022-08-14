package main

import (
	"bufio"
	"io/ioutil"
	logger "logger"
	"os"
)

// BasicTest1 :
// Initialise le logger, utilise tout les niveaux de logs et exit le logger.
//
// Conditions de réussite :
// - Le dossier logs est créé.
// - Il contient un seul fichier .log.
// - Le fichier .log contient 8 lignes de logs.
var BasicTest1 = Test{
	Name: "BasicTest1",
	Before: func() {
	},
	Run: func() bool {
		logger.Init(logger.FILE_AND_CONSOLE, []logger.LoggerType{logger.DEBUG, logger.INFO, logger.WARNING, logger.ERROR, logger.SUCCESS})

		logger.Info("Info message")
		logger.Success("Success message")
		logger.Error("Error message")
		logger.Warning("Warning message")
		logger.Debug("Debug message")

		logger.Exit()

		// ====================

		// Le dossier logs est créé
		if _, err := os.Stat("logs"); os.IsNotExist(err) {
			return false
		}

		// Il contient un seul fichier .log.
		files, err := ioutil.ReadDir("logs")
		if err != nil {
			return false
		}
		if len(files) != 1 {
			return false
		}
		var fileName = files[0].Name()

		// Le fichier .log contient 8 lignes de logs.
		file, err := os.Open("logs/" + fileName)
		if err != nil {
			return false
		}
		defer file.Close()
		scanner := bufio.NewScanner(file)
		var nbLine = 0
		for scanner.Scan() {
			nbLine++
		}
		if nbLine != 8 {
			return false
		}

		return true
	},
	After: func() {
		ClearDir("logs")
	},
}
