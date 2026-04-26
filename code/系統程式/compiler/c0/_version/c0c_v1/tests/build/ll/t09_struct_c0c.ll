; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t09_struct.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t09_struct.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

%struct.Point = type { i32, i32 }

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.Point, align 8
  store i32 0, ptr %1, align 4
  %3 = getelementptr inbounds %struct.Point, ptr %2, i32 0, i32 0
  %4 = add nsw i32 0, 3
  store i32 %4, ptr %3, align 4
  %5 = getelementptr inbounds %struct.Point, ptr %2, i32 0, i32 1
  %6 = add nsw i32 0, 4
  store i32 %6, ptr %5, align 4
  %7 = getelementptr inbounds %struct.Point, ptr %2, i32 0, i32 0
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds %struct.Point, ptr %2, i32 0, i32 1
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  store i32 %11, ptr %1, align 4
  br label %return

  br label %return

return:
  %14 = load i32, ptr %1, align 4
  ret i32 %14
}

attributes #0 = { noinline nounwind optnone uwtable }
