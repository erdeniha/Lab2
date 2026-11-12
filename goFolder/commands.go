package main

import (
	"errors"
	"fmt"
	"os"
	"strings"
)

type Op int

const (
	OpSetAdd Op = iota
	OpSetDel
	OpSetAt
)

type Command struct {
	name    string
	op      Op
	creates bool
}

var commands = []Command{
	{"SETADD", OpSetAdd, true},
	{"SETDEL", OpSetDel, false},
	{"SET_AT", OpSetAt, false},
}

func findCommand(name string) *Command {
	for i := range commands {
		if commands[i].name == name {
			return &commands[i]
		}
	}
	return nil
}

func toUpper(s string) string {
	bytes := []byte(s)
	for i := range bytes {
		if bytes[i] >= 'a' && bytes[i] <= 'z' {
			bytes[i] = bytes[i] - 'a' + 'A'
		}
	}
	return string(bytes)
}

func readLines(file string) []string {
	data, err := os.ReadFile(file)
	if err != nil {
		return nil
	}
	lines := []string{}
	for _, line := range strings.Split(string(data), "\n") {
		if line != "" {
			lines = append(lines, line)
		}
	}
	return lines
}

func writeLines(file string, lines []string) error {
	text := ""
	for _, line := range lines {
		text += line + "\n"
	}
	return os.WriteFile(file, []byte(text), 0644)
}

func findLine(lines []string, name string) int {
	for i, line := range lines {
		fields := strings.Fields(line)
		if len(fields) > 0 && fields[0] == name {
			return i
		}
	}
	return -1
}

func doSet(op Op, element string, values []string) (bool, []string, bool) {
	set := newSet()
	for _, v := range values {
		setAdd(set, v)
	}

	result := false
	changed := false
	switch op {
	case OpSetAdd:
		result = setAdd(set, element)
		changed = result
	case OpSetDel:
		result = setDel(set, element)
		changed = result
	case OpSetAt:
		result = setAt(set, element)
	}

	if changed {
		values = setValues(set)
	}
	return result, values, changed
}

func runQuery(file, query string) error {
	t := strings.Fields(query)
	if len(t) == 0 {
		return errors.New("пустой запрос")
	}
	t[0] = toUpper(t[0])

	cmd := findCommand(t[0])
	if cmd == nil {
		return errors.New("неизвестная команда: " + t[0])
	}
	if len(t) != 3 {
		return errors.New("неверное число аргументов у команды " + t[0])
	}

	lines := readLines(file)
	name := t[1]
	lineIndex := findLine(lines, name)

	if lineIndex < 0 && !cmd.creates {
		return errors.New("множество '" + name + "' не найдено")
	}

	values := []string{}
	if lineIndex >= 0 {
		values = strings.Fields(lines[lineIndex])[1:]
	}

	result, values, changed := doSet(cmd.op, t[2], values)
	if result {
		fmt.Println("-> TRUE")
	} else {
		fmt.Println("-> FALSE")
	}

	if changed {
		line := name
		for _, v := range values {
			line += " " + v
		}
		if lineIndex >= 0 {
			lines[lineIndex] = line
		} else {
			lines = append(lines, line)
		}
		if err := writeLines(file, lines); err != nil {
			return errors.New("не удалось записать файл " + file)
		}
	}
	return nil
}
