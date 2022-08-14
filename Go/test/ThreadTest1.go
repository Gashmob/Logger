package main

import (
	"bufio"
	"io/ioutil"
	"logger"
	"os"
	"strings"
	"sync"
	"time"
)

// ThreadTest1 :
// Test thread 1 :
// Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
// Le thread 1 log en info puis attend 1 seconde
// Le thread 2 attend 1 seconde puis log en debug
//
// Conditions de réussite :
// - Le dossier logs est créé.
// - Il contient un seul fichier .log.
// - Le fichier .log contient 5 lignes de logs.
// - La 3ème ligne du fichier .log contient le message "test" en info.
// - La 4ème ligne du fichier .log contient le message "test" en debug.
var ThreadTest1 = Test{
	Name: "ThreadTest1",
	Before: func() {
	},
	Run: func() bool {
		logger.Init(logger.FILE_AND_CONSOLE, []logger.LoggerType{logger.DEBUG, logger.INFO, logger.WARNING, logger.ERROR, logger.SUCCESS})

		var wg sync.WaitGroup
		wg.Add(2)
		go thread1_1(&wg)
		go thread1_2(&wg)

		wg.Wait()

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

		// Le fichier .log contient 5 lignes de logs.
		file, err := os.Open("logs/" + fileName)
		if err != nil {
			return false
		}
		defer file.Close()
		scanner := bufio.NewScanner(file)
		var nbLine = 0
		var line3, line4 string
		for scanner.Scan() {
			if nbLine == 2 {
				line3 = scanner.Text()
			} else if nbLine == 3 {
				line4 = scanner.Text()
			}
			nbLine++
		}
		if nbLine != 5 {
			return false
		}

		// La 3ème ligne du fichier .log contient le message "test" en info.
		if !strings.Contains(line3, "test") && !strings.Contains(line3, "[INFO]") {
			return false
		}

		// La 4ème ligne du fichier .log contient le message "test" en debug.
		if !strings.Contains(line4, "test") && !strings.Contains(line4, "[DEBUG]") {
			return false
		}

		return true
	},
	After: func() {
		ClearDir("logs")
	},
}

func thread1_1(wg *sync.WaitGroup) {
	defer wg.Done()
	logger.Info("test")
	time.Sleep(1 * time.Second)
}

func thread1_2(wg *sync.WaitGroup) {
	defer wg.Done()
	time.Sleep(1 * time.Second)
	logger.Debug("test")
}
