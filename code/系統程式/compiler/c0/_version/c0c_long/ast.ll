; ModuleID = 'ast.c'
source_filename = "ast.c"
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


define dso_local ptr @node_new(ptr %t0, i64 %t1) {
entry:
  %t2 = alloca ptr
  %t3 = call ptr @calloc(i64 1, i64 8)
  store ptr %t3, ptr %t2
  %t4 = load ptr, ptr %t2
  %t6 = ptrtoint ptr %t4 to i64
  %t7 = icmp eq i64 %t6, 0
  %t5 = zext i1 %t7 to i64
  %t8 = icmp ne i64 %t5, 0
  br i1 %t8, label %L0, label %L2
L0:
  %t9 = getelementptr [7 x i8], ptr @.str0, i64 0, i64 0
  call void @perror(ptr %t9)
  call void @exit(i64 1)
  br label %L2
L2:
  %t12 = load ptr, ptr %t2
  %t13 = getelementptr i8, ptr %t12, i64 0
  store ptr %t0, ptr %t13
  %t14 = load ptr, ptr %t2
  %t15 = getelementptr i8, ptr %t14, i64 0
  store i64 %t1, ptr %t15
  %t16 = load ptr, ptr %t2
  ret ptr %t16
L3:
  ret ptr null
}

define dso_local void @node_add_child(ptr %t0, ptr %t1) {
entry:
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = load i64, ptr %t2
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t7 = sext i32 1 to i64
  %t6 = add i64 %t5, %t7
  %t9 = sext i32 8 to i64
  %t8 = mul i64 %t6, %t9
  %t10 = call ptr @realloc(i64 %t3, i64 %t8)
  %t11 = getelementptr i8, ptr %t0, i64 0
  store ptr %t10, ptr %t11
  %t12 = getelementptr i8, ptr %t0, i64 0
  %t13 = load i64, ptr %t12
  %t15 = icmp eq i64 %t13, 0
  %t14 = zext i1 %t15 to i64
  %t16 = icmp ne i64 %t14, 0
  br i1 %t16, label %L0, label %L2
L0:
  %t17 = getelementptr [8 x i8], ptr @.str1, i64 0, i64 0
  call void @perror(ptr %t17)
  call void @exit(i64 1)
  br label %L2
L2:
  %t20 = getelementptr i8, ptr %t0, i64 0
  %t21 = load i64, ptr %t20
  %t22 = getelementptr i8, ptr %t0, i64 0
  %t23 = load i64, ptr %t22
  %t24 = add i64 %t23, 1
  %t25 = getelementptr i8, ptr %t0, i64 0
  store i64 %t24, ptr %t25
  %t27 = inttoptr i64 %t21 to ptr
  %t26 = getelementptr ptr, ptr %t27, i64 %t23
  store ptr %t1, ptr %t26
  ret void
}

define dso_local void @node_free(ptr %t0) {
entry:
  %t2 = ptrtoint ptr %t0 to i64
  %t3 = icmp eq i64 %t2, 0
  %t1 = zext i1 %t3 to i64
  %t4 = icmp ne i64 %t1, 0
  br i1 %t4, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  br label %L4
L4:
  %t7 = load i64, ptr %t5
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  %t11 = sext i32 %t7 to i64
  %t10 = icmp slt i64 %t11, %t9
  %t12 = zext i1 %t10 to i64
  %t13 = icmp ne i64 %t12, 0
  br i1 %t13, label %L5, label %L7
L5:
  %t14 = getelementptr i8, ptr %t0, i64 0
  %t15 = load i64, ptr %t14
  %t16 = load i64, ptr %t5
  %t17 = inttoptr i64 %t15 to ptr
  %t18 = sext i32 %t16 to i64
  %t19 = getelementptr ptr, ptr %t17, i64 %t18
  %t20 = load ptr, ptr %t19
  call void @node_free(ptr %t20)
  br label %L6
L6:
  %t22 = load i64, ptr %t5
  %t24 = sext i32 %t22 to i64
  %t23 = add i64 %t24, 1
  store i64 %t23, ptr %t5
  br label %L4
L7:
  %t25 = getelementptr i8, ptr %t0, i64 0
  %t26 = load i64, ptr %t25
  call void @free(i64 %t26)
  %t28 = getelementptr i8, ptr %t0, i64 0
  %t29 = load i64, ptr %t28
  call void @free(i64 %t29)
  %t31 = getelementptr i8, ptr %t0, i64 0
  %t32 = load i64, ptr %t31
  call void @free(i64 %t32)
  %t34 = getelementptr i8, ptr %t0, i64 0
  %t35 = load i64, ptr %t34
  call void @free(i64 %t35)
  %t37 = getelementptr i8, ptr %t0, i64 0
  %t38 = load i64, ptr %t37
  call void @free(i64 %t38)
  call void @free(ptr %t0)
  ret void
}

