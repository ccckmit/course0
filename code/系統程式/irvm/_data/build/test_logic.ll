; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_logic.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_logic.c"
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
  %t1 = sext i32 1 to i64
  store i64 %t1, ptr %t0
  %t2 = alloca i64
  %t3 = sext i32 0 to i64
  store i64 %t3, ptr %t2
  %t4 = alloca i64
  %t5 = sext i32 0 to i64
  store i64 %t5, ptr %t4
  %t6 = load i64, ptr %t0
  %t7 = sext i32 %t6 to i64
  %t8 = icmp ne i64 %t7, 0
  br i1 %t8, label %L0, label %L1
L0:
  %t9 = load i64, ptr %t2
  %t11 = sext i32 %t9 to i64
  %t12 = icmp eq i64 %t11, 0
  %t10 = zext i1 %t12 to i64
  %t13 = icmp ne i64 %t10, 0
  %t14 = zext i1 %t13 to i64
  br label %L2
L1:
  br label %L2
L2:
  %t15 = phi i64 [ %t14, %L0 ], [ 0, %L1 ]
  %t16 = icmp ne i64 %t15, 0
  br i1 %t16, label %L3, label %L5
L3:
  %t17 = load i64, ptr %t4
  %t19 = sext i32 %t17 to i64
  %t18 = add i64 %t19, 1
  store i64 %t18, ptr %t4
  br label %L5
L5:
  %t20 = load i64, ptr %t0
  %t21 = sext i32 %t20 to i64
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L6, label %L7
L6:
  br label %L8
L7:
  %t23 = load i64, ptr %t2
  %t24 = sext i32 %t23 to i64
  %t25 = icmp ne i64 %t24, 0
  %t26 = zext i1 %t25 to i64
  br label %L8
L8:
  %t27 = phi i64 [ 1, %L6 ], [ %t26, %L7 ]
  %t28 = icmp ne i64 %t27, 0
  br i1 %t28, label %L9, label %L11
L9:
  %t29 = load i64, ptr %t4
  %t31 = sext i32 %t29 to i64
  %t30 = add i64 %t31, 1
  store i64 %t30, ptr %t4
  br label %L11
L11:
  %t32 = load i64, ptr %t0
  %t33 = sext i32 %t32 to i64
  %t34 = icmp ne i64 %t33, 0
  br i1 %t34, label %L12, label %L13
L12:
  %t35 = load i64, ptr %t2
  %t36 = sext i32 %t35 to i64
  %t37 = icmp ne i64 %t36, 0
  %t38 = zext i1 %t37 to i64
  br label %L14
L13:
  br label %L14
L14:
  %t39 = phi i64 [ %t38, %L12 ], [ 0, %L13 ]
  %t41 = icmp eq i64 %t39, 0
  %t40 = zext i1 %t41 to i64
  %t42 = icmp ne i64 %t40, 0
  br i1 %t42, label %L15, label %L17
L15:
  %t43 = load i64, ptr %t4
  %t45 = sext i32 %t43 to i64
  %t44 = add i64 %t45, 1
  store i64 %t44, ptr %t4
  br label %L17
L17:
  %t46 = load i64, ptr %t4
  %t47 = sext i32 %t46 to i64
  %t48 = trunc i64 %t47 to i32
  ret i32 %t48
L18:
  ret i32 0
}

