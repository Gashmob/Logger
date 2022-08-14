module test

go 1.13

replace logger => ../logger

require (
	golang.org/x/sync v0.0.0-20220722155255-886fb9371eb4
	logger v0.0.0-00010101000000-000000000000
)
