; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t02_arith.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t02_arith.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %5 = add nsw i32 0, 10
  store i32 %5, ptr %2, align 4
  %6 = add nsw i32 0, 3
  store i32 %6, ptr %3, align 4
  %7 = load i32, ptr %2, align 4
  %8 = load i32, ptr %3, align 4
  %9 = add nsw i32 0, 2
  %10 = mul nsw i32 %8, %9
  %11 = add nsw i32 %7, %10
  %12 = add nsw i32 0, 1
  %13 = sub nsw i32 %11, %12
  store i32 %13, ptr %4, align 4
  %14 = load i32, ptr %4, align 4
  store i32 %14, ptr %1, align 4
  br label %return

  br label %return

return:
  %17 = load i32, ptr %1, align 4
  ret i32 %17
}

attributes #0 = { noinline nounwind optnone uwtable }
