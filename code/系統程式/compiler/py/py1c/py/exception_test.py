def safe_div(a, b):
    try:
        result = a / b
        return result
    except ZeroDivisionError:
        print("Error: division by zero")
        return None

print(safe_div(10, 2))
print(safe_div(10, 0))

def check_positive(n):
    if n < 0:
        raise ValueError("must be positive")
    return n * 2

try:
    print(check_positive(5))
    print(check_positive(-1))
except ValueError as e:
    print("Caught:", e)
