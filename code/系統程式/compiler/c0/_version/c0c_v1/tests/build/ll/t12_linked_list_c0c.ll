; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t12_linked_list.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t12_linked_list.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

%struct.Node = type { i32, ptr }

define dso_local i32 @sum(ptr noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  store ptr %0, ptr %3, align 8
  %5 = add nsw i32 0, 0
  store i32 %5, ptr %4, align 4
  br label %while.cond.0

while.cond.0:
  %7 = load ptr, ptr %3, align 8
  %8 = icmp ne ptr %7, null
  br i1 %8, label %while.body.0, label %while.end.0

while.body.0:
  %10 = load i32, ptr %4, align 4
  %11 = load ptr, ptr %3, align 8
  %12 = getelementptr inbounds %struct.Node, ptr %11, i32 0, i32 0
  %13 = load i32, ptr %12, align 4
  %14 = add nsw i32 %10, %13
  store i32 %14, ptr %4, align 4
  %15 = load ptr, ptr %3, align 8
  %16 = getelementptr inbounds %struct.Node, ptr %15, i32 0, i32 1
  %17 = load ptr, ptr %16, align 8
  store ptr %17, ptr %3, align 8
  br label %while.cond.0

while.end.0:
  %19 = load i32, ptr %4, align 4
  store i32 %19, ptr %2, align 4
  br label %return

  br label %return

return:
  %22 = load i32, ptr %2, align 4
  ret i32 %22
}

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.Node, align 8
  %3 = alloca %struct.Node, align 8
  %4 = alloca %struct.Node, align 8
  store i32 0, ptr %1, align 4
  %5 = getelementptr inbounds %struct.Node, ptr %2, i32 0, i32 0
  %6 = add nsw i32 0, 1
  store i32 %6, ptr %5, align 4
  %7 = getelementptr inbounds %struct.Node, ptr %2, i32 0, i32 1
  store ptr %3, ptr %7, align 8
  %8 = getelementptr inbounds %struct.Node, ptr %3, i32 0, i32 0
  %9 = add nsw i32 0, 2
  store i32 %9, ptr %8, align 4
  %10 = getelementptr inbounds %struct.Node, ptr %3, i32 0, i32 1
  store ptr %4, ptr %10, align 8
  %11 = getelementptr inbounds %struct.Node, ptr %4, i32 0, i32 0
  %12 = add nsw i32 0, 3
  store i32 %12, ptr %11, align 4
  %13 = getelementptr inbounds %struct.Node, ptr %4, i32 0, i32 1
  store ptr null, ptr %13, align 8
  %14 = add nsw i32 0, 0
  %15 = call i32 @sum(ptr noundef %2)
  store i32 %15, ptr %1, align 4
  br label %return

  br label %return

return:
  %18 = load i32, ptr %1, align 4
  ret i32 %18
}

attributes #0 = { noinline nounwind optnone uwtable }
