; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [5 x i8]
  %v1 = alloca ptr
  store ptr %v0, ptr %v1
  %0 = getelementptr i8, ptr %v0, i32 0
  store i32 7, ptr %0
  %1 = getelementptr i8, ptr %v0, i32 4
  %2 = trunc i32 9 to i8
  store i8 %2, ptr %1
  %3 = getelementptr [11 x i8], ptr @.str.0, i32 0, i32 0
  %4 = load ptr, ptr %v1
  %5 = getelementptr i8, ptr %4, i32 0
  %6 = load i32, ptr %5
  %7 = getelementptr i8, ptr %v0, i32 4
  %8 = load i8, ptr %7
  %9 = sext i8 %8 to i32
  %10 = call i32 (ptr, ...) @printf(ptr %3, i32 %6, i32 %9)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [11 x i8] c"x=%d y=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
