; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 10, ptr %v0
  %v1 = alloca ptr
  store ptr %v0, ptr %v1
  %0 = load ptr, ptr %v1
  %1 = load ptr, ptr %v1
  %2 = load i32, ptr %1
  %3 = add i32 %2, 5
  store i32 %3, ptr %0
  %v2 = alloca [3 x i32]
  %4 = getelementptr [3 x i32], ptr %v2, i32 0, i32 0
  %5 = mul i32 0, 4
  %6 = getelementptr i8, ptr %4, i32 %5
  store i32 1, ptr %6
  %7 = getelementptr [3 x i32], ptr %v2, i32 0, i32 0
  %8 = mul i32 1, 4
  %9 = getelementptr i8, ptr %7, i32 %8
  store i32 2, ptr %9
  %10 = getelementptr [3 x i32], ptr %v2, i32 0, i32 0
  %11 = mul i32 2, 4
  %12 = getelementptr i8, ptr %10, i32 %11
  store i32 3, ptr %12
  %v3 = alloca i32
  %13 = getelementptr [3 x i32], ptr %v2, i32 0, i32 0
  %14 = mul i32 1, 4
  %15 = getelementptr i8, ptr %13, i32 %14
  %16 = load i32, ptr %15
  store i32 %16, ptr %v3
  %17 = getelementptr [11 x i8], ptr @.str.0, i32 0, i32 0
  %18 = load i32, ptr %v0
  %19 = load i32, ptr %v3
  %20 = call i32 (ptr, ...) @printf(ptr %17, i32 %18, i32 %19)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [11 x i8] c"x=%d y=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
