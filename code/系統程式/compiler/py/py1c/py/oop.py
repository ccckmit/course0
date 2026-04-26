# py/oop.py
class BankAccount:
    def __init__(self, owner, balance):
        self.owner = owner
        self.balance = balance

    def deposit(self, amount):
        self.balance = self.balance + amount
        print(f"{self.owner} 存入了 {amount} 元，目前餘額：{self.balance}")

    def withdraw(self, amount):
        if self.balance >= amount:
            self.balance = self.balance - amount
            print(f"{self.owner} 提出了 {amount} 元，目前餘額：{self.balance}")
        else:
            print(f"{self.owner} 餘額不足！欲提款 {amount} 元，但只有 {self.balance} 元。")

# 1. 建立帳戶
account1 = BankAccount("小明", 1000)
account2 = BankAccount("小華", 500)

# 2. 呼叫方法，測試狀態變更
account1.deposit(500)
account1.withdraw(200)
account1.withdraw(2000)  # 測試餘額不足

print("---")

account2.deposit(100)
account2.withdraw(600)

print(f"最終狀態 - {account1.owner}: {account1.balance}, {account2.owner}: {account2.balance}")