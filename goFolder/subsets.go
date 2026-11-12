package main

import "sort"

func buildSubsets(items *Array, start, size int, picked []int, seen *Set, result *Array) {
	if len(picked) == size {
		chosen := []string{}
		text := "{"
		for i := 0; i < len(picked); i++ {
			item, _ := arrayGet(items, picked[i])
			chosen = append(chosen, item)
			if i > 0 {
				text += ", "
			}
			text += item
		}
		text += "}"
		sort.Strings(chosen)
		key := ""
		for i := 0; i < len(chosen); i++ {
			key += chosen[i] + "\x1f"
		}
		if setAdd(seen, key) {
			arrayPush(result, text)
		}
		return
	}
	n := items.size
	still := size - len(picked)
	for i := start; i <= n-still; i++ {
		buildSubsets(items, i+1, size, append(picked, i), seen, result)
	}
}

func allSubsets(items *Array, result *Array) {
	seen := newSet()
	for size := 0; size <= items.size; size++ {
		buildSubsets(items, 0, size, []int{}, seen, result)
	}
}
