; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_arith.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_arith.c"
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
  %t3 = sext i32 5 to i64
  store i64 %t3, ptr %t2
  %t4 = alloca i64
  %t5 = load i64, ptr %t0
  %t6 = load i64, ptr %t2
  %t8 = sext i32 %t5 to i64
  %t9 = sext i32 %t6 to i64
  %t7 = add i64 %t8, %t9
  store i64 %t7, ptr %t4
  %t10 = alloca i64
  %t11 = load i64, ptr %t4
  %t13 = sext i32 %t11 to i64
  %t14 = sext i32 2 to i64
  %t12 = mul i64 %t13, %t14
  store i64 %t12, ptr %t10
  %t15 = alloca i64
  %t16 = load i64, ptr %t10
  %t18 = sext i32 %t16 to i64
  %t19 = sext i32 3 to i64
  %t17 = sdiv i64 %t18, %t19
  store i64 %t17, ptr %t15
  %t20 = load i64, ptr %t15
  %t22 = sext i32 %t20 to i64
  %t23 = sext i32 20 to i64
  %t21 = add i64 %t22, %t23
  %t24 = trunc i64 %t21 to i32
  ret i32 %t24
L0:
  ret i32 0
}

