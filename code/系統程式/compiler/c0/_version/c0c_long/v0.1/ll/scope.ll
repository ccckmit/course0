; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 1, ptr %v0
  %v1 = alloca i32
  store i32 2, ptr %v1
  %0 = getelementptr [10 x i8], ptr @.str.0, i32 0, i32 0
  %1 = load i32, ptr %v1
  %2 = call i32 (ptr, ...) @printf(ptr %0, i32 %1)
  %3 = getelementptr [10 x i8], ptr @.str.1, i32 0, i32 0
  %4 = load i32, ptr %v0
  %5 = call i32 (ptr, ...) @printf(ptr %3, i32 %4)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [10 x i8] c"inner=%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c"outer=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
