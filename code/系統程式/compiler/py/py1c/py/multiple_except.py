def parse_int(s):
    try:
        return int(s)
    except ValueError:
        print("ValueError:", s, "is not an int")
        return None
    except TypeError:
        print("TypeError: got", type(s))
        return None

print(parse_int("42"))
print(parse_int("hello"))
print(parse_int(None))

# finally (skipped if not supported)
def read_data(key, d):
    try:
        val = d[key]
        return val * 2
    except KeyError:
        print("Key not found:", key)
        return 0

data = {"a": 10, "b": 20}
print(read_data("a", data))
print(read_data("c", data))
