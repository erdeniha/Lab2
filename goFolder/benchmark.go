package main

import (
	"fmt"
	"math"
	"math/rand"
	"strconv"
	"time"
)

const (
	tableSize    = 10007
	trials       = 5
	missingCount = 3000
)

var loads = []float64{0.1, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95}

type Measure struct {
	found   float64
	miss    float64
	longest float64
	nanos   float64
}

func makeKey(rng *rand.Rand, numeric bool) string {
	if numeric {
		return strconv.Itoa(100000000 + rng.Intn(900000000))
	}
	const alphabet = "abcdefghijklmnopqrstuvwxyz0123456789"
	key := make([]byte, 8)
	for i := range key {
		key[i] = alphabet[rng.Intn(len(alphabet))]
	}
	return string(key)
}

func measureTable(t *HashTable, keys []string, count int, missing []string) Measure {
	result := Measure{}

	before := t.probes
	for i := 0; i < count; i++ {
		start := t.probes
		htFind(t, keys[i])
		length := float64(t.probes - start)
		if length > result.longest {
			result.longest = length
		}
	}
	result.found = float64(t.probes-before) / float64(count)

	before = t.probes
	for i := range missing {
		htFind(t, missing[i])
	}
	result.miss = float64(t.probes-before) / float64(len(missing))

	begin := time.Now()
	for i := 0; i < count; i++ {
		htFind(t, keys[i])
	}
	result.nanos = float64(time.Since(begin).Nanoseconds()) / float64(count)
	return result
}

func runExperiment(numeric bool) {
	maxKeys := int(loads[len(loads)-1] * tableSize)
	modes := []ProbeMode{ProbeLinear, ProbeDouble}
	total := [2][]Measure{make([]Measure, len(loads)), make([]Measure, len(loads))}

	for trial := 0; trial < trials; trial++ {
		rng := rand.New(rand.NewSource(int64(trial + 1)))
		used := newSet()
		keys := []string{}
		for len(keys) < maxKeys {
			key := makeKey(rng, numeric)
			if setAdd(used, key) {
				keys = append(keys, key)
			}
		}
		missing := []string{}
		for len(missing) < missingCount {
			key := makeKey(rng, numeric)
			if !setAt(used, key) {
				missing = append(missing, key)
			}
		}

		for m := 0; m < 2; m++ {
			table := newHashTable(modes[m], tableSize, false)
			inserted := 0
			for l := range loads {
				target := int(loads[l] * tableSize)
				for ; inserted < target; inserted++ {
					htInsert(table, keys[inserted], "", nil)
				}
				one := measureTable(table, keys, inserted, missing)
				total[m][l].found += one.found
				total[m][l].miss += one.miss
				total[m][l].longest += one.longest
				total[m][l].nanos += one.nanos
			}
		}
	}

	if numeric {
		fmt.Printf("Ключи: 9-значные числа, таблица %d слотов, среднее по %d запускам\n", tableSize, trials)
	} else {
		fmt.Printf("Ключи: случайные строки по 8 символов, таблица %d слотов, среднее по %d запускам\n", tableSize, trials)
	}
	names := []string{"Линейное апробирование", "Двойное хеширование"}
	for m := 0; m < 2; m++ {
		fmt.Println(names[m])
		fmt.Println("нагрузка  ключей   найден (теория)   не найден (теория)   макс.проб   нс/поиск")
		for l := range loads {
			inserted := int(loads[l] * tableSize)
			a := float64(inserted) / tableSize
			var theoryFound, theoryMiss float64
			if m == 0 {
				theoryFound = 0.5 * (1 + 1/(1-a))
				theoryMiss = 0.5 * (1 + 1/((1-a)*(1-a)))
			} else {
				theoryFound = (1 / a) * math.Log(1/(1-a))
				theoryMiss = 1 / (1 - a)
			}
			fmt.Printf("%8.2f%9d%11.2f (%.2f)%11.2f (%.2f)%12.0f%12.1f\n",
				loads[l], inserted,
				total[m][l].found/trials, theoryFound,
				total[m][l].miss/trials, theoryMiss,
				total[m][l].longest/trials,
				total[m][l].nanos/trials)
		}
	}
	fmt.Println()
}

func runBenchmark() {
	runExperiment(false)
	runExperiment(true)
}
