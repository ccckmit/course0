# 巢狀迴圈 + break/continue
result = []
for i in range(5):
    for j in range(5):
        if j == 3:
            break
        if i == 2 and j == 1:
            continue
        result.append(i * 10 + j)
print(result)

# 矩陣乘法
a = [[1, 2], [3, 4]]
b = [[5, 6], [7, 8]]
c = [[0, 0], [0, 0]]
for i in range(2):
    for j in range(2):
        for k in range(2):
            c[i][j] = c[i][j] + a[i][k] * b[k][j]
print(c)
