; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_div_signed.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_div_signed.c"
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
  %t0 = alloca i64
  %t1 = sext i32 10 to i64
  store i64 %t1, ptr %t0
  %t2 = alloca i64
  %t4 = sext i32 3 to i64
  %t3 = sub i64 0, %t4
  store i64 %t3, ptr %t2
  %t5 = alloca i64
  %t7 = sext i32 10 to i64
  %t8 = sext i32 3 to i64
  %t6 = sdiv i64 %t7, %t8
  store i64 %t6, ptr %t5
  %t9 = alloca i64
  %t11 = sext i32 10 to i64
  %t10 = sub i64 0, %t11
  %t13 = sext i32 3 to i64
  %t12 = sdiv i64 %t10, %t13
  store i64 %t12, ptr %t9
  %t14 = alloca i64
  %t16 = sext i32 3 to i64
  %t15 = sub i64 0, %t16
  %t18 = sext i32 10 to i64
  %t17 = sdiv i64 %t18, %t15
  store i64 %t17, ptr %t14
  %t19 = alloca i64
  %t21 = sext i32 10 to i64
  %t20 = sub i64 0, %t21
  %t23 = sext i32 3 to i64
  %t22 = sub i64 0, %t23
  %t24 = sdiv i64 %t20, %t22
  store i64 %t24, ptr %t19
  %t25 = alloca i64
  %t27 = sext i32 10 to i64
  %t28 = sext i32 3 to i64
  %t26 = srem i64 %t27, %t28
  store i64 %t26, ptr %t25
  %t29 = alloca i64
  %t31 = sext i32 10 to i64
  %t30 = sub i64 0, %t31
  %t33 = sext i32 3 to i64
  %t32 = srem i64 %t30, %t33
  store i64 %t32, ptr %t29
  %t34 = alloca i64
  %t36 = sext i32 3 to i64
  %t35 = sub i64 0, %t36
  %t38 = sext i32 10 to i64
  %t37 = srem i64 %t38, %t35
  store i64 %t37, ptr %t34
  %t39 = alloca i64
  %t41 = sext i32 10 to i64
  %t40 = sub i64 0, %t41
  %t43 = sext i32 3 to i64
  %t42 = sub i64 0, %t43
  %t44 = srem i64 %t40, %t42
  store i64 %t44, ptr %t39
  %t45 = load i64, ptr %t5
  %t46 = load i64, ptr %t9
  %t48 = sext i32 %t45 to i64
  %t49 = sext i32 %t46 to i64
  %t47 = add i64 %t48, %t49
  %t50 = load i64, ptr %t14
  %t52 = sext i32 %t50 to i64
  %t51 = add i64 %t47, %t52
  %t53 = load i64, ptr %t19
  %t55 = sext i32 %t53 to i64
  %t54 = add i64 %t51, %t55
  %t56 = load i64, ptr %t25
  %t58 = sext i32 %t56 to i64
  %t57 = add i64 %t54, %t58
  %t59 = load i64, ptr %t29
  %t61 = sext i32 %t59 to i64
  %t60 = add i64 %t57, %t61
  %t62 = load i64, ptr %t34
  %t64 = sext i32 %t62 to i64
  %t63 = add i64 %t60, %t64
  %t65 = load i64, ptr %t39
  %t67 = sext i32 %t65 to i64
  %t66 = add i64 %t63, %t67
  %t68 = trunc i64 %t66 to i32
  ret i32 %t68
L0:
  ret i32 0
}

