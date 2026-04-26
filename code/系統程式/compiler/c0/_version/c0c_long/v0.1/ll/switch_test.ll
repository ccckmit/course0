; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 2, ptr %v0
  %v1 = alloca i32
  store i32 0, ptr %v1
  %0 = load i32, ptr %v0
  br label %L4
L4:
  %1 = icmp eq i32 %0, 1
  br i1 %1, label %L1, label %L5
L5:
  %2 = icmp eq i32 %0, 2
  br i1 %2, label %L2, label %L6
L6:
  br label %L3
L1:
  store i32 10, ptr %v1
  br label %L0
L2:
  store i32 20, ptr %v1
  br label %L0
L3:
  store i32 30, ptr %v1
  br label %L0
L0:
  %3 = getelementptr [6 x i8], ptr @.str.0, i32 0, i32 0
  %4 = load i32, ptr %v1
  %5 = call i32 (ptr, ...) @printf(ptr %3, i32 %4)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [6 x i8] c"y=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
