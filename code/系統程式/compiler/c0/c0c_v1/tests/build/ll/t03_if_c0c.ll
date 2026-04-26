; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t03_if.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t03_if.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %3 = add nsw i32 0, 5
  store i32 %3, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = add nsw i32 0, 3
  %6 = icmp sgt i32 %4, %5
  %7 = zext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if.then.0, label %if.end.0

if.then.0:
  %10 = add nsw i32 0, 1
  store i32 %10, ptr %1, align 4
  br label %return

  br label %if.end.0

if.end.0:
  %12 = add nsw i32 0, 0
  store i32 %12, ptr %1, align 4
  br label %return

  br label %return

return:
  %15 = load i32, ptr %1, align 4
  ret i32 %15
}

attributes #0 = { noinline nounwind optnone uwtable }
