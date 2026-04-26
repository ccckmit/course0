# range 迭代
total = 0
for i in range(1, 11):
    total = total + i
print("1~10 sum:", total)

# enumerate
fruits = ["apple", "banana", "cherry"]
for i, f in enumerate(fruits):
    print(i, f)

# zip
names = ["Alice", "Bob", "Carol"]
scores = [95, 87, 92]
for n, s in zip(names, scores):
    print(n, "->", s)
