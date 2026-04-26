; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 0, ptr %v0
  %v1 = alloca i32
  store i32 0, ptr %v1
  br label %L0
L0:
  %0 = load i32, ptr %v0
  %1 = icmp slt i32 %0, 10
  br i1 %1, label %L1, label %L2
L1:
  %2 = load i32, ptr %v0
  %3 = add i32 %2, 1
  store i32 %3, ptr %v0
  %4 = load i32, ptr %v0
  %5 = icmp eq i32 %4, 5
  br i1 %5, label %L3, label %L5
L3:
  br label %L0
L5:
  %6 = load i32, ptr %v0
  %7 = icmp eq i32 %6, 8
  br i1 %7, label %L6, label %L8
L6:
  br label %L2
L8:
  %8 = load i32, ptr %v1
  %9 = load i32, ptr %v0
  %10 = add i32 %8, %9
  store i32 %10, ptr %v1
  br label %L0
L2:
  %11 = getelementptr [8 x i8], ptr @.str.0, i32 0, i32 0
  %12 = load i32, ptr %v1
  %13 = call i32 (ptr, ...) @printf(ptr %11, i32 %12)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [8 x i8] c"sum=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
