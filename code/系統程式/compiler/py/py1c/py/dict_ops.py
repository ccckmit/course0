scores = {}
scores["Alice"] = 95
scores["Bob"] = 87
scores["Carol"] = 92

for name in scores:
    print(name, ":", scores[name])

print("has Alice?", "Alice" in scores)
print("has Dave?", "Dave" in scores)
print("total keys:", len(scores))
