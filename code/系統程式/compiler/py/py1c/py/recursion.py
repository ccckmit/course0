def quicksort(arr):
    if len(arr) <= 1:
        return arr
    pivot = arr[0]
    left = []
    right = []
    for x in arr[1:]:
        if x <= pivot:
            left.append(x)
        else:
            right.append(x)
    return quicksort(left) + [pivot] + quicksort(right)

data = [3, 6, 8, 10, 1, 2, 1]
print(quicksort(data))

def flatten(lst):
    result = []
    for item in lst:
        if isinstance(item, list):
            for x in flatten(item):
                result.append(x)
        else:
            result.append(item)
    return result

nested = [1, [2, [3, 4], 5], [6, 7]]
print(flatten(nested))
