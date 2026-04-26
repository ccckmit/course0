; ModuleID = 'c0c_compat.c'
source_filename = "c0c_compat.c"
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

@compat_emit_buf = internal global ptr zeroinitializer
@tbuf_storage = internal global ptr zeroinitializer
@td_names_table = internal global ptr zeroinitializer
@td_kinds_table = internal global ptr zeroinitializer
@assign_ops_table = internal global ptr zeroinitializer

define dso_local void @__c0c_emit(ptr %t0, ptr %t1, ...) {
entry:
  %t2 = alloca i64
  %t3 = load i64, ptr %t2
  call void @__c0c_va_start(i64 %t3, ptr %t1)
  %t5 = alloca i64
  %t6 = load ptr, ptr @compat_emit_buf
  %t7 = load i64, ptr %t2
  %t8 = call i32 (ptr, ...) @vsnprintf(ptr %t6, i64 8192, ptr %t1, i64 %t7)
  %t9 = sext i32 %t8 to i64
  store i64 %t9, ptr %t5
  %t10 = load i64, ptr %t2
  call void @__c0c_va_end(i64 %t10)
  %t12 = load i64, ptr %t5
  %t14 = sext i32 %t12 to i64
  %t15 = sext i32 0 to i64
  %t13 = icmp sgt i64 %t14, %t15
  %t16 = zext i1 %t13 to i64
  %t17 = icmp ne i64 %t16, 0
  br i1 %t17, label %L0, label %L2
L0:
  %t18 = load ptr, ptr @compat_emit_buf
  %t19 = load i64, ptr %t5
  %t21 = sext i32 %t19 to i64
  %t22 = sext i32 8192 to i64
  %t20 = icmp slt i64 %t21, %t22
  %t23 = zext i1 %t20 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L3, label %L4
L3:
  %t25 = load i64, ptr %t5
  %t27 = sext i32 %t25 to i64
  %t26 = add i64 %t27, 0
  br label %L5
L4:
  %t29 = sext i32 8192 to i64
  %t30 = sext i32 1 to i64
  %t28 = sub i64 %t29, %t30
  br label %L5
L5:
  %t31 = phi i64 [ %t26, %L3 ], [ %t28, %L4 ]
  %t32 = call i64 @fwrite(ptr %t18, i64 1, i64 %t31, ptr %t0)
  br label %L2
L2:
  ret void
}

define dso_local ptr @__c0c_stderr() {
entry:
  %t0 = call ptr @__c0c_stderr()
  ret ptr %t0
L0:
  ret ptr null
}

define dso_local ptr @__c0c_stdout() {
entry:
  %t0 = call ptr @__c0c_stdout()
  ret ptr %t0
L0:
  ret ptr null
}

define dso_local ptr @__c0c_stdin() {
entry:
  %t0 = call ptr @__c0c_stdin()
  ret ptr %t0
L0:
  ret ptr null
}

define dso_local ptr @__c0c_get_tbuf(i64 %t0) {
entry:
  %t1 = load ptr, ptr @tbuf_storage
  %t2 = add i64 %t0, 0
  %t4 = sext i32 8 to i64
  %t3 = srem i64 %t2, %t4
  %t5 = getelementptr ptr, ptr %t1, i64 %t3
  %t6 = load ptr, ptr %t5
  ret ptr %t6
L0:
  ret ptr null
}

define dso_local ptr @__c0c_get_td_name(i64 %t0) {
entry:
  %t1 = load ptr, ptr @td_names_table
  %t2 = getelementptr ptr, ptr %t1, i64 %t0
  %t3 = load ptr, ptr %t2
  ret ptr %t3
L0:
  ret ptr null
}

define dso_local i64 @__c0c_get_td_kind(i64 %t0) {
entry:
  %t1 = load ptr, ptr @td_kinds_table
  %t2 = getelementptr ptr, ptr %t1, i64 %t0
  %t3 = load ptr, ptr %t2
  %t4 = ptrtoint ptr %t3 to i64
  ret i64 %t4
L0:
  ret i64 0
}

define dso_local i32 @__c0c_get_assign_op(i64 %t0) {
entry:
  %t1 = load ptr, ptr @assign_ops_table
  %t2 = getelementptr ptr, ptr %t1, i64 %t0
  %t3 = load ptr, ptr %t2
  %t4 = ptrtoint ptr %t3 to i64
  %t5 = trunc i64 %t4 to i32
  ret i32 %t5
L0:
  ret i32 0
}

