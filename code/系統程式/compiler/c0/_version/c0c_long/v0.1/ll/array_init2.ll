; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [2 x i32]
  %0 = getelementptr [2 x i32], ptr %v0, i32 0, i32 0
  store i32 1, ptr %0
  %1 = getelementptr [2 x i32], ptr %v0, i32 0, i32 1
  store i32 2, ptr %1
  %v1 = alloca [4 x i32]
  %2 = getelementptr [4 x i32], ptr %v1, i32 0, i32 0
  store i32 5, ptr %2
  %3 = getelementptr [4 x i32], ptr %v1, i32 0, i32 1
  store i32 0, ptr %3
  %4 = getelementptr [4 x i32], ptr %v1, i32 0, i32 2
  store i32 0, ptr %4
  %5 = getelementptr [4 x i32], ptr %v1, i32 0, i32 3
  store i32 0, ptr %5
  %v2 = alloca [3 x i8]
  %6 = getelementptr [3 x i8], ptr %v2, i32 0, i32 0
  store i8 104, ptr %6
  %7 = getelementptr [3 x i8], ptr %v2, i32 0, i32 1
  store i8 105, ptr %7
  %8 = getelementptr [3 x i8], ptr %v2, i32 0, i32 2
  store i8 0, ptr %8
  %9 = getelementptr [19 x i8], ptr @.str.0, i32 0, i32 0
  %10 = getelementptr [2 x i32], ptr %v0, i32 0, i32 0
  %11 = mul i32 1, 4
  %12 = getelementptr i8, ptr %10, i32 %11
  %13 = load i32, ptr %12
  %14 = getelementptr [4 x i32], ptr %v1, i32 0, i32 0
  %15 = mul i32 3, 4
  %16 = getelementptr i8, ptr %14, i32 %15
  %17 = load i32, ptr %16
  %18 = getelementptr [3 x i8], ptr %v2, i32 0, i32 0
  %19 = getelementptr i8, ptr %18, i32 2
  %20 = load i8, ptr %19
  %21 = sext i8 %20 to i32
  %22 = call i32 (ptr, ...) @printf(ptr %9, i32 %13, i32 %17, i32 %21)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [19 x i8] c"a1=%d b3=%d s2=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
