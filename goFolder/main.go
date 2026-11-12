package main

import (
	"fmt"
	"os"
)

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		printUsage()
		os.Exit(1)
	}
	first := args[0]

	if len(first) > 1 && first[0] == '-' {
		file := ""
		query := ""
		for i := 0; i+1 < len(args); i += 2 {
			if args[i] == "--file" {
				file = args[i+1]
			} else if args[i] == "--query" {
				query = args[i+1]
			}
		}
		if file == "" || query == "" {
			printUsage()
			os.Exit(1)
		}
		if err := runQuery(file, query); err != nil {
			fmt.Println("-> ОШИБКА:", err)
			os.Exit(1)
		}
		return
	}

	code, found := runTask(first, args[1:])
	if !found {
		fmt.Println("ERROR: unknown task: " + first)
		printUsage()
		os.Exit(1)
	}
	os.Exit(code)
}
