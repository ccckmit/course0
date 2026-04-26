# 類別方法與屬性
class Counter:
    def __init__(self):
        self.count = 0

    def increment(self):
        self.count = self.count + 1

    def decrement(self):
        self.count = self.count - 1

    def value(self):
        return self.count

c = Counter()
c.increment()
c.increment()
c.increment()
c.decrement()
print(c.value())

class Stack:
    def __init__(self):
        self.items = []

    def push(self, item):
        self.items.append(item)

    def pop(self):
        return self.items.pop()

    def peek(self):
        return self.items[-1]

    def is_empty(self):
        return len(self.items) == 0

    def size(self):
        return len(self.items)

s = Stack()
print(s.is_empty())
s.push(1)
s.push(2)
s.push(3)
print(s.size())
print(s.peek())
print(s.pop())
print(s.size())
