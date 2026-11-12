package main

type DNode struct {
	value string
	prev  *DNode
	next  *DNode
}

type DList struct {
	head *DNode
	tail *DNode
}

func dAddHead(l *DList, value string) {
	node := &DNode{value: value, next: l.head}
	if l.head != nil {
		l.head.prev = node
	} else {
		l.tail = node
	}
	l.head = node
}

func dDelTail(l *DList) (string, bool) {
	if l.tail == nil {
		return "", false
	}
	node := l.tail
	l.tail = node.prev
	if l.tail != nil {
		l.tail.next = nil
	} else {
		l.head = nil
	}
	return node.value, true
}

func dMoveToHead(l *DList, node *DNode) {
	if node == l.head {
		return
	}
	node.prev.next = node.next
	if node.next != nil {
		node.next.prev = node.prev
	} else {
		l.tail = node.prev
	}
	node.prev = nil
	node.next = l.head
	l.head.prev = node
	l.head = node
}
