; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  %v1 = alloca ptr
  %v2 = alloca i8
  %0 = getelementptr [16 x i8], ptr @.str.0, i32 0, i32 0
  %1 = call i32 (ptr, ...) @printf(ptr %0, i32 4, i32 1, i32 8)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [16 x i8] c"i=%d c=%d p=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
