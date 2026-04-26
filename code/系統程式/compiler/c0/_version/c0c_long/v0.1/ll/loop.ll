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
  %1 = icmp slt i32 %0, 5
  br i1 %1, label %L1, label %L2
L1:
  %2 = load i32, ptr %v1
  %3 = load i32, ptr %v0
  %4 = add i32 %2, %3
  store i32 %4, ptr %v1
  %5 = load i32, ptr %v0
  %6 = add i32 %5, 1
  store i32 %6, ptr %v0
  br label %L0
L2:
  %v2 = alloca i32
  %v3 = alloca i32
  store i32 1, ptr %v3
  store i32 1, ptr %v2
  br label %L3
L3:
  %7 = load i32, ptr %v2
  %8 = icmp sle i32 %7, 4
  br i1 %8, label %L4, label %L6
L4:
  %9 = load i32, ptr %v3
  %10 = load i32, ptr %v2
  %11 = mul i32 %9, %10
  store i32 %11, ptr %v3
  br label %L5
L5:
  %12 = load i32, ptr %v2
  %13 = add i32 %12, 1
  store i32 %13, ptr %v2
  br label %L3
L6:
  %14 = getelementptr [16 x i8], ptr @.str.0, i32 0, i32 0
  %15 = load i32, ptr %v1
  %16 = load i32, ptr %v3
  %17 = call i32 (ptr, ...) @printf(ptr %14, i32 %15, i32 %16)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [16 x i8] c"sum=%d prod=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
