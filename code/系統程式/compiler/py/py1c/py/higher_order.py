def apply(func, lst):
    result = []
    for x in lst:
        result.append(func(x))
    return result

def double(x):
    return x * 2

def square(x):
    return x * x

nums = [1, 2, 3, 4, 5]
print(apply(double, nums))
print(apply(square, nums))

# filter-like
def keep_if(pred, lst):
    result = []
    for x in lst:
        if pred(x):
            result.append(x)
    return result

def is_even(x):
    return x % 2 == 0

print(keep_if(is_even, nums))
