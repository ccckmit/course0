; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca double
  store double 3.70000000000000018, ptr %v0
  %v1 = alloca i32
  %0 = load double, ptr %v0
  %1 = fptosi double %0 to i32
  store i32 %1, ptr %v1
  %v2 = alloca float
  %2 = load i32, ptr %v1
  %3 = sitofp i32 %2 to float
  %4 = fpext float %3 to double
  %5 = fadd double %4, 0.50000000000000000
  %6 = fptrunc double %5 to float
  store float %6, ptr %v2
  %7 = getelementptr [11 x i8], ptr @.str.0, i32 0, i32 0
  %8 = load i32, ptr %v1
  %9 = load float, ptr %v2
  %10 = fpext float %9 to double
  %11 = call i32 (ptr, ...) @printf(ptr %7, i32 %8, double %10)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [11 x i8] c"i=%d f=%f\0A\00", align 1
declare i32 @printf(ptr, ...)
