; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 0, ptr %v0
  %v1 = alloca i32
  store i32 0, ptr %v1
  br label %L0
L0:
  %0 = load i32, ptr %v1
  %1 = load i32, ptr %v0
  %2 = add i32 %0, %1
  store i32 %2, ptr %v1
  %3 = load i32, ptr %v0
  %4 = add i32 %3, 1
  store i32 %4, ptr %v0
  br label %L1
L1:
  %5 = load i32, ptr %v0
  %6 = icmp slt i32 %5, 3
  br i1 %6, label %L0, label %L2
L2:
  %7 = getelementptr [8 x i8], ptr @.str.0, i32 0, i32 0
  %8 = load i32, ptr %v1
  %9 = call i32 (ptr, ...) @printf(ptr %7, i32 %8)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [8 x i8] c"sum=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
