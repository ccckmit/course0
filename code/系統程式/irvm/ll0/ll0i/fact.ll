; ModuleID = 'c0c'
define i32 @factorial(i32 %n) {
entry:
  %v0 = alloca i32
  store i32 %n, ptr %v0
  %0 = load i32, ptr %v0
  %1 = icmp eq i32 %0, 0
  br i1 %1, label %L0, label %L2
L0:
  ret i32 1
L2:
  %2 = load i32, ptr %v0
  %3 = load i32, ptr %v0
  %4 = sub i32 %3, 1
  %5 = call i32 @factorial(i32 %4)
  %6 = mul i32 %2, %5
  ret i32 %6
}

define i32 @main() {
entry:
  %0 = call i32 @factorial(i32 5)
  ret i32 %0
}

declare i32 @printf(ptr, ...)
