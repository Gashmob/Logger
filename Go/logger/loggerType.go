package logger

type LoggerType struct {
	name  string
	color LoggerColor
}

var (
	INFO    = LoggerType{"INFO", BLUE}
	SUCCESS = LoggerType{"SUCCESS", GREEN}
	ERROR   = LoggerType{"ERROR", RED}
	WARNING = LoggerType{"WARNING", YELLOW}
	DEBUG   = LoggerType{"DEBUG", PURPLE}
)

func (lt LoggerType) String() string {
	return lt.name
}
