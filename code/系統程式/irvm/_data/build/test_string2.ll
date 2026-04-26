; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_string2.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_string2.c"
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
  %t0 = alloca ptr
  %t1 = load ptr, ptr %t0
  %t3 = sext i32 0 to i64
  %t2 = getelementptr ptr, ptr %t1, i64 %t3
  %t4 = sext i32 104 to i64
  store i64 %t4, ptr %t2
  %t5 = load ptr, ptr %t0
  %t7 = sext i32 1 to i64
  %t6 = getelementptr ptr, ptr %t5, i64 %t7
  %t8 = sext i32 101 to i64
  store i64 %t8, ptr %t6
  %t9 = load ptr, ptr %t0
  %t11 = sext i32 2 to i64
  %t10 = getelementptr ptr, ptr %t9, i64 %t11
  %t12 = sext i32 108 to i64
  store i64 %t12, ptr %t10
  %t13 = load ptr, ptr %t0
  %t15 = sext i32 3 to i64
  %t14 = getelementptr ptr, ptr %t13, i64 %t15
  %t16 = sext i32 108 to i64
  store i64 %t16, ptr %t14
  %t17 = load ptr, ptr %t0
  %t19 = sext i32 4 to i64
  %t18 = getelementptr ptr, ptr %t17, i64 %t19
  %t20 = sext i32 111 to i64
  store i64 %t20, ptr %t18
  %t21 = load ptr, ptr %t0
  %t23 = sext i32 5 to i64
  %t22 = getelementptr ptr, ptr %t21, i64 %t23
  %t24 = sext i32 0 to i64
  store i64 %t24, ptr %t22
  %t25 = alloca i64
  %t26 = sext i32 0 to i64
  store i64 %t26, ptr %t25
  br label %L0
L0:
  %t27 = load ptr, ptr %t0
  %t28 = load i64, ptr %t25
  %t29 = sext i32 %t28 to i64
  %t30 = getelementptr ptr, ptr %t27, i64 %t29
  %t31 = load ptr, ptr %t30
  %t33 = ptrtoint ptr %t31 to i64
  %t34 = sext i32 0 to i64
  %t32 = icmp ne i64 %t33, %t34
  %t35 = zext i1 %t32 to i64
  %t36 = icmp ne i64 %t35, 0
  br i1 %t36, label %L1, label %L2
L1:
  %t37 = load i64, ptr %t25
  %t39 = sext i32 %t37 to i64
  %t38 = add i64 %t39, 1
  store i64 %t38, ptr %t25
  br label %L0
L2:
  %t40 = load i64, ptr %t25
  %t41 = sext i32 %t40 to i64
  %t42 = trunc i64 %t41 to i32
  ret i32 %t42
L3:
  ret i32 0
}

