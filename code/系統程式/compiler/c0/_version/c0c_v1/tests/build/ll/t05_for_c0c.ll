; ModuleID = '/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t05_for.c'
source_filename = "/Users/Shared/ccc/c0py/compiler/c0/_version/c0c_v1/tests/cases/t05_for.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "arm64-apple-darwin"

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %4 = add nsw i32 0, 0
  store i32 %4, ptr %2, align 4
  %5 = add nsw i32 0, 0
  store i32 %5, ptr %3, align 4
  br label %for.cond.0

for.cond.0:
  %7 = load i32, ptr %3, align 4
  %8 = add nsw i32 0, 10
  %9 = icmp slt i32 %7, %8
  %10 = zext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %for.body.0, label %for.end.0

for.body.0:
  %13 = load i32, ptr %2, align 4
  %14 = load i32, ptr %3, align 4
  %15 = add nsw i32 %13, %14
  store i32 %15, ptr %2, align 4
  br label %for.step.0

for.step.0:
  %17 = load i32, ptr %3, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, ptr %3, align 4
  br label %for.cond.0

for.end.0:
  %20 = load i32, ptr %2, align 4
  store i32 %20, ptr %1, align 4
  br label %return

  br label %return

return:
  %23 = load i32, ptr %1, align 4
  ret i32 %23
}

attributes #0 = { noinline nounwind optnone uwtable }
