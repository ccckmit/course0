; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i16
  %0 = trunc i32 65000 to i16
  store i16 %0, ptr %v0
  %v1 = alloca i16
  %1 = sub i32 0, 10
  %2 = trunc i32 %1 to i16
  store i16 %2, ptr %v1
  %v2 = alloca i64
  %3 = sext i32 100000 to i64
  store i64 %3, ptr %v2
  %v3 = alloca i64
  %4 = sext i32 -294967296 to i64
  store i64 %4, ptr %v3
  %v4 = alloca i32
  %5 = load i16, ptr %v0
  %6 = zext i16 %5 to i32
  %7 = load i16, ptr %v1
  %8 = sext i16 %7 to i32
  %9 = add i32 %6, %8
  store i32 %9, ptr %v4
  %v5 = alloca i64
  %10 = load i64, ptr %v2
  %11 = sext i32 5 to i64
  %12 = add i64 %10, %11
  store i64 %12, ptr %v5
  %v6 = alloca i64
  %13 = load i64, ptr %v3
  %14 = sext i32 5 to i64
  %15 = add i64 %13, %14
  store i64 %15, ptr %v6
  %16 = getelementptr [18 x i8], ptr @.str.0, i32 0, i32 0
  %17 = load i32, ptr %v4
  %18 = load i64, ptr %v5
  %19 = load i64, ptr %v6
  %20 = call i32 (ptr, ...) @printf(ptr %16, i32 %17, i64 %18, i64 %19)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [18 x i8] c"x=%d y=%ld z=%lu\0A\00", align 1
declare i32 @printf(ptr, ...)