define dso_local ptr @type_new(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @calloc(i64 1, i64 8)
  store ptr %t2, ptr %t1
  %t3 = load ptr, ptr %t1
  %t5 = ptrtoint ptr %t3 to i64
  %t6 = icmp eq i64 %t5, 0
  %t4 = zext i1 %t6 to i64
  %t7 = icmp ne i64 %t4, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = getelementptr [7 x i8], ptr @.str2, i64 0, i64 0
  call void @perror(ptr %t8)
  call void @exit(i64 1)
  br label %L2
L2:
  %t11 = load ptr, ptr %t1
  %t12 = getelementptr i8, ptr %t11, i64 0
  store ptr %t0, ptr %t12
  %t14 = sext i32 1 to i64
  %t13 = sub i64 0, %t14
  %t15 = load ptr, ptr %t1
  %t16 = getelementptr i8, ptr %t15, i64 0
  store i64 %t13, ptr %t16
  %t17 = load ptr, ptr %t1
  ret ptr %t17
L3:
  ret ptr null
}

define dso_local ptr @type_ptr(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @type_new(i64 15)
  store ptr %t2, ptr %t1
  %t3 = load ptr, ptr %t1
  %t4 = getelementptr i8, ptr %t3, i64 0
  store ptr %t0, ptr %t4
  %t5 = load ptr, ptr %t1
  ret ptr %t5
L0:
  ret ptr null
}

define dso_local ptr @type_array(ptr %t0, i64 %t1) {
entry:
  %t2 = alloca ptr
  %t3 = call ptr @type_new(i64 16)
  store ptr %t3, ptr %t2
  %t4 = load ptr, ptr %t2
  %t5 = getelementptr i8, ptr %t4, i64 0
  store ptr %t0, ptr %t5
  %t6 = load ptr, ptr %t2
  %t7 = getelementptr i8, ptr %t6, i64 0
  store i64 %t1, ptr %t7
  %t8 = load ptr, ptr %t2
  ret ptr %t8
L0:
  ret ptr null
}

define dso_local void @type_free(ptr %t0) {
entry:
  %t2 = ptrtoint ptr %t0 to i64
  %t3 = icmp eq i64 %t2, 0
  %t1 = zext i1 %t3 to i64
  %t4 = icmp ne i64 %t1, 0
  br i1 %t4, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t5 = getelementptr i8, ptr %t0, i64 0
  %t6 = load i64, ptr %t5
  call void @free(i64 %t6)
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  call void @free(i64 %t9)
  %t11 = getelementptr i8, ptr %t0, i64 0
  %t12 = load i64, ptr %t11
  %t13 = icmp ne i64 %t12, 0
  br i1 %t13, label %L4, label %L6
L4:
  %t14 = alloca i64
  %t15 = sext i32 0 to i64
  store i64 %t15, ptr %t14
  br label %L7
L7:
  %t16 = load i64, ptr %t14
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t20 = sext i32 %t16 to i64
  %t19 = icmp slt i64 %t20, %t18
  %t21 = zext i1 %t19 to i64
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L8, label %L10
L8:
  %t23 = getelementptr i8, ptr %t0, i64 0
  %t24 = load i64, ptr %t23
  %t25 = load i64, ptr %t14
  %t27 = inttoptr i64 %t24 to ptr
  %t28 = sext i32 %t25 to i64
  %t26 = getelementptr ptr, ptr %t27, i64 %t28
  %t29 = getelementptr i8, ptr %t26, i64 0
  %t30 = load i64, ptr %t29
  call void @free(i64 %t30)
  br label %L9
L9:
  %t32 = load i64, ptr %t14
  %t34 = sext i32 %t32 to i64
  %t33 = add i64 %t34, 1
  store i64 %t33, ptr %t14
  br label %L7
L10:
  %t35 = getelementptr i8, ptr %t0, i64 0
  %t36 = load i64, ptr %t35
  call void @free(i64 %t36)
  br label %L6
L6:
  call void @free(ptr %t0)
  ret void
}

define dso_local i32 @type_is_integer(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = add i64 %t2, 0
  switch i64 %t3, label %L14 [
    i64 1, label %L1
    i64 2, label %L2
    i64 3, label %L3
    i64 4, label %L4
    i64 5, label %L5
    i64 6, label %L6
    i64 7, label %L7
    i64 8, label %L8
    i64 9, label %L9
    i64 10, label %L10
    i64 11, label %L11
    i64 12, label %L12
    i64 20, label %L13
  ]
L1:
  br label %L2
L2:
  br label %L3
L3:
  br label %L4
L4:
  br label %L5
L5:
  br label %L6
L6:
  br label %L7
L7:
  br label %L8
L8:
  br label %L9
L9:
  br label %L10
L10:
  br label %L11
L11:
  br label %L12
L12:
  br label %L13
L13:
  %t4 = sext i32 1 to i64
  %t5 = trunc i64 %t4 to i32
  ret i32 %t5
L15:
  br label %L0
L14:
  %t6 = sext i32 0 to i64
  %t7 = trunc i64 %t6 to i32
  ret i32 %t7
L16:
  br label %L0
L0:
  ret i32 0
}

