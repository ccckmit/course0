; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_bitwise.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_bitwise.c"
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
  %t1 = sext i32 12 to i64
  store i64 %t1, ptr %t0
  %t2 = alloca i64
  %t3 = sext i32 10 to i64
  store i64 %t3, ptr %t2
  %t4 = alloca i64
  %t5 = load i64, ptr %t0
  %t6 = load i64, ptr %t2
  %t8 = sext i32 %t5 to i64
  %t9 = sext i32 %t6 to i64
  %t7 = and i64 %t8, %t9
  store i64 %t7, ptr %t4
  %t10 = alloca i64
  %t11 = load i64, ptr %t0
  %t12 = load i64, ptr %t2
  %t14 = sext i32 %t11 to i64
  %t15 = sext i32 %t12 to i64
  %t13 = or i64 %t14, %t15
  store i64 %t13, ptr %t10
  %t16 = alloca i64
  %t17 = load i64, ptr %t0
  %t18 = load i64, ptr %t2
  %t20 = sext i32 %t17 to i64
  %t21 = sext i32 %t18 to i64
  %t19 = xor i64 %t20, %t21
  store i64 %t19, ptr %t16
  %t22 = alloca i64
  %t23 = load i64, ptr %t10
  %t25 = sext i32 %t23 to i64
  %t26 = sext i32 2 to i64
  %t24 = shl i64 %t25, %t26
  store i64 %t24, ptr %t22
  %t27 = load i64, ptr %t4
  %t28 = load i64, ptr %t16
  %t30 = sext i32 %t27 to i64
  %t31 = sext i32 %t28 to i64
  %t29 = add i64 %t30, %t31
  %t32 = load i64, ptr %t22
  %t34 = sext i32 %t32 to i64
  %t35 = sext i32 10 to i64
  %t33 = sub i64 %t34, %t35
  %t36 = add i64 %t29, %t33
  %t37 = trunc i64 %t36 to i32
  ret i32 %t37
L0:
  ret i32 0
}

