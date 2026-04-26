; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca double
  store double 3.00000000000000000, ptr %v0
  %v1 = alloca double
  store double 2.00000000000000000, ptr %v1
  %v2 = alloca double
  %0 = load double, ptr %v0
  %1 = load double, ptr %v1
  %2 = fdiv double %0, %1
  store double %2, ptr %v2
  %3 = getelementptr [6 x i8], ptr @.str.0, i32 0, i32 0
  %4 = load double, ptr %v2
  %5 = call i32 (ptr, ...) @printf(ptr %3, double %4)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [6 x i8] c"c=%f\0A\00", align 1
declare i32 @printf(ptr, ...)
