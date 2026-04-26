; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_compare.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_compare.c"
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
  %t1 = sext i32 5 to i64
  store i64 %t1, ptr %t0
  %t2 = alloca i64
  %t3 = sext i32 10 to i64
  store i64 %t3, ptr %t2
  %t4 = alloca i64
  %t5 = sext i32 0 to i64
  store i64 %t5, ptr %t4
  %t6 = load i64, ptr %t0
  %t7 = load i64, ptr %t2
  %t9 = sext i32 %t6 to i64
  %t10 = sext i32 %t7 to i64
  %t8 = icmp slt i64 %t9, %t10
  %t11 = zext i1 %t8 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L0, label %L2
L0:
  %t13 = load i64, ptr %t4
  %t15 = sext i32 %t13 to i64
  %t14 = add i64 %t15, 1
  store i64 %t14, ptr %t4
  br label %L2
L2:
  %t16 = load i64, ptr %t0
  %t17 = load i64, ptr %t2
  %t19 = sext i32 %t16 to i64
  %t20 = sext i32 %t17 to i64
  %t18 = icmp sle i64 %t19, %t20
  %t21 = zext i1 %t18 to i64
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L3, label %L5
L3:
  %t23 = load i64, ptr %t4
  %t25 = sext i32 %t23 to i64
  %t24 = add i64 %t25, 1
  store i64 %t24, ptr %t4
  br label %L5
L5:
  %t26 = load i64, ptr %t2
  %t27 = load i64, ptr %t0
  %t29 = sext i32 %t26 to i64
  %t30 = sext i32 %t27 to i64
  %t28 = icmp sgt i64 %t29, %t30
  %t31 = zext i1 %t28 to i64
  %t32 = icmp ne i64 %t31, 0
  br i1 %t32, label %L6, label %L8
L6:
  %t33 = load i64, ptr %t4
  %t35 = sext i32 %t33 to i64
  %t34 = add i64 %t35, 1
  store i64 %t34, ptr %t4
  br label %L8
L8:
  %t36 = load i64, ptr %t2
  %t37 = load i64, ptr %t0
  %t39 = sext i32 %t36 to i64
  %t40 = sext i32 %t37 to i64
  %t38 = icmp sge i64 %t39, %t40
  %t41 = zext i1 %t38 to i64
  %t42 = icmp ne i64 %t41, 0
  br i1 %t42, label %L9, label %L11
L9:
  %t43 = load i64, ptr %t4
  %t45 = sext i32 %t43 to i64
  %t44 = add i64 %t45, 1
  store i64 %t44, ptr %t4
  br label %L11
L11:
  %t46 = load i64, ptr %t0
  %t47 = load i64, ptr %t2
  %t49 = sext i32 %t46 to i64
  %t50 = sext i32 %t47 to i64
  %t48 = icmp ne i64 %t49, %t50
  %t51 = zext i1 %t48 to i64
  %t52 = icmp ne i64 %t51, 0
  br i1 %t52, label %L12, label %L14
L12:
  %t53 = load i64, ptr %t4
  %t55 = sext i32 %t53 to i64
  %t54 = add i64 %t55, 1
  store i64 %t54, ptr %t4
  br label %L14
L14:
  %t56 = load i64, ptr %t0
  %t58 = sext i32 %t56 to i64
  %t59 = sext i32 5 to i64
  %t57 = icmp eq i64 %t58, %t59
  %t60 = zext i1 %t57 to i64
  %t61 = icmp ne i64 %t60, 0
  br i1 %t61, label %L15, label %L17
L15:
  %t62 = load i64, ptr %t4
  %t64 = sext i32 %t62 to i64
  %t63 = add i64 %t64, 1
  store i64 %t63, ptr %t4
  br label %L17
L17:
  %t65 = load i64, ptr %t4
  %t66 = sext i32 %t65 to i64
  %t67 = trunc i64 %t66 to i32
  ret i32 %t67
L18:
  ret i32 0
}

