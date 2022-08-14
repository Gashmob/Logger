package logger

type LoggerOption int

const (
	FILE_ONLY LoggerOption = iota
	CONSOLE_ONLY
	FILE_AND_CONSOLE
)
