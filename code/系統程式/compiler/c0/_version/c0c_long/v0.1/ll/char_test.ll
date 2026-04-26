; ModuleID = 'c0c'
define i8 @inc(i8 %c) {
entry:
  %v0 = alloca i8
  store i8 %c, ptr %v0
  %0 = load i8, ptr %v0
  %1 = sext i8 %0 to i32
  %2 = add i32 %1, 1
  %3 = trunc i32 %2 to i8
  ret i8 %3
}

define i32 @main() {
entry:
  %v0 = alloca i8
  %0 = trunc i32 65 to i8
  store i8 %0, ptr %v0
  %v1 = alloca i8
  %1 = load i8, ptr %v0
  %2 = call i8 @inc(i8 %1)
  store i8 %2, ptr %v1
  %v2 = alloca i32
  %3 = load i8, ptr %v0
  %4 = sext i8 %3 to i32
  %5 = load i8, ptr %v1
  %6 = sext i8 %5 to i32
  %7 = add i32 %4, %6
  store i32 %7, ptr %v2
  %8 = getelementptr [16 x i8], ptr @.str.0, i32 0, i32 0
  %9 = load i8, ptr %v0
  %10 = load i8, ptr %v1
  %11 = load i32, ptr %v2
  %12 = sext i8 %9 to i32
  %13 = sext i8 %10 to i32
  %14 = call i32 (ptr, ...) @printf(ptr %8, i32 %12, i32 %13, i32 %11)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [16 x i8] c"c=%d d=%d x=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
