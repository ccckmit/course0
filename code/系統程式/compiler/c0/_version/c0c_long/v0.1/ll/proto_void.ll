; ModuleID = 'c0c'
define void @hello() {
entry:
  %0 = getelementptr [4 x i8], ptr @.str.0, i32 0, i32 0
  %1 = call i32 (ptr, ...) @printf(ptr %0)
  ret void
}

define i32 @add(i32 %a, i32 %b) {
entry:
  %v0 = alloca i32
  store i32 %a, ptr %v0
  %v1 = alloca i32
  store i32 %b, ptr %v1
  %v2 = alloca i32
  store i32 %b, ptr %v2
  %0 = load i32, ptr %v0
  %1 = load i32, ptr %v2
  %2 = add i32 %0, %1
  ret i32 %2
}

define i32 @main() {
entry:
  call void @hello()
  %v0 = alloca i32
  %0 = call i32 @add(i32 3, i32 4)
  store i32 %0, ptr %v0
  %1 = getelementptr [6 x i8], ptr @.str.1, i32 0, i32 0
  %2 = load i32, ptr %v0
  %3 = call i32 (ptr, ...) @printf(ptr %1, i32 %2)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [4 x i8] c"hi\0A\00", align 1
@.str.1 = private unnamed_addr constant [6 x i8] c"v=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
