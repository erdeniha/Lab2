package main

import (
	"fmt"
	"strconv"
	"strings"
)

type Partition struct {
	groups []*Array
	count  int
}

func place(numbers []int64, index int, target int64, sums []int64, basket []int) bool {
	if index == len(numbers) {
		return true
	}
	for b := 0; b < len(sums); b++ {
		if sums[b]+numbers[index] > target {
			continue
		}
		sameAsBefore := false
		for c := 0; c < b; c++ {
			if sums[c] == sums[b] {
				sameAsBefore = true
			}
		}
		if sameAsBefore {
			continue
		}
		sums[b] += numbers[index]
		basket[index] = b
		if place(numbers, index+1, target, sums, basket) {
			return true
		}
		sums[b] -= numbers[index]
	}
	return false
}

func findPartition(input *Array, target int64) (*Partition, bool) {
	n := input.size
	numbers := []int64{}
	var total int64
	for i := 0; i < n; i++ {
		text, _ := arrayGet(input, i)
		value, _ := strconv.ParseInt(text, 10, 64)
		if value > target {
			return nil, false
		}
		numbers = append(numbers, value)
		total += value
	}
	if target <= 0 || total%target != 0 {
		return nil, false
	}
	k := int(total / target)
	if k < 2 || k > n {
		return nil, false
	}

	sums := make([]int64, k)
	basket := make([]int, n)
	if !place(numbers, 0, target, sums, basket) {
		return nil, false
	}

	result := &Partition{count: k}
	for b := 0; b < k; b++ {
		result.groups = append(result.groups, newArray())
	}
	for i := 0; i < n; i++ {
		text, _ := arrayGet(input, i)
		arrayPush(result.groups[basket[i]], text)
	}
	return result, true
}

func printPartition(p *Partition) {
	parts := []string{}
	for b := 0; b < p.count; b++ {
		parts = append(parts, "{"+strings.Join(arrayValues(p.groups[b]), ", ")+"}")
	}
	fmt.Println(strings.Join(parts, " "))
}
