; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_float.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_float.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx15.0.0"

; stdlib declarations
declare ptr @malloc(i64)
declare ptr @calloc(i64, i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)
declare i64 @strlen(ptr)
declare ptr @strdup(ptr)
declare ptr @strndup(ptr, i64)
declare ptr @strcpy(ptr, ptr)
declare ptr @strncpy(ptr, ptr, i64)
declare ptr @strcat(ptr, ptr)
declare ptr @strchr(ptr, i64)
declare ptr @strstr(ptr, ptr)
declare i32 @strcmp(ptr, ptr)
declare i32 @strncmp(ptr, ptr, i64)
declare ptr @memcpy(ptr, ptr, i64)
declare ptr @memset(ptr, i32, i64)
declare i32 @memcmp(ptr, ptr, i64)
declare i32 @printf(ptr, ...)
declare i32 @fprintf(ptr, ptr, ...)
declare i32 @sprintf(ptr, ptr, ...)
declare i32 @snprintf(ptr, i64, ptr, ...)
declare i32 @vfprintf(ptr, ptr, ptr)
declare i32 @vsnprintf(ptr, i64, ptr, ptr)
declare ptr @fopen(ptr, ptr)
declare i32 @fclose(ptr)
declare i64 @fread(ptr, i64, i64, ptr)
declare i64 @fwrite(ptr, i64, i64, ptr)
declare i32 @fseek(ptr, i64, i32)
declare i64 @ftell(ptr)
declare void @perror(ptr)
declare void @exit(i32)
declare ptr @getenv(ptr)
declare i32 @atoi(ptr)
declare i64 @atol(ptr)
declare i64 @strtol(ptr, ptr, i32)
declare i64 @strtoll(ptr, ptr, i32)
declare double @atof(ptr)
declare i32 @isspace(i32)
declare i32 @isdigit(i32)
declare i32 @isalpha(i32)
declare i32 @isalnum(i32)
declare i32 @isxdigit(i32)
declare i32 @isupper(i32)
declare i32 @islower(i32)
declare i32 @toupper(i32)
declare i32 @tolower(i32)
declare i32 @assert(i32)
declare ptr @__c0c_stderr()
declare ptr @__c0c_stdout()
declare ptr @__c0c_stdin()
declare ptr @__c0c_get_tbuf(i32)
declare ptr @__c0c_get_td_name(i64)
declare i64 @__c0c_get_td_kind(i64)
declare void @__c0c_emit(ptr, ptr, ...)


define dso_local i32 @main() {
entry:
  %t0 = alloca double
  %t1 = fadd double 0.0, 1.5
  store double %t1, ptr %t0
  %t2 = alloca double
  %t3 = fadd double 0.0, 2
  store double %t3, ptr %t2
  %t4 = alloca double
  %t5 = load double, ptr %t0
  %t6 = load double, ptr %t2
  %t7 = fadd double %t5, %t6
  store double %t7, ptr %t4
  %t8 = alloca double
  %t9 = load double, ptr %t0
  %t10 = load double, ptr %t2
  %t11 = fmul double %t9, %t10
  store double %t11, ptr %t8
  %t12 = alloca double
  %t13 = load double, ptr %t2
  %t14 = load double, ptr %t0
  %t15 = fsub double %t13, %t14
  store double %t15, ptr %t12
  %t16 = alloca i64
  %t17 = load double, ptr %t4
  %t19 = sitofp i64 0 to double
  %t18 = fcmp ogt double %t17, %t19
  %t20 = zext i1 %t18 to i64
  %t21 = icmp ne i64 %t20, 0
  br i1 %t21, label %L0, label %L1
L0:
  %t22 = sext i32 1 to i64
  br label %L2
L1:
  %t23 = sext i32 0 to i64
  br label %L2
L2:
  %t24 = phi i64 [ %t22, %L0 ], [ %t23, %L1 ]
  store i64 %t24, ptr %t16
  %t25 = load i64, ptr %t16
  %t26 = sext i32 %t25 to i64
  %t27 = trunc i64 %t26 to i32
  ret i32 %t27
L3:
  ret i32 0
}

