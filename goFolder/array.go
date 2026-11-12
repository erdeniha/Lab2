package main

type Array struct {
	data []string
	size int
}

func newArray() *Array {
	return &Array{data: make([]string, 4)}
}

func arrayPush(a *Array, value string) {
	if a.size == len(a.data) {
		bigger := make([]string, len(a.data)*2)
		copy(bigger, a.data)
		a.data = bigger
	}
	a.data[a.size] = value
	a.size++
}

func arrayGet(a *Array, index int) (string, bool) {
	if index < 0 || index >= a.size {
		return "", false
	}
	return a.data[index], true
}

func arrayValues(a *Array) []string {
	return append([]string{}, a.data[:a.size]...)
}
