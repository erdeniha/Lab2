package main

type Node struct {
	value string
	next  *Node
}

type Stack struct {
	head *Node
}

func stackPush(s *Stack, value string) {
	s.head = &Node{value: value, next: s.head}
}

func stackPop(s *Stack) (string, bool) {
	if s.head == nil {
		return "", false
	}
	value := s.head.value
	s.head = s.head.next
	return value, true
}

func stackPeek(s *Stack) (string, bool) {
	if s.head == nil {
		return "", false
	}
	return s.head.value, true
}
