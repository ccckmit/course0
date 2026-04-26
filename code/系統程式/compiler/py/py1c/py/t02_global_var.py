# 全域變數讀寫
total = 0

def add(n):
    global total
    total = total + n

add(10)
add(20)
add(5)
print(total)
