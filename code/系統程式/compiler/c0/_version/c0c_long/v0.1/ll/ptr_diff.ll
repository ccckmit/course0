; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca [10 x i32]
  %v1 = alloca ptr
  %0 = getelementptr [10 x i32], ptr %v0, i32 0, i32 0
  %1 = mul i32 5, 4
  %2 = getelementptr i8, ptr %0, i32 %1
  store ptr %2, ptr %v1
  %v2 = alloca ptr
  %3 = getelementptr [10 x i32], ptr %v0, i32 0, i32 0
  %4 = mul i32 2, 4
  %5 = getelementptr i8, ptr %3, i32 %4
  store ptr %5, ptr %v2
  %v3 = alloca i32
  %6 = load ptr, ptr %v1
  %7 = load ptr, ptr %v2
  %8 = ptrtoint ptr %6 to i64
  %9 = ptrtoint ptr %7 to i64
  %10 = sub i64 %8, %9
  %11 = sdiv i64 %10, 4
  %12 = trunc i64 %11 to i32
  store i32 %12, ptr %v3
  %v4 = alloca i32
  %13 = load ptr, ptr %v1
  %14 = load ptr, ptr %v2
  %15 = ptrtoint ptr %13 to i64
  %16 = ptrtoint ptr %14 to i64
  %17 = icmp sgt i64 %15, %16
  %18 = zext i1 %17 to i32
  store i32 %18, ptr %v4
  %v5 = alloca i32
  %19 = load ptr, ptr %v1
  %20 = load ptr, ptr %v2
  %21 = ptrtoint ptr %19 to i64
  %22 = ptrtoint ptr %20 to i64
  %23 = icmp sge i64 %21, %22
  %24 = zext i1 %23 to i32
  store i32 %24, ptr %v5
  %v6 = alloca i32
  %25 = load ptr, ptr %v2
  %26 = load ptr, ptr %v1
  %27 = ptrtoint ptr %25 to i64
  %28 = ptrtoint ptr %26 to i64
  %29 = icmp slt i64 %27, %28
  %30 = zext i1 %29 to i32
  store i32 %30, ptr %v6
  %31 = getelementptr [24 x i8], ptr @.str.0, i32 0, i32 0
  %32 = load i32, ptr %v3
  %33 = load i32, ptr %v4
  %34 = load i32, ptr %v5
  %35 = load i32, ptr %v6
  %36 = call i32 (ptr, ...) @printf(ptr %31, i32 %32, i32 %33, i32 %34, i32 %35)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [24 x i8] c"d=%d c1=%d c2=%d c3=%d\0A\00", align 1
declare i32 @printf(ptr, ...)
