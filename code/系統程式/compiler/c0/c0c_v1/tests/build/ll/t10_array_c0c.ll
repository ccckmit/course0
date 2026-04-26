; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t10_array.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t10_array.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca [5 x i32], align 8
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %4 = add nsw i32 0, 0
  store i32 %4, ptr %3, align 4
  br label %for.cond.0

for.cond.0:
  %6 = load i32, ptr %3, align 4
  %7 = add nsw i32 0, 5
  %8 = icmp slt i32 %6, %7
  %9 = zext i1 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %for.body.0, label %for.end.0

for.body.0:
  %12 = load i32, ptr %3, align 4
  %13 = sext i32 %12 to i64
  %14 = getelementptr i32, ptr %2, i64 %13
  %15 = load i32, ptr %3, align 4
  %16 = add nsw i32 0, 2
  %17 = mul nsw i32 %15, %16
  store i32 %17, ptr %14, align 4
  br label %for.step.0

for.step.0:
  %19 = load i32, ptr %3, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %3, align 4
  br label %for.cond.0

for.end.0:
  %22 = add nsw i32 0, 4
  %23 = sext i32 %22 to i64
  %24 = getelementptr i32, ptr %2, i64 %23
  %25 = load i32, ptr %24, align 4
  store i32 %25, ptr %1, align 4
  br label %return

  br label %return

return:
  %28 = load i32, ptr %1, align 4
  ret i32 %28
}

attributes #0 = { noinline nounwind optnone uwtable }
