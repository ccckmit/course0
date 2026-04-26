nums = [3, 1, 4, 1, 5, 9, 2, 6]
total = 0
for n in nums:
    total = total + n
print("sum =", total)

squares = []
for n in nums:
    squares.append(n * n)
print("squares =", squares)

words = ["hello", "world", "qd0vm"]
result = ""
for w in words:
    result = result + w + " "
print("joined =", result.strip())
