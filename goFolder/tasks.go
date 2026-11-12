package main

import (
	"fmt"
	"strconv"
	"strings"
)

func fail(message string) int {
	fmt.Println("ERROR: " + message)
	return 1
}

func isNumber(s string, maxDigits int) bool {
	if s == "" || len(s) > maxDigits {
		return false
	}
	for i := 0; i < len(s); i++ {
		if s[i] < '0' || s[i] > '9' {
			return false
		}
	}
	return true
}

func parseInteger(s string) (int64, bool) {
	negative := s != "" && s[0] == '-'
	digits := s
	if negative {
		digits = s[1:]
	}
	if !isNumber(digits, 15) {
		return 0, false
	}
	value, _ := strconv.ParseInt(digits, 10, 64)
	if negative {
		value = -value
	}
	return value, true
}

func taskExpr(args []string) int {
	text := strings.Join(args, " ")
	result, msg := evalExpression(text)
	if msg != "" {
		return fail(msg)
	}
	fmt.Println(result)
	return 0
}

func taskPartition(args []string) int {
	start := 0
	var wanted int64
	if len(args) >= 2 && args[0] == "--sum" {
		if !isNumber(args[1], 15) {
			return fail("--sum must be a natural number")
		}
		wanted, _ = strconv.ParseInt(args[1], 10, 64)
		if wanted == 0 {
			return fail("--sum must be a natural number")
		}
		start = 2
	}

	input := newArray()
	var total int64
	for i := start; i < len(args); i++ {
		value, _ := strconv.ParseInt(args[i], 10, 64)
		if !isNumber(args[i], 9) || value == 0 {
			return fail("'" + args[i] + "' is not a natural number")
		}
		arrayPush(input, strconv.FormatInt(value, 10))
		total += value
	}
	n := input.size
	if n == 0 {
		return fail("the set is empty")
	}

	found := 0
	for k := 2; k <= n; k++ {
		if total%int64(k) != 0 {
			continue
		}
		target := total / int64(k)
		if wanted != 0 && target != wanted {
			continue
		}
		if partition, ok := findPartition(input, target); ok {
			fmt.Printf("Sum %d (%d subsets): ", target, k)
			printPartition(partition)
			found++
		}
	}
	if found == 0 {
		return fail("the set cannot be partitioned into subsets with equal sum")
	}
	return 0
}

func taskSubsets(args []string) int {
	if len(args) > 20 {
		return fail("too many elements (at most 20)")
	}
	items := newArray()
	result := newArray()
	for i := 0; i < len(args); i++ {
		arrayPush(items, args[i])
	}
	allSubsets(items, result)
	fmt.Println("[" + strings.Join(arrayValues(result), ", ") + "]")
	return 0
}

func taskBst(args []string) int {
	tree := &Tree{}
	for i := 0; i < len(args); i++ {
		value, ok := parseInteger(args[i])
		if !ok {
			return fail("'" + args[i] + "' is not an integer")
		}
		if !bstInsert(tree, value) {
			fmt.Printf("Duplicate ignored: %d\n", value)
		}
	}
	printBst(tree)

	edge := newArray()
	bstRightEdge(tree, edge)
	fmt.Println("Right edge: " + strings.Join(arrayValues(edge), ", "))
	return 0
}

func taskFold(args []string) int {
	size := 11
	mode := ProbeDouble
	keys := []string{}
	for i := 0; i < len(args); i++ {
		if args[i] == "--size" && i+1 < len(args) {
			if !isNumber(args[i+1], 6) {
				return fail("--size must be a number from 5")
			}
			number, _ := strconv.Atoi(args[i+1])
			if number < 5 {
				return fail("--size must be a number from 5")
			}
			size = number
			i++
		} else if args[i] == "--mode" && i+1 < len(args) {
			i++
			if args[i] != "linear" && args[i] != "double" {
				return fail("--mode must be linear or double")
			}
			if args[i] == "linear" {
				mode = ProbeLinear
			} else {
				mode = ProbeDouble
			}
		} else {
			keys = append(keys, args[i])
		}
	}
	if len(keys) == 0 {
		return fail("no keys given")
	}

	table := newHashTable(mode, size, false)
	for i := 0; i < len(keys); i++ {
		sum := foldKey(keys[i], 3)
		fmt.Printf("%s: %s = %d, home slot = %d mod %d = %d\n",
			keys[i], foldParts(keys[i], 3), sum, sum, table.capacity, sum%int64(table.capacity))
		if htInsert(table, keys[i], "", nil) == HtFull {
			fmt.Printf("  table is full, '%s' was not added\n", keys[i])
		}
	}
	printHashTable(table)
	return 0
}

func taskLru(args []string) int {
	if len(args) < 1 || !isNumber(args[0], 6) {
		return fail("first argument must be the cache capacity (a number from 1)")
	}
	capacity, _ := strconv.Atoi(args[0])
	if capacity < 1 {
		return fail("first argument must be the cache capacity (a number from 1)")
	}
	t := []string{}
	for i := 1; i < len(args); i++ {
		t = append(t, strings.Fields(args[i])...)
	}

	cache := newLRU(capacity)
	output := ""
	i := 0
	for i < len(t) {
		if t[i] == "SET" && i+2 < len(t) {
			lruSet(cache, t[i+1], t[i+2])
			i += 3
		} else if t[i] == "GET" && i+1 < len(t) {
			value, ok := lruGet(cache, t[i+1])
			if !ok {
				value = "-1"
			}
			if output != "" {
				output += " "
			}
			output += value
			i += 2
		} else {
			return fail("bad query at word '" + t[i] + "' (expected SET x y or GET x)")
		}
	}
	fmt.Println(output)
	return 0
}

func taskBench(args []string) int {
	runBenchmark()
	return 0
}

type Task struct {
	name string
	run  func(args []string) int
}

var tasks = []Task{
	{"expr", taskExpr},
	{"partition", taskPartition},
	{"subsets", taskSubsets},
	{"bst", taskBst},
	{"fold", taskFold},
	{"lru", taskLru},
	{"bench", taskBench},
}

func runTask(name string, args []string) (int, bool) {
	for i := 0; i < len(tasks); i++ {
		if tasks[i].name == name {
			return tasks[i].run(args), true
		}
	}
	return 1, false
}

func printUsage() {
	fmt.Println("Задача №1     ./lab2 expr '<expression>'")
	fmt.Println("Задача №2     ./lab2 --file <file> --query '<COMMAND args>'   (SETADD, SETDEL, SET_AT, ...)")
	fmt.Println("Задача №3     ./lab2 partition [--sum S] n1 n2 ...")
	fmt.Println("Задача №4     ./lab2 subsets e1 e2 ...")
	fmt.Println("Задача №5     ./lab2 bst v1 v2 ...")
	fmt.Println("Задача №6.1   ./lab2 bench")
	fmt.Println("Задача №6.2   ./lab2 fold [--size N] [--mode linear|double] key ...")
	fmt.Println("Задача №7     ./lab2 lru <capacity> '<SET x y | GET x ...>'")
}
