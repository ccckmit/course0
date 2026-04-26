# dict 方法
d = {"a": 1, "b": 2, "c": 3}
print(sorted(d.keys()))
print(sorted(d.values()))
print(sorted(d.items()))
d.update({"d": 4, "a": 10})
print(d.get("a"))
print(d.get("z", -1))
del d["b"]
print(sorted(d.keys()))
