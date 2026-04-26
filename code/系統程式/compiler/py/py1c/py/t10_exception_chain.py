# 多層 exception 處理
def level3():
    raise ValueError("deep error")

def level2():
    try:
        level3()
    except ValueError as e:
        print("level2 caught:", e)
        raise RuntimeError("wrapped")

def level1():
    try:
        level2()
    except RuntimeError as e:
        print("level1 caught:", e)

level1()

# exception in loop
errors = 0
for i in range(5):
    try:
        if i == 2:
            raise IndexError("bad index")
        print("ok:", i)
    except IndexError:
        errors = errors + 1
        print("error at:", i)

print("total errors:", errors)
