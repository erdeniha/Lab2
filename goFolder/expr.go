package main

import "strconv"

const limit int64 = 2000000000

func priority(op string) int {
	if op == "~" {
		return 3
	}
	if op == "*" {
		return 2
	}
	if op == "+" || op == "-" {
		return 1
	}
	return 0
}

func applyTop(numbers, operations *Stack) string {
	op, ok1 := stackPop(operations)
	right, ok2 := stackPop(numbers)
	if !ok1 || !ok2 {
		return "not enough operands"
	}
	b, _ := strconv.ParseInt(right, 10, 64)
	var value int64
	if op == "~" {
		value = -b
	} else {
		left, ok3 := stackPop(numbers)
		if !ok3 {
			return "not enough operands"
		}
		a, _ := strconv.ParseInt(left, 10, 64)
		switch op[0] {
		case '+':
			value = a + b
		case '-':
			value = a - b
		case '*':
			value = a * b
		}
	}
	if value > limit || value < -limit {
		return "value is out of range (more than 2*10^9 in absolute value)"
	}
	stackPush(numbers, strconv.FormatInt(value, 10))
	return ""
}

func calculate(text string, numbers, operations *Stack) (int64, string) {
	expectOperand := true
	i := 0
	for i < len(text) {
		c := text[i]
		where := " at position " + strconv.Itoa(i+1)

		if c == ' ' || c == '\t' {
			i++
		} else if c >= '0' && c <= '9' {
			if !expectOperand {
				return 0, "operator expected before number" + where
			}
			var number int64
			for i < len(text) && text[i] >= '0' && text[i] <= '9' {
				number = number*10 + int64(text[i]-'0')
				if number > limit {
					return 0, "number is out of range" + where
				}
				i++
			}
			stackPush(numbers, strconv.FormatInt(number, 10))
			expectOperand = false
		} else if c == '(' {
			if !expectOperand {
				return 0, "operator expected before '('" + where
			}
			stackPush(operations, "(")
			i++
		} else if c == ')' {
			if expectOperand {
				return 0, "operand expected before ')'" + where
			}
			for {
				top, ok := stackPeek(operations)
				if !ok || top == "(" {
					break
				}
				if msg := applyTop(numbers, operations); msg != "" {
					return 0, msg
				}
			}
			if _, ok := stackPop(operations); !ok {
				return 0, "unbalanced ')'" + where
			}
			i++
		} else if c == '+' || c == '-' || c == '*' {
			op := string(c)
			if expectOperand {
				if c == '*' {
					return 0, "operand expected before '*'" + where
				}

				if c == '-' {
					stackPush(operations, "~")
				}
			} else {
				for {
					top, ok := stackPeek(operations)
					if !ok || priority(top) < priority(op) {
						break
					}
					if msg := applyTop(numbers, operations); msg != "" {
						return 0, msg
					}
				}
				stackPush(operations, op)
				expectOperand = true
			}
			i++
		} else {
			return 0, "unexpected character '" + string(c) + "'" + where
		}
	}

	if expectOperand {
		empty := true
		for k := 0; k < len(text); k++ {
			if text[k] != ' ' && text[k] != '\t' {
				empty = false
			}
		}
		if empty {
			return 0, "empty expression"
		}
		return 0, "expression is incomplete"
	}
	for {
		top, ok := stackPeek(operations)
		if !ok {
			break
		}
		if top == "(" {
			return 0, "unbalanced '('"
		}
		if msg := applyTop(numbers, operations); msg != "" {
			return 0, msg
		}
	}
	answer, _ := stackPop(numbers)
	result, _ := strconv.ParseInt(answer, 10, 64)
	return result, ""
}

func evalExpression(text string) (int64, string) {
	numbers := &Stack{}
	operations := &Stack{}
	return calculate(text, numbers, operations)
}
