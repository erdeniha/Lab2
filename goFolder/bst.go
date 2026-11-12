package main

import (
	"fmt"
	"strconv"
	"strings"
)

func bstInsert(t *Tree, value int64) bool {
	place := &t.root
	for *place != nil {
		current, _ := strconv.ParseInt((*place).value, 10, 64)
		if value == current {
			return false
		}
		if value < current {
			place = &(*place).left
		} else {
			place = &(*place).right
		}
	}
	*place = &TreeNode{value: strconv.FormatInt(value, 10)}
	t.size++
	return true
}

func rightEdge(n *TreeNode, depth int, result *Array) {
	if n == nil {
		return
	}
	if depth == result.size {
		arrayPush(result, n.value)
	}
	rightEdge(n.right, depth+1, result)
	rightEdge(n.left, depth+1, result)
}

func bstRightEdge(t *Tree, result *Array) {
	rightEdge(t.root, 0, result)
}

func printBstSideways(n *TreeNode, depth int) {
	if n == nil {
		return
	}
	printBstSideways(n.right, depth+1)
	fmt.Println(strings.Repeat(" ", depth*4) + n.value)
	printBstSideways(n.left, depth+1)
}

func printBst(t *Tree) {
	if t.root == nil {
		fmt.Println("(Пустое дерево)")
		return
	}
	fmt.Println("Дерево:")
	printBstSideways(t.root, 0)
}
