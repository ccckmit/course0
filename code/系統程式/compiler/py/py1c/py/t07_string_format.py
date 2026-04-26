# 字串格式化
name = "World"
n = 42
pi = 3.14159

print("Hello, " + name + "!")
print("n = " + str(n))
print("pi ~ " + str(round(pi, 2)))

# format()
print("{}+{}={}".format(1, 2, 3))
print("{name} is {age}".format(name="Alice", age=30))
