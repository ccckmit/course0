; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t06_func.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t06_func.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store i32 %1, ptr %5, align 4
  %6 = load i32, ptr %4, align 4
  %7 = load i32, ptr %5, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, ptr %3, align 4
  br label %return

  br label %return

return:
  %11 = load i32, ptr %3, align 4
  ret i32 %11
}

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = add nsw i32 0, 3
  %3 = add nsw i32 0, 4
  %4 = call i32 @add(i32 noundef %2, i32 noundef %3)
  store i32 %4, ptr %1, align 4
  br label %return

  br label %return

return:
  %7 = load i32, ptr %1, align 4
  ret i32 %7
}

attributes #0 = { noinline nounwind optnone uwtable }
