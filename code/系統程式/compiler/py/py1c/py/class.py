class Student:
    # 1. 建構子：初始化學生的屬性
    def __init__(self, name, grades):
        self.name = name          # 姓名 (字串)
        self.grades = grades      # 成績 (串列)

    # 2. 方法：計算該學生的平均成績
    def get_average(self):
        if not self.grades:
            return 0
        return sum(self.grades) / len(self.grades)

    # 3. 方法：自我介紹
    def introduce(self):
        avg = self.get_average()
        print(f"你好，我是 {self.name}，我的平均成績是 {avg:.1f} 分。")

# --- 實際使用範例 ---

# 建立物件 (實例化)
student_a = Student("小明", [85, 90, 78])
student_b = Student("小華", [92, 88, 95])

# 呼叫物件的方法
student_a.introduce()
student_b.introduce()

# 也可以直接存取屬性
print(f"直接查詢：{student_a.name} 考了 {len(student_a.grades)} 門科目。")