package main

type Test struct {
	Name   string
	Before func()
	Run    func() bool
	After  func()
}
