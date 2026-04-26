# 1. 定義一個「字典」的「串列」（陣列）
# 每個字典代表一位學生的詳細資料
students = [
    {
        "name": "小明",
        "age": 20,
        "grades": [85, 90, 78]  # 字典內也可以包含串列
    },
    {
        "name": "小華",
        "age": 22,
        "grades": [92, 88, 95]
    },
    {
        "name": "小美",
        "age": 21,
        "grades": [70, 85, 80]
    }
]

# 2. 新增一筆資料到串列中（操作陣列）
new_student = {"name": "阿強", "age": 23, "grades": [88, 76, 90]}
students.append(new_student)

# 3. 處理並印出資料
print(f"--- 學生成績報表 (共有 {len(students)} 位學生) ---")

for s in students:
    # 存取字典中的特定鍵 (Key)
    name = s["name"]
    age = s["age"]
    grades = s["grades"]
    
    # 計算平均成績 (使用數學公式概念)
    # 平均值 $\bar{x} = \frac{\sum_{i=1}^{n} x_i}{n}$
    average_score = sum(grades) / len(grades)
    
    # 印出結果，格式化小數點到第二位
    print(f"學生：{name} | 年齡：{age} | 平均分數：{average_score:.2f}")

# 4. 單獨存取特定的資料範例
print("\n--- 特定查詢 ---")
print(f"第一位學生的名字是：{students[0]['name']}")