; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t08_global.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t08_global.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

@g = dso_local global i32 100, align 4

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = load i32, ptr @g, align 4
  %3 = add nsw i32 0, 1
  %4 = add nsw i32 %2, %3
  store i32 %4, ptr @g, align 4
  %5 = load i32, ptr @g, align 4
  store i32 %5, ptr %1, align 4
  br label %return

  br label %return

return:
  %8 = load i32, ptr %1, align 4
  ret i32 %8
}

attributes #0 = { noinline nounwind optnone uwtable }
