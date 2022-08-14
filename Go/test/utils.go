package main

import (
	"io/ioutil"
	"os"
	"path/filepath"
)

func ClearDir(dir string) {
	files, err := ioutil.ReadDir(dir)
	if err != nil {
		return
	}
	for _, file := range files {
		err := os.RemoveAll(filepath.Join(dir, file.Name()))
		if err != nil {
			return
		}
	}
	err = os.Remove(dir)
	if err != nil {
		return
	}
}
