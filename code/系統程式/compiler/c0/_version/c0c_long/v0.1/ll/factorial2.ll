; ModuleID = 'c0c'
define i32 @fact(i32 %n) {
entry:
  %n.addr = alloca i32
  store i32 %n, ptr %n.addr
  %0 = load i32, ptr %n.addr
  %1 = icmp sle i32 %0, 1
  br i1 %1, label %L0, label %L1
L0:
  ret i32 1
L1:
  %2 = load i32, ptr %n.addr
  %3 = load i32, ptr %n.addr
  %4 = sub i32 %3, 1
  %5 = call i32 @fact(i32 %4)
  %6 = mul i32 %2, %5
  ret i32 %6
}

define i32 @main() {
entry:
  %r = alloca i32
  %0 = call i32 @fact(i32 5)
  store i32 %0, ptr %r
  %1 = load i32, ptr %r
  %2 = call i32 (ptr, ...) @printf(ptr @.str.0, i32 %1)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [12 x i8] c"fact(5)=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
