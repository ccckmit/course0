; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [3 x i32]
  %0 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  %1 = mul i32 0, 4
  %2 = getelementptr i8, ptr %0, i32 %1
  store i32 10, ptr %2
  %3 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  %4 = mul i32 1, 4
  %5 = getelementptr i8, ptr %3, i32 %4
  store i32 20, ptr %5
  %6 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  %7 = mul i32 2, 4
  %8 = getelementptr i8, ptr %6, i32 %7
  store i32 30, ptr %8
  %v1 = alloca ptr
  %9 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  %10 = mul i32 0, 4
  %11 = getelementptr i8, ptr %9, i32 %10
  store ptr %11, ptr %v1
  %v2 = alloca i32
  %12 = load ptr, ptr %v1
  %13 = mul i32 1, 4
  %14 = getelementptr i8, ptr %12, i32 %13
  %15 = load i32, ptr %14
  store i32 %15, ptr %v2
  %v3 = alloca i32
  %16 = load ptr, ptr %v1
  %17 = mul i32 2, 4
  %18 = getelementptr i8, ptr %16, i32 %17
  %19 = load i32, ptr %18
  store i32 %19, ptr %v3
  %20 = getelementptr [11 x i8], ptr @.str.0, i32 0, i32 0
  %21 = load i32, ptr %v2
  %22 = load i32, ptr %v3
  %23 = call i32 (ptr, ...) @printf(ptr %20, i32 %21, i32 %22)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [11 x i8] c"x=%d y=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
