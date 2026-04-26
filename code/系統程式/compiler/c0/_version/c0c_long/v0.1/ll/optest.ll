; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca i32
  store i32 0, ptr %v0
  %v1 = alloca i32
  store i32 0, ptr %v1
  br label %L0
L0:
  %0 = alloca i1
  %1 = load i32, ptr %v0
  %2 = icmp slt i32 %1, 5
  br i1 %2, label %L3, label %L4
L3:
  %3 = load i32, ptr %v1
  %4 = icmp slt i32 %3, 10
  store i1 %4, ptr %0
  br label %L5
L4:
  store i1 0, ptr %0
  br label %L5
L5:
  %5 = load i1, ptr %0
  br i1 %5, label %L1, label %L2
L1:
  %6 = load i32, ptr %v1
  %7 = load i32, ptr %v0
  %8 = add i32 %6, %7
  store i32 %8, ptr %v1
  %9 = load i32, ptr %v0
  %10 = add i32 %9, 1
  store i32 %10, ptr %v0
  br label %L0
L2:
  %v2 = alloca i32
  store i32 3, ptr %v2
  store i32 3, ptr %v2
  br label %L6
L6:
  %11 = load i32, ptr %v2
  %12 = icmp sgt i32 %11, 0
  br i1 %12, label %L7, label %L9
L7:
  %13 = load i32, ptr %v1
  %14 = load i32, ptr %v2
  %15 = add i32 %13, %14
  store i32 %15, ptr %v1
  br label %L8
L8:
  %16 = load i32, ptr %v2
  %17 = sub i32 %16, 1
  store i32 %17, ptr %v2
  br label %L6
L9:
  %18 = alloca i1
  %19 = load i32, ptr %v1
  %20 = srem i32 %19, 2
  %21 = icmp eq i32 %20, 0
  %22 = xor i1 %21, 1
  br i1 %22, label %L11, label %L10
L10:
  %23 = load i32, ptr %v1
  %24 = icmp eq i32 %23, 0
  store i1 %24, ptr %18
  br label %L12
L11:
  store i1 1, ptr %18
  br label %L12
L12:
  %25 = load i1, ptr %18
  br i1 %25, label %L13, label %L15
L13:
  %26 = getelementptr [8 x i8], ptr @.str.0, i32 0, i32 0
  %27 = load i32, ptr %v1
  %28 = call i32 (ptr, ...) @printf(ptr %26, i32 %27)
  br label %L15
L15:
  ret i32 0
}

@.str.0 = private unnamed_addr constant [8 x i8] c"sum=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
