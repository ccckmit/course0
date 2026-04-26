; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [3 x i32]
  %0 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  store i32 1, ptr %0
  %1 = getelementptr [3 x i32], ptr %v0, i32 0, i32 1
  store i32 2, ptr %1
  %2 = getelementptr [3 x i32], ptr %v0, i32 0, i32 2
  store i32 3, ptr %2
  %v1 = alloca [4 x i8]
  %3 = getelementptr [4 x i8], ptr %v1, i32 0, i32 0
  store i8 97, ptr %3
  %4 = getelementptr [4 x i8], ptr %v1, i32 0, i32 1
  store i8 98, ptr %4
  %5 = getelementptr [4 x i8], ptr %v1, i32 0, i32 2
  store i8 99, ptr %5
  %6 = getelementptr [4 x i8], ptr %v1, i32 0, i32 3
  store i8 0, ptr %6
  %7 = getelementptr [19 x i8], ptr @.str.0, i32 0, i32 0
  %8 = getelementptr [3 x i32], ptr %v0, i32 0, i32 0
  %9 = mul i32 1, 4
  %10 = getelementptr i8, ptr %8, i32 %9
  %11 = load i32, ptr %10
  %12 = getelementptr [4 x i8], ptr %v1, i32 0, i32 0
  %13 = getelementptr i8, ptr %12, i32 0
  %14 = load i8, ptr %13
  %15 = getelementptr [4 x i8], ptr %v1, i32 0, i32 0
  %16 = getelementptr i8, ptr %15, i32 2
  %17 = load i8, ptr %16
  %18 = sext i8 %14 to i32
  %19 = sext i8 %17 to i32
  %20 = call i32 (ptr, ...) @printf(ptr %7, i32 %11, i32 %18, i32 %19)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [19 x i8] c"a1=%d s0=%d s2=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
