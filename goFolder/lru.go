package main

type LRUCache struct {
	capacity int
	table    *HashTable
	order    *DList
}

func newLRU(capacity int) *LRUCache {
	return &LRUCache{
		capacity: capacity,
		table:    newHashTable(ProbeDouble, 2*capacity+1, true),
		order:    &DList{},
	}
}

func lruSet(c *LRUCache, key, value string) {
	if slot := htFind(c.table, key); slot != nil {
		slot.value = value
		dMoveToHead(c.order, slot.node)
		return
	}
	if htSize(c.table) == c.capacity {
		oldest, _ := dDelTail(c.order)
		htErase(c.table, oldest)
	}
	dAddHead(c.order, key)
	htInsert(c.table, key, value, c.order.head)
}

func lruGet(c *LRUCache, key string) (string, bool) {
	slot := htFind(c.table, key)
	if slot == nil {
		return "", false
	}
	dMoveToHead(c.order, slot.node)
	return slot.value, true
}
