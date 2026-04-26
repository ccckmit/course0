; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca ptr
  %0 = getelementptr [4 x i8], ptr @.str.0, i32 0, i32 0
  store ptr %0, ptr %v0
  %1 = getelementptr [3 x i8], ptr @.str.1, i32 0, i32 0
  %2 = load ptr, ptr %v0
  %3 = call i32 (ptr, ...) @printf(ptr %1, ptr %2)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [4 x i8] c"hi\0A\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
declare i32 @printf(ptr, ...)
