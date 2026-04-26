; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [4 x i8]
  %0 = getelementptr i8, ptr %v0, i32 0
  store i32 3, ptr %0
  %1 = getelementptr [6 x i8], ptr @.str.0, i32 0, i32 0
  %2 = getelementptr i8, ptr %v0, i32 0
  %3 = load i32, ptr %2
  %4 = call i32 (ptr, ...) @printf(ptr %1, i32 %3)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [6 x i8] c"v=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
