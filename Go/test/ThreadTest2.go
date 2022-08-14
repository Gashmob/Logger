package main

import (
	"bufio"
	"context"
	"golang.org/x/sync/semaphore"
	"io/ioutil"
	"logger"
	"os"
	"regexp"
	"sync"
	"time"
)

// ThreadTest2 :
// Initialise le logger, lance 2 threads qui utilisent le logger et exit le logger après avoir join les threads.
// Le thread 1 log en info
// Le thread 2 log en debug
// Les 2 threads attendent que le sémaphore soit libéré avant de log en info et debug
//
// Conditions de réussite :
// - Le dossier logs est créé.
// - Il contient un seul fichier .log.
// - Le fichier .log contient 5 lignes de logs.
// - Les 3ème et 4ème lignes du fichier sont bien formées.
var ThreadTest2 = Test{
	Name: "ThreadTest2",
	Before: func() {
	},
	Run: func() bool {
		logger.Init(logger.FILE_AND_CONSOLE, []logger.LoggerType{logger.DEBUG, logger.INFO, logger.WARNING, logger.ERROR, logger.SUCCESS})

		var sem = semaphore.NewWeighted(2)
		var wg sync.WaitGroup
		wg.Add(2)
		go thread2_1(&wg, sem)
		go thread2_2(&wg, sem)

		time.Sleep(1 * time.Second)
		sem.Release(2)
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

		// Les 3ème et 4ème lignes du fichier sont bien formées.
		// [14-11:15:51:122434198-INFO]	[main.thread2_1:55]	test
		// [14-11:15:51:122453289-DEBUG]	[main.thread2_2:65]	test
		var regex1 = "(.*)-INFO]\\t\\[main\\.thread2_1:.*]\\ttest"
		var regex2 = "(.*)-DEBUG]\\t\\[main\\.thread2_2:.*]\\ttest"
		var res1, _ = regexp.MatchString(regex1, line3)
		var res2, _ = regexp.MatchString(regex1, line4)
		var res3, _ = regexp.MatchString(regex2, line3)
		var res4, _ = regexp.MatchString(regex2, line4)
		if !res1 && !res2 {
			return false
		}
		if !res3 && !res4 {
			return false
		}

		return true
	},
	After: func() {
		ClearDir("logs")
	},
}

func thread2_1(wg *sync.WaitGroup, sem *semaphore.Weighted) {
	defer wg.Done()
	ctxt := context.Background()
	err := sem.Acquire(ctxt, 1)
	if err != nil {
		return
	}
	logger.Info("test")
}

func thread2_2(wg *sync.WaitGroup, sem *semaphore.Weighted) {
	defer wg.Done()
	ctxt := context.Background()
	err := sem.Acquire(ctxt, 1)
	if err != nil {
		return
	}
	logger.Debug("test")
}
