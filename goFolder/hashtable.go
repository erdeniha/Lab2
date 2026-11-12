package main

import (
	"fmt"
	"strconv"
)

type ProbeMode int

const (
	ProbeLinear ProbeMode = iota
	ProbeDouble
)

type SlotState int

const (
	SlotEmpty SlotState = iota
	SlotUsed
	SlotDeleted
)

type InsertResult int

const (
	HtAdded InsertResult = iota
	HtUpdated
	HtFull
)

type Slot struct {
	key   string
	value string
	node  *DNode
	state SlotState
}

type HashTable struct {
	slots    []Slot
	capacity int
	used     int // сколько занято
	deleted  int // сколько могил
	mode     ProbeMode
	autoGrow bool  // расширять таблицу при заполнении больше 70%
	probes   int64 // сколько всего слотов просмотрено
}

func allDigits(key string) bool {
	if key == "" {
		return false
	}
	for i := 0; i < len(key); i++ {
		if key[i] < '0' || key[i] > '9' {
			return false
		}
	}
	return true
}

func foldKey(key string, groupSize int) int64 {
	numeric := allDigits(key)
	var base int64 = 256
	if numeric {
		base = 10
	}
	var sum, part int64
	length := 0
	for i := 0; i < len(key); i++ {
		digit := int64(key[i])
		if numeric {
			digit = int64(key[i] - '0')
		}
		part = part*base + digit
		length++
		if length == groupSize {
			sum += part
			part = 0
			length = 0
		}
	}
	return sum + part
}

func foldParts(key string, groupSize int) string {
	text := ""
	for i := 0; i < len(key); i += groupSize {
		if i > 0 {
			text += "+"
		}
		end := i + groupSize
		if end > len(key) {
			end = len(key)
		}
		text += key[i:end]
	}
	return text
}

func nextPrime(n int) int {
	if n < 2 {
		n = 2
	}
	for {
		prime := true
		for d := 2; d*d <= n; d++ {
			if n%d == 0 {
				prime = false
				break
			}
		}
		if prime {
			return n
		}
		n++
	}
}

func newHashTable(mode ProbeMode, capacity int, autoGrow bool) *HashTable {
	if capacity < 5 {
		capacity = 5
	}
	capacity = nextPrime(capacity)
	return &HashTable{slots: make([]Slot, capacity), capacity: capacity, mode: mode, autoGrow: autoGrow}
}

func locate(t *HashTable, key string) (int, bool) {
	start := foldKey(key, 3) % int64(t.capacity)
	var step int64 = 1
	if t.mode == ProbeDouble {
		step = 1 + foldKey(key, 2)%int64(t.capacity-1)
	}

	firstDeleted := -1
	for i := 0; i < t.capacity; i++ {
		index := int((start + int64(i)*step) % int64(t.capacity))
		t.probes++
		slot := &t.slots[index]
		if slot.state == SlotEmpty {
			if firstDeleted >= 0 {
				return firstDeleted, false
			}
			return index, false
		}
		if slot.state == SlotDeleted {
			if firstDeleted < 0 {
				firstDeleted = index
			}
		} else if slot.key == key {
			return index, true
		}
	}
	return firstDeleted, false
}

func rehash(t *HashTable) {
	newCapacity := t.capacity
	if (t.used+1)*20 > t.capacity*7 {
		newCapacity = t.capacity * 2
	}
	bigger := newHashTable(t.mode, newCapacity, t.autoGrow)
	for i := 0; i < t.capacity; i++ {
		if t.slots[i].state == SlotUsed {
			index, _ := locate(bigger, t.slots[i].key)
			bigger.slots[index] = t.slots[i]
			bigger.used++
		}
	}
	bigger.probes = t.probes
	*t = *bigger
}

func htInsert(t *HashTable, key, value string, node *DNode) InsertResult {
	index, found := locate(t, key)
	if found {
		t.slots[index].value = value
		t.slots[index].node = node
		return HtUpdated
	}
	if t.autoGrow && (t.used+t.deleted+1)*10 > t.capacity*7 {
		rehash(t)
		index, _ = locate(t, key)
	}
	if index < 0 {
		return HtFull
	}
	slot := &t.slots[index]
	if slot.state == SlotDeleted {
		t.deleted--
	}
	slot.key = key
	slot.value = value
	slot.node = node
	slot.state = SlotUsed
	t.used++
	return HtAdded
}

func htFind(t *HashTable, key string) *Slot {
	index, found := locate(t, key)
	if !found {
		return nil
	}
	return &t.slots[index]
}

func htErase(t *HashTable, key string) bool {
	index, found := locate(t, key)
	if !found {
		return false
	}
	t.slots[index] = Slot{state: SlotDeleted}
	t.used--
	t.deleted++
	return true
}

func htSize(t *HashTable) int {
	return t.used
}

func printHashTable(t *HashTable) {
	name := "linear probing"
	if t.mode == ProbeDouble {
		name = "double hashing"
	}
	fmt.Printf("Table size = %d, used = %d, deleted = %d, %s\n", t.capacity, t.used, t.deleted, name)
	for i := 0; i < t.capacity; i++ {
		slot := &t.slots[i]
		if slot.state == SlotUsed {
			line := "  [" + strconv.Itoa(i) + "] " + slot.key
			if slot.value != "" {
				line += " -> " + slot.value
			}
			fmt.Println(line)
		}
	}
}
