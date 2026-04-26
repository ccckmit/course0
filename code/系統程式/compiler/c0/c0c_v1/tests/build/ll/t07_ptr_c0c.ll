; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t07_ptr.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t07_ptr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca ptr, align 8
  store i32 0, ptr %1, align 4
  %4 = add nsw i32 0, 10
  store i32 %4, ptr %2, align 4
  store ptr %2, ptr %3, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = add nsw i32 0, 20
  store i32 %6, ptr %5, align 4
  %7 = load i32, ptr %2, align 4
  store i32 %7, ptr %1, align 4
  br label %return

  br label %return

return:
  %10 = load i32, ptr %1, align 4
  ret i32 %10
}

attributes #0 = { noinline nounwind optnone uwtable }
