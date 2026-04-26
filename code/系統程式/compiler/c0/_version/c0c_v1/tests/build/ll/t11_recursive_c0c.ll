; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t11_recursive.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t11_recursive.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @fib(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  store i32 %0, ptr %3, align 4
  %4 = load i32, ptr %3, align 4
  %5 = add nsw i32 0, 1
  %6 = icmp sle i32 %4, %5
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if.then.0, label %if.end.0

if.then.0:
  %10 = load i32, ptr %3, align 4
  store i32 %10, ptr %2, align 4
  br label %return

  br label %if.end.0

if.end.0:
  %12 = load i32, ptr %3, align 4
  %13 = add nsw i32 0, 1
  %14 = sub nsw i32 %12, %13
  %15 = call i32 @fib(i32 noundef %14)
  %16 = load i32, ptr %3, align 4
  %17 = add nsw i32 0, 2
  %18 = sub nsw i32 %16, %17
  %19 = call i32 @fib(i32 noundef %18)
  %20 = add nsw i32 %15, %19
  store i32 %20, ptr %2, align 4
  br label %return

  br label %return

return:
  %23 = load i32, ptr %2, align 4
  ret i32 %23
}

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = add nsw i32 0, 10
  %3 = call i32 @fib(i32 noundef %2)
  store i32 %3, ptr %1, align 4
  br label %return

  br label %return

return:
  %6 = load i32, ptr %1, align 4
  ret i32 %6
}

attributes #0 = { noinline nounwind optnone uwtable }
