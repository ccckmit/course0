; ModuleID = 'c0c'
define i32 @main() {
entry:
  %v0 = alloca float
  %0 = fptrunc double 1.50000000000000000 to float
  store float %0, ptr %v0
  %v1 = alloca float
  %1 = fptrunc double 2.00000000000000000 to float
  store float %1, ptr %v1
  %v2 = alloca float
  %2 = load float, ptr %v0
  %3 = load float, ptr %v1
  %4 = fmul float %2, %3
  %5 = fpext float %4 to double
  %6 = fadd double %5, 1.00000000000000000
  %7 = fptrunc double %6 to float
  store float %7, ptr %v2
  %8 = getelementptr [6 x i8], ptr @.str.0, i32 0, i32 0
  %9 = load float, ptr %v2
  %10 = fpext float %9 to double
  %11 = call i32 (ptr, ...) @printf(ptr %8, double %10)
  ret i32 0
}

@.str.0 = private unnamed_addr constant [6 x i8] c"c=%f\0A\00", align 1
declare i32 @printf(ptr, ...)
