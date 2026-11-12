package main

type TreeNode struct {
	value string
	left  *TreeNode
	right *TreeNode
}

type Tree struct {
	root *TreeNode
	size int
}
