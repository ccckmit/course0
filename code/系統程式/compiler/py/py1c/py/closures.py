def make_counter(start):
    count = [start]
    def increment():
        count[0] = count[0] + 1
        return count[0]
    return increment

c1 = make_counter(0)
c2 = make_counter(10)
print(c1())
print(c1())
print(c2())
print(c1())
