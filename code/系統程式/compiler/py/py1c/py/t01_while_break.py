# while + break + continue
i = 0
result = []
while True:
    i = i + 1
    if i % 2 == 0:
        continue
    if i > 9:
        break
    result.append(i)
print(result)