define dso_local i32 @type_is_float(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t4 = sext i32 13 to i64
  %t3 = icmp eq i64 %t2, %t4
  %t5 = zext i1 %t3 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L1
L0:
  br label %L2
L1:
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = load i64, ptr %t7
  %t10 = sext i32 14 to i64
  %t9 = icmp eq i64 %t8, %t10
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  %t13 = zext i1 %t12 to i64
  br label %L2
L2:
  %t14 = phi i64 [ 1, %L0 ], [ %t13, %L1 ]
  %t15 = trunc i64 %t14 to i32
  ret i32 %t15
L3:
  ret i32 0
}

define dso_local i32 @type_is_pointer(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t4 = sext i32 15 to i64
  %t3 = icmp eq i64 %t2, %t4
  %t5 = zext i1 %t3 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L1
L0:
  br label %L2
L1:
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = load i64, ptr %t7
  %t10 = sext i32 16 to i64
  %t9 = icmp eq i64 %t8, %t10
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  %t13 = zext i1 %t12 to i64
  br label %L2
L2:
  %t14 = phi i64 [ 1, %L0 ], [ %t13, %L1 ]
  %t15 = trunc i64 %t14 to i32
  ret i32 %t15
L3:
  ret i32 0
}

define dso_local i32 @type_size(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = add i64 %t2, 0
  switch i64 %t3, label %L19 [
    i64 0, label %L1
    i64 1, label %L2
    i64 2, label %L3
    i64 3, label %L4
    i64 4, label %L5
    i64 5, label %L6
    i64 6, label %L7
    i64 7, label %L8
    i64 8, label %L9
    i64 13, label %L10
    i64 20, label %L11
    i64 9, label %L12
    i64 10, label %L13
    i64 11, label %L14
    i64 12, label %L15
    i64 14, label %L16
    i64 15, label %L17
    i64 16, label %L18
  ]
L1:
  %t4 = sext i32 0 to i64
  %t5 = trunc i64 %t4 to i32
  ret i32 %t5
L20:
  br label %L2
L2:
  br label %L3
L3:
  br label %L4
L4:
  br label %L5
L5:
  %t6 = sext i32 1 to i64
  %t7 = trunc i64 %t6 to i32
  ret i32 %t7
L21:
  br label %L6
L6:
  br label %L7
L7:
  %t8 = sext i32 2 to i64
  %t9 = trunc i64 %t8 to i32
  ret i32 %t9
L22:
  br label %L8
L8:
  br label %L9
L9:
  br label %L10
L10:
  br label %L11
L11:
  %t10 = sext i32 4 to i64
  %t11 = trunc i64 %t10 to i32
  ret i32 %t11
L23:
  br label %L12
L12:
  br label %L13
L13:
  br label %L14
L14:
  br label %L15
L15:
  br label %L16
L16:
  br label %L17
L17:
  %t12 = sext i32 8 to i64
  %t13 = trunc i64 %t12 to i32
  ret i32 %t13
L24:
  br label %L18
L18:
  %t14 = getelementptr i8, ptr %t0, i64 0
  %t15 = load i64, ptr %t14
  %t17 = sext i32 0 to i64
  %t16 = icmp slt i64 %t15, %t17
  %t18 = zext i1 %t16 to i64
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L25, label %L27
L25:
  %t20 = sext i32 0 to i64
  %t21 = trunc i64 %t20 to i32
  ret i32 %t21
L28:
  br label %L27
L27:
  %t22 = getelementptr i8, ptr %t0, i64 0
  %t23 = load i64, ptr %t22
  %t24 = getelementptr i8, ptr %t0, i64 0
  %t25 = load i64, ptr %t24
  %t26 = call i32 @type_size(i64 %t25)
  %t27 = sext i32 %t26 to i64
  %t28 = mul i64 %t23, %t27
  %t29 = add i64 %t28, 0
  %t30 = trunc i64 %t29 to i32
  ret i32 %t30
L29:
  br label %L0
L19:
  %t31 = sext i32 0 to i64
  %t32 = trunc i64 %t31 to i32
  ret i32 %t32
L30:
  br label %L0
L0:
  ret i32 0
}

@.str0 = private unnamed_addr constant [7 x i8] c"calloc\00"
@.str1 = private unnamed_addr constant [8 x i8] c"realloc\00"
@.str2 = private unnamed_addr constant [7 x i8] c"calloc\00"
