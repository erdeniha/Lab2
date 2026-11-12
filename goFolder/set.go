package main

import "sort"

type Set struct {
	table *HashTable
}

func newSet() *Set {
	return &Set{table: newHashTable(ProbeDouble, 11, true)}
}

func setAdd(s *Set, value string) bool {
	return htInsert(s.table, value, "", nil) == HtAdded
}

func setDel(s *Set, value string) bool {
	return htErase(s.table, value)
}

func setAt(s *Set, value string) bool {
	return htFind(s.table, value) != nil
}

func setValues(s *Set) []string {
	values := []string{}
	for i := range s.table.slots {
		if s.table.slots[i].state == SlotUsed {
			values = append(values, s.table.slots[i].key)
		}
	}
	sort.Strings(values)
	return values
}
