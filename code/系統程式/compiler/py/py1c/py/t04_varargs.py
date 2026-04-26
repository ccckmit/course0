# *args
def sum_all(*args):
    total = 0
    for x in args:
        total = total + x
    return total

print(sum_all(1, 2, 3))
print(sum_all(10, 20, 30, 40))
print(sum_all())
