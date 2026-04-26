; ModuleID = 'macro.c'
source_filename = "macro.c"
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

@macro_names = internal global ptr zeroinitializer
@macro_bodies = internal global ptr zeroinitializer
@macro_params_0 = internal global ptr zeroinitializer
@macro_params_1 = internal global ptr zeroinitializer
@macro_params_2 = internal global ptr zeroinitializer
@macro_params_3 = internal global ptr zeroinitializer
@macro_params_4 = internal global ptr zeroinitializer
@macro_params_5 = internal global ptr zeroinitializer
@macro_params_6 = internal global ptr zeroinitializer
@macro_params_7 = internal global ptr zeroinitializer
@macro_nparams = internal global ptr zeroinitializer
@macro_funclike = internal global ptr zeroinitializer
@macro_defined = internal global ptr zeroinitializer
@n_macros = internal global i32 zeroinitializer

define internal i64 @buf_new() {
entry:
  %t0 = alloca i64
  %t1 = call ptr @calloc(i64 3, i64 8)
  store ptr %t1, ptr %t0
  %t2 = call ptr @malloc(i64 65536)
  %t3 = load i64, ptr %t0
  %t5 = inttoptr i64 %t3 to ptr
  %t6 = sext i32 0 to i64
  %t4 = getelementptr ptr, ptr %t5, i64 %t6
  store ptr %t2, ptr %t4
  %t7 = load i64, ptr %t0
  %t8 = inttoptr i64 %t7 to ptr
  %t9 = sext i32 0 to i64
  %t10 = getelementptr ptr, ptr %t8, i64 %t9
  %t11 = load ptr, ptr %t10
  %t13 = sext i32 0 to i64
  %t12 = getelementptr ptr, ptr %t11, i64 %t13
  %t14 = sext i32 0 to i64
  store i64 %t14, ptr %t12
  %t16 = sext i32 0 to i64
  %t15 = inttoptr i64 %t16 to ptr
  %t17 = load i64, ptr %t0
  %t19 = inttoptr i64 %t17 to ptr
  %t20 = sext i32 1 to i64
  %t18 = getelementptr ptr, ptr %t19, i64 %t20
  store ptr %t15, ptr %t18
  %t22 = sext i32 65536 to i64
  %t21 = inttoptr i64 %t22 to ptr
  %t23 = load i64, ptr %t0
  %t25 = inttoptr i64 %t23 to ptr
  %t26 = sext i32 2 to i64
  %t24 = getelementptr ptr, ptr %t25, i64 %t26
  store ptr %t21, ptr %t24
  %t27 = load i64, ptr %t0
  %t28 = sext i32 %t27 to i64
  ret i64 %t28
L0:
  ret i64 0
}

define internal void @buf_init(ptr %t0) {
entry:
  %t1 = call ptr @malloc(i64 65536)
  %t3 = sext i32 0 to i64
  %t2 = getelementptr ptr, ptr %t0, i64 %t3
  store ptr %t1, ptr %t2
  %t4 = sext i32 0 to i64
  %t5 = getelementptr ptr, ptr %t0, i64 %t4
  %t6 = load ptr, ptr %t5
  %t8 = ptrtoint ptr %t6 to i64
  %t9 = icmp eq i64 %t8, 0
  %t7 = zext i1 %t9 to i64
  %t10 = icmp ne i64 %t7, 0
  br i1 %t10, label %L0, label %L2
L0:
  %t11 = getelementptr [7 x i8], ptr @.str0, i64 0, i64 0
  call void @perror(ptr %t11)
  call void @exit(i64 1)
  br label %L2
L2:
  %t14 = sext i32 0 to i64
  %t15 = getelementptr ptr, ptr %t0, i64 %t14
  %t16 = load ptr, ptr %t15
  %t18 = sext i32 0 to i64
  %t17 = getelementptr ptr, ptr %t16, i64 %t18
  %t19 = sext i32 0 to i64
  store i64 %t19, ptr %t17
  %t21 = sext i32 0 to i64
  %t20 = inttoptr i64 %t21 to ptr
  %t23 = sext i32 1 to i64
  %t22 = getelementptr ptr, ptr %t0, i64 %t23
  store ptr %t20, ptr %t22
  %t25 = sext i32 65536 to i64
  %t24 = inttoptr i64 %t25 to ptr
  %t27 = sext i32 2 to i64
  %t26 = getelementptr ptr, ptr %t0, i64 %t27
  store ptr %t24, ptr %t26
  ret void
}

define internal void @buf_grow(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = sext i32 1 to i64
  %t4 = getelementptr ptr, ptr %t0, i64 %t3
  %t5 = load ptr, ptr %t4
  %t6 = ptrtoint ptr %t5 to i64
  store i64 %t6, ptr %t2
  %t7 = alloca i64
  %t8 = sext i32 2 to i64
  %t9 = getelementptr ptr, ptr %t0, i64 %t8
  %t10 = load ptr, ptr %t9
  %t11 = ptrtoint ptr %t10 to i64
  store i64 %t11, ptr %t7
  %t12 = load i64, ptr %t7
  %t14 = sext i32 %t12 to i64
  %t15 = sext i32 0 to i64
  %t13 = icmp eq i64 %t14, %t15
  %t16 = zext i1 %t13 to i64
  %t17 = icmp ne i64 %t16, 0
  br i1 %t17, label %L0, label %L2
L0:
  %t18 = sext i32 65536 to i64
  store i64 %t18, ptr %t7
  br label %L2
L2:
  br label %L3
L3:
  %t19 = load i64, ptr %t2
  %t21 = sext i32 %t19 to i64
  %t22 = ptrtoint ptr %t1 to i64
  %t20 = add i64 %t21, %t22
  %t24 = sext i32 1 to i64
  %t23 = add i64 %t20, %t24
  %t25 = load i64, ptr %t7
  %t27 = sext i32 %t25 to i64
  %t26 = icmp sgt i64 %t23, %t27
  %t28 = zext i1 %t26 to i64
  %t29 = icmp ne i64 %t28, 0
  br i1 %t29, label %L4, label %L5
L4:
  %t30 = load i64, ptr %t7
  %t32 = sext i32 %t30 to i64
  %t33 = sext i32 2 to i64
  %t31 = mul i64 %t32, %t33
  store i64 %t31, ptr %t7
  br label %L3
L5:
  %t34 = load i64, ptr %t7
  %t35 = sext i32 2 to i64
  %t36 = getelementptr ptr, ptr %t0, i64 %t35
  %t37 = load ptr, ptr %t36
  %t38 = ptrtoint ptr %t37 to i64
  %t40 = sext i32 %t34 to i64
  %t39 = icmp ne i64 %t40, %t38
  %t41 = zext i1 %t39 to i64
  %t42 = icmp ne i64 %t41, 0
  br i1 %t42, label %L6, label %L8
L6:
  %t43 = sext i32 0 to i64
  %t44 = getelementptr ptr, ptr %t0, i64 %t43
  %t45 = load ptr, ptr %t44
  %t46 = load i64, ptr %t7
  %t47 = call ptr @realloc(ptr %t45, i64 %t46)
  %t49 = sext i32 0 to i64
  %t48 = getelementptr ptr, ptr %t0, i64 %t49
  store ptr %t47, ptr %t48
  %t50 = sext i32 0 to i64
  %t51 = getelementptr ptr, ptr %t0, i64 %t50
  %t52 = load ptr, ptr %t51
  %t54 = ptrtoint ptr %t52 to i64
  %t55 = icmp eq i64 %t54, 0
  %t53 = zext i1 %t55 to i64
  %t56 = icmp ne i64 %t53, 0
  br i1 %t56, label %L9, label %L11
L9:
  %t57 = getelementptr [8 x i8], ptr @.str1, i64 0, i64 0
  call void @perror(ptr %t57)
  call void @exit(i64 1)
  br label %L11
L11:
  %t60 = load i64, ptr %t7
  %t62 = sext i32 %t60 to i64
  %t61 = add i64 %t62, 0
  %t63 = inttoptr i64 %t61 to ptr
  %t65 = sext i32 2 to i64
  %t64 = getelementptr ptr, ptr %t0, i64 %t65
  store ptr %t63, ptr %t64
  br label %L8
L8:
  ret void
}

define internal void @buf_append(ptr %t0, ptr %t1, ptr %t2) {
entry:
  call void @buf_grow(ptr %t0, ptr %t2)
  %t4 = alloca i64
  %t5 = sext i32 1 to i64
  %t6 = getelementptr ptr, ptr %t0, i64 %t5
  %t7 = load ptr, ptr %t6
  %t8 = ptrtoint ptr %t7 to i64
  store i64 %t8, ptr %t4
  %t9 = sext i32 0 to i64
  %t10 = getelementptr ptr, ptr %t0, i64 %t9
  %t11 = load ptr, ptr %t10
  %t12 = load i64, ptr %t4
  %t14 = ptrtoint ptr %t11 to i64
  %t15 = sext i32 %t12 to i64
  %t16 = inttoptr i64 %t14 to ptr
  %t13 = getelementptr i8, ptr %t16, i64 %t15
  %t17 = call ptr @memcpy(ptr %t13, ptr %t1, ptr %t2)
  %t18 = load i64, ptr %t4
  %t20 = sext i32 %t18 to i64
  %t21 = ptrtoint ptr %t2 to i64
  %t19 = add i64 %t20, %t21
  %t22 = add i64 %t19, 0
  %t23 = inttoptr i64 %t22 to ptr
  %t25 = sext i32 1 to i64
  %t24 = getelementptr ptr, ptr %t0, i64 %t25
  store ptr %t23, ptr %t24
  %t26 = sext i32 0 to i64
  %t27 = getelementptr ptr, ptr %t0, i64 %t26
  %t28 = load ptr, ptr %t27
  %t29 = load i64, ptr %t4
  %t31 = sext i32 %t29 to i64
  %t32 = ptrtoint ptr %t2 to i64
  %t30 = add i64 %t31, %t32
  %t33 = getelementptr ptr, ptr %t28, i64 %t30
  %t34 = sext i32 0 to i64
  store i64 %t34, ptr %t33
  ret void
}

define internal void @buf_putc(ptr %t0, i64 %t1) {
entry:
  call void @buf_grow(ptr %t0, i64 1)
  %t3 = alloca i64
  %t4 = sext i32 1 to i64
  %t5 = getelementptr ptr, ptr %t0, i64 %t4
  %t6 = load ptr, ptr %t5
  %t7 = ptrtoint ptr %t6 to i64
  store i64 %t7, ptr %t3
  %t8 = sext i32 0 to i64
  %t9 = getelementptr ptr, ptr %t0, i64 %t8
  %t10 = load ptr, ptr %t9
  %t11 = load i64, ptr %t3
  %t13 = sext i32 %t11 to i64
  %t12 = getelementptr ptr, ptr %t10, i64 %t13
  store i64 %t1, ptr %t12
  %t14 = load i64, ptr %t3
  %t16 = sext i32 %t14 to i64
  %t17 = sext i32 1 to i64
  %t15 = add i64 %t16, %t17
  %t18 = add i64 %t15, 0
  %t19 = inttoptr i64 %t18 to ptr
  %t21 = sext i32 1 to i64
  %t20 = getelementptr ptr, ptr %t0, i64 %t21
  store ptr %t19, ptr %t20
  %t22 = sext i32 0 to i64
  %t23 = getelementptr ptr, ptr %t0, i64 %t22
  %t24 = load ptr, ptr %t23
  %t25 = load i64, ptr %t3
  %t27 = sext i32 %t25 to i64
  %t28 = sext i32 1 to i64
  %t26 = add i64 %t27, %t28
  %t29 = getelementptr ptr, ptr %t24, i64 %t26
  %t30 = sext i32 0 to i64
  store i64 %t30, ptr %t29
  ret void
}

define internal void @macros_init() {
entry:
  %t0 = load ptr, ptr @macro_names
  %t1 = icmp ne ptr %t0, null
  br i1 %t1, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t2 = call ptr @calloc(i64 512, i64 8)
  store ptr %t2, ptr @macro_names
  %t3 = call ptr @calloc(i64 512, i64 8)
  store ptr %t3, ptr @macro_bodies
  %t4 = call ptr @calloc(i64 512, i64 8)
  store ptr %t4, ptr @macro_params_0
  %t5 = call ptr @calloc(i64 512, i64 8)
  store ptr %t5, ptr @macro_params_1
  %t6 = call ptr @calloc(i64 512, i64 8)
  store ptr %t6, ptr @macro_params_2
  %t7 = call ptr @calloc(i64 512, i64 8)
  store ptr %t7, ptr @macro_params_3
  %t8 = call ptr @calloc(i64 512, i64 8)
  store ptr %t8, ptr @macro_params_4
  %t9 = call ptr @calloc(i64 512, i64 8)
  store ptr %t9, ptr @macro_params_5
  %t10 = call ptr @calloc(i64 512, i64 8)
  store ptr %t10, ptr @macro_params_6
  %t11 = call ptr @calloc(i64 512, i64 8)
  store ptr %t11, ptr @macro_params_7
  %t12 = call ptr @calloc(i64 512, i64 4)
  store ptr %t12, ptr @macro_nparams
  %t13 = call ptr @calloc(i64 512, i64 4)
  store ptr %t13, ptr @macro_funclike
  %t14 = call ptr @calloc(i64 512, i64 4)
  store ptr %t14, ptr @macro_defined
  ret void
}

define internal ptr @macro_get_param(i64 %t0, i64 %t1) {
entry:
  %t3 = sext i32 0 to i64
  %t2 = icmp eq i64 %t1, %t3
  %t4 = zext i1 %t2 to i64
  %t5 = icmp ne i64 %t4, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = load ptr, ptr @macro_params_0
  %t7 = getelementptr ptr, ptr %t6, i64 %t0
  %t8 = load ptr, ptr %t7
  ret ptr %t8
L3:
  br label %L2
L2:
  %t10 = sext i32 1 to i64
  %t9 = icmp eq i64 %t1, %t10
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L4, label %L6
L4:
  %t13 = load ptr, ptr @macro_params_1
  %t14 = getelementptr ptr, ptr %t13, i64 %t0
  %t15 = load ptr, ptr %t14
  ret ptr %t15
L7:
  br label %L6
L6:
  %t17 = sext i32 2 to i64
  %t16 = icmp eq i64 %t1, %t17
  %t18 = zext i1 %t16 to i64
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L8, label %L10
L8:
  %t20 = load ptr, ptr @macro_params_2
  %t21 = getelementptr ptr, ptr %t20, i64 %t0
  %t22 = load ptr, ptr %t21
  ret ptr %t22
L11:
  br label %L10
L10:
  %t24 = sext i32 3 to i64
  %t23 = icmp eq i64 %t1, %t24
  %t25 = zext i1 %t23 to i64
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L12, label %L14
L12:
  %t27 = load ptr, ptr @macro_params_3
  %t28 = getelementptr ptr, ptr %t27, i64 %t0
  %t29 = load ptr, ptr %t28
  ret ptr %t29
L15:
  br label %L14
L14:
  %t31 = sext i32 4 to i64
  %t30 = icmp eq i64 %t1, %t31
  %t32 = zext i1 %t30 to i64
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L16, label %L18
L16:
  %t34 = load ptr, ptr @macro_params_4
  %t35 = getelementptr ptr, ptr %t34, i64 %t0
  %t36 = load ptr, ptr %t35
  ret ptr %t36
L19:
  br label %L18
L18:
  %t38 = sext i32 5 to i64
  %t37 = icmp eq i64 %t1, %t38
  %t39 = zext i1 %t37 to i64
  %t40 = icmp ne i64 %t39, 0
  br i1 %t40, label %L20, label %L22
L20:
  %t41 = load ptr, ptr @macro_params_5
  %t42 = getelementptr ptr, ptr %t41, i64 %t0
  %t43 = load ptr, ptr %t42
  ret ptr %t43
L23:
  br label %L22
L22:
  %t45 = sext i32 6 to i64
  %t44 = icmp eq i64 %t1, %t45
  %t46 = zext i1 %t44 to i64
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L24, label %L26
L24:
  %t48 = load ptr, ptr @macro_params_6
  %t49 = getelementptr ptr, ptr %t48, i64 %t0
  %t50 = load ptr, ptr %t49
  ret ptr %t50
L27:
  br label %L26
L26:
  %t52 = sext i32 7 to i64
  %t51 = icmp eq i64 %t1, %t52
  %t53 = zext i1 %t51 to i64
  %t54 = icmp ne i64 %t53, 0
  br i1 %t54, label %L28, label %L30
L28:
  %t55 = load ptr, ptr @macro_params_7
  %t56 = getelementptr ptr, ptr %t55, i64 %t0
  %t57 = load ptr, ptr %t56
  ret ptr %t57
L31:
  br label %L30
L30:
  %t59 = sext i32 0 to i64
  %t58 = inttoptr i64 %t59 to ptr
  ret ptr %t58
L32:
  ret ptr null
}

define internal void @macro_set_param(i64 %t0, i64 %t1, ptr %t2) {
entry:
  %t4 = sext i32 0 to i64
  %t3 = icmp eq i64 %t1, %t4
  %t5 = zext i1 %t3 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L1
L0:
  %t7 = load ptr, ptr @macro_params_0
  %t8 = getelementptr ptr, ptr %t7, i64 %t0
  store ptr %t2, ptr %t8
  br label %L2
L1:
  %t10 = sext i32 1 to i64
  %t9 = icmp eq i64 %t1, %t10
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L3, label %L4
L3:
  %t13 = load ptr, ptr @macro_params_1
  %t14 = getelementptr ptr, ptr %t13, i64 %t0
  store ptr %t2, ptr %t14
  br label %L5
L4:
  %t16 = sext i32 2 to i64
  %t15 = icmp eq i64 %t1, %t16
  %t17 = zext i1 %t15 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L6, label %L7
L6:
  %t19 = load ptr, ptr @macro_params_2
  %t20 = getelementptr ptr, ptr %t19, i64 %t0
  store ptr %t2, ptr %t20
  br label %L8
L7:
  %t22 = sext i32 3 to i64
  %t21 = icmp eq i64 %t1, %t22
  %t23 = zext i1 %t21 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L9, label %L10
L9:
  %t25 = load ptr, ptr @macro_params_3
  %t26 = getelementptr ptr, ptr %t25, i64 %t0
  store ptr %t2, ptr %t26
  br label %L11
L10:
  %t28 = sext i32 4 to i64
  %t27 = icmp eq i64 %t1, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L12, label %L13
L12:
  %t31 = load ptr, ptr @macro_params_4
  %t32 = getelementptr ptr, ptr %t31, i64 %t0
  store ptr %t2, ptr %t32
  br label %L14
L13:
  %t34 = sext i32 5 to i64
  %t33 = icmp eq i64 %t1, %t34
  %t35 = zext i1 %t33 to i64
  %t36 = icmp ne i64 %t35, 0
  br i1 %t36, label %L15, label %L16
L15:
  %t37 = load ptr, ptr @macro_params_5
  %t38 = getelementptr ptr, ptr %t37, i64 %t0
  store ptr %t2, ptr %t38
  br label %L17
L16:
  %t40 = sext i32 6 to i64
  %t39 = icmp eq i64 %t1, %t40
  %t41 = zext i1 %t39 to i64
  %t42 = icmp ne i64 %t41, 0
  br i1 %t42, label %L18, label %L19
L18:
  %t43 = load ptr, ptr @macro_params_6
  %t44 = getelementptr ptr, ptr %t43, i64 %t0
  store ptr %t2, ptr %t44
  br label %L20
L19:
  %t46 = sext i32 7 to i64
  %t45 = icmp eq i64 %t1, %t46
  %t47 = zext i1 %t45 to i64
  %t48 = icmp ne i64 %t47, 0
  br i1 %t48, label %L21, label %L23
L21:
  %t49 = load ptr, ptr @macro_params_7
  %t50 = getelementptr ptr, ptr %t49, i64 %t0
  store ptr %t2, ptr %t50
  br label %L23
L23:
  br label %L20
L20:
  br label %L17
L17:
  br label %L14
L14:
  br label %L11
L11:
  br label %L8
L8:
  br label %L5
L5:
  br label %L2
L2:
  ret void
}

define internal i32 @macro_find_idx(ptr %t0) {
entry:
  %t1 = load ptr, ptr @macro_names
  %t3 = ptrtoint ptr %t1 to i64
  %t4 = icmp eq i64 %t3, 0
  %t2 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t2, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t7 = sext i32 1 to i64
  %t6 = sub i64 0, %t7
  %t8 = trunc i64 %t6 to i32
  ret i32 %t8
L3:
  br label %L2
L2:
  %t9 = alloca i64
  %t10 = sext i32 0 to i64
  store i64 %t10, ptr %t9
  br label %L4
L4:
  %t11 = load i64, ptr %t9
  %t12 = load i64, ptr @n_macros
  %t14 = sext i32 %t11 to i64
  %t15 = sext i32 %t12 to i64
  %t13 = icmp slt i64 %t14, %t15
  %t16 = zext i1 %t13 to i64
  %t17 = icmp ne i64 %t16, 0
  br i1 %t17, label %L5, label %L7
L5:
  %t18 = load ptr, ptr @macro_defined
  %t19 = load i64, ptr %t9
  %t20 = sext i32 %t19 to i64
  %t21 = getelementptr ptr, ptr %t18, i64 %t20
  %t22 = load ptr, ptr %t21
  %t23 = ptrtoint ptr %t22 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L8, label %L9
L8:
  %t25 = load ptr, ptr @macro_names
  %t26 = load i64, ptr %t9
  %t27 = sext i32 %t26 to i64
  %t28 = getelementptr ptr, ptr %t25, i64 %t27
  %t29 = load ptr, ptr %t28
  %t30 = ptrtoint ptr %t29 to i64
  %t31 = icmp ne i64 %t30, 0
  %t32 = zext i1 %t31 to i64
  br label %L10
L9:
  br label %L10
L10:
  %t33 = phi i64 [ %t32, %L8 ], [ 0, %L9 ]
  %t34 = icmp ne i64 %t33, 0
  br i1 %t34, label %L11, label %L12
L11:
  %t35 = load ptr, ptr @macro_names
  %t36 = load i64, ptr %t9
  %t37 = sext i32 %t36 to i64
  %t38 = getelementptr ptr, ptr %t35, i64 %t37
  %t39 = load ptr, ptr %t38
  %t40 = call i32 @strcmp(ptr %t39, ptr %t0)
  %t41 = sext i32 %t40 to i64
  %t43 = sext i32 0 to i64
  %t42 = icmp eq i64 %t41, %t43
  %t44 = zext i1 %t42 to i64
  %t45 = icmp ne i64 %t44, 0
  %t46 = zext i1 %t45 to i64
  br label %L13
L12:
  br label %L13
L13:
  %t47 = phi i64 [ %t46, %L11 ], [ 0, %L12 ]
  %t48 = icmp ne i64 %t47, 0
  br i1 %t48, label %L14, label %L16
L14:
  %t49 = load i64, ptr %t9
  %t50 = sext i32 %t49 to i64
  %t51 = trunc i64 %t50 to i32
  ret i32 %t51
L17:
  br label %L16
L16:
  br label %L6
L6:
  %t52 = load i64, ptr %t9
  %t54 = sext i32 %t52 to i64
  %t53 = add i64 %t54, 1
  store i64 %t53, ptr %t9
  br label %L4
L7:
  %t56 = sext i32 1 to i64
  %t55 = sub i64 0, %t56
  %t57 = trunc i64 %t55 to i32
  ret i32 %t57
L18:
  ret i32 0
}

define internal ptr @macro_find_body(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = call i32 @macro_find_idx(ptr %t0)
  %t3 = sext i32 %t2 to i64
  store i64 %t3, ptr %t1
  %t4 = load i64, ptr %t1
  %t6 = sext i32 %t4 to i64
  %t7 = sext i32 0 to i64
  %t5 = icmp sge i64 %t6, %t7
  %t8 = zext i1 %t5 to i64
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L0, label %L1
L0:
  %t10 = load ptr, ptr @macro_bodies
  %t11 = load i64, ptr %t1
  %t12 = sext i32 %t11 to i64
  %t13 = getelementptr ptr, ptr %t10, i64 %t12
  %t14 = load ptr, ptr %t13
  %t15 = ptrtoint ptr %t14 to i64
  br label %L2
L1:
  %t17 = sext i32 0 to i64
  %t16 = inttoptr i64 %t17 to ptr
  %t18 = ptrtoint ptr %t16 to i64
  br label %L2
L2:
  %t19 = phi i64 [ %t15, %L0 ], [ %t18, %L1 ]
  %t20 = inttoptr i64 %t19 to ptr
  ret ptr %t20
L3:
  ret ptr null
}

define internal void @macro_define(ptr %t0, ptr %t1, ptr %t2, i64 %t3, i64 %t4) {
entry:
  %t5 = load ptr, ptr @macro_names
  %t7 = ptrtoint ptr %t5 to i64
  %t8 = icmp eq i64 %t7, 0
  %t6 = zext i1 %t8 to i64
  %t9 = icmp ne i64 %t6, 0
  br i1 %t9, label %L0, label %L2
L0:
  call void @macros_init()
  br label %L2
L2:
  %t11 = alloca i64
  %t12 = call i32 @macro_find_idx(ptr %t0)
  %t13 = sext i32 %t12 to i64
  store i64 %t13, ptr %t11
  %t14 = load i64, ptr %t11
  %t16 = sext i32 %t14 to i64
  %t17 = sext i32 0 to i64
  %t15 = icmp slt i64 %t16, %t17
  %t18 = zext i1 %t15 to i64
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L3, label %L5
L3:
  %t20 = alloca i64
  %t21 = sext i32 0 to i64
  store i64 %t21, ptr %t20
  br label %L6
L6:
  %t22 = load i64, ptr %t20
  %t23 = load i64, ptr @n_macros
  %t25 = sext i32 %t22 to i64
  %t26 = sext i32 %t23 to i64
  %t24 = icmp slt i64 %t25, %t26
  %t27 = zext i1 %t24 to i64
  %t28 = icmp ne i64 %t27, 0
  br i1 %t28, label %L7, label %L9
L7:
  %t29 = load ptr, ptr @macro_names
  %t30 = load i64, ptr %t20
  %t31 = sext i32 %t30 to i64
  %t32 = getelementptr ptr, ptr %t29, i64 %t31
  %t33 = load ptr, ptr %t32
  %t34 = ptrtoint ptr %t33 to i64
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L10, label %L11
L10:
  %t36 = load ptr, ptr @macro_names
  %t37 = load i64, ptr %t20
  %t38 = sext i32 %t37 to i64
  %t39 = getelementptr ptr, ptr %t36, i64 %t38
  %t40 = load ptr, ptr %t39
  %t41 = call i32 @strcmp(ptr %t40, ptr %t0)
  %t42 = sext i32 %t41 to i64
  %t44 = sext i32 0 to i64
  %t43 = icmp eq i64 %t42, %t44
  %t45 = zext i1 %t43 to i64
  %t46 = icmp ne i64 %t45, 0
  %t47 = zext i1 %t46 to i64
  br label %L12
L11:
  br label %L12
L12:
  %t48 = phi i64 [ %t47, %L10 ], [ 0, %L11 ]
  %t49 = icmp ne i64 %t48, 0
  br i1 %t49, label %L13, label %L15
L13:
  %t50 = load i64, ptr %t20
  %t51 = sext i32 %t50 to i64
  store i64 %t51, ptr %t11
  br label %L9
L16:
  br label %L15
L15:
  br label %L8
L8:
  %t52 = load i64, ptr %t20
  %t54 = sext i32 %t52 to i64
  %t53 = add i64 %t54, 1
  store i64 %t53, ptr %t20
  br label %L6
L9:
  br label %L5
L5:
  %t55 = load i64, ptr %t11
  %t57 = sext i32 %t55 to i64
  %t58 = sext i32 0 to i64
  %t56 = icmp sge i64 %t57, %t58
  %t59 = zext i1 %t56 to i64
  %t60 = icmp ne i64 %t59, 0
  br i1 %t60, label %L17, label %L19
L17:
  %t61 = load ptr, ptr @macro_bodies
  %t62 = load i64, ptr %t11
  %t63 = sext i32 %t62 to i64
  %t64 = getelementptr ptr, ptr %t61, i64 %t63
  %t65 = load ptr, ptr %t64
  call void @free(ptr %t65)
  %t67 = call ptr @strdup(ptr %t1)
  %t68 = load ptr, ptr @macro_bodies
  %t69 = load i64, ptr %t11
  %t71 = sext i32 %t69 to i64
  %t70 = getelementptr ptr, ptr %t68, i64 %t71
  store ptr %t67, ptr %t70
  %t72 = load ptr, ptr @macro_defined
  %t73 = load i64, ptr %t11
  %t75 = sext i32 %t73 to i64
  %t74 = getelementptr ptr, ptr %t72, i64 %t75
  %t76 = sext i32 1 to i64
  store i64 %t76, ptr %t74
  ret void
L20:
  br label %L19
L19:
  %t77 = load i64, ptr @n_macros
  %t79 = sext i32 %t77 to i64
  %t80 = sext i32 512 to i64
  %t78 = icmp sge i64 %t79, %t80
  %t81 = zext i1 %t78 to i64
  %t82 = icmp ne i64 %t81, 0
  br i1 %t82, label %L21, label %L23
L21:
  %t83 = call ptr @__c0c_stderr()
  %t84 = getelementptr [18 x i8], ptr @.str2, i64 0, i64 0
  %t85 = call i32 (ptr, ...) @fprintf(ptr %t83, ptr %t84)
  %t86 = sext i32 %t85 to i64
  ret void
L24:
  br label %L23
L23:
  %t87 = load i64, ptr @n_macros
  %t89 = sext i32 %t87 to i64
  %t88 = add i64 %t89, 1
  store i64 %t88, ptr @n_macros
  %t90 = sext i32 %t87 to i64
  store i64 %t90, ptr %t11
  %t91 = call ptr @strdup(ptr %t0)
  %t92 = load ptr, ptr @macro_names
  %t93 = load i64, ptr %t11
  %t95 = sext i32 %t93 to i64
  %t94 = getelementptr ptr, ptr %t92, i64 %t95
  store ptr %t91, ptr %t94
  %t96 = call ptr @strdup(ptr %t1)
  %t97 = load ptr, ptr @macro_bodies
  %t98 = load i64, ptr %t11
  %t100 = sext i32 %t98 to i64
  %t99 = getelementptr ptr, ptr %t97, i64 %t100
  store ptr %t96, ptr %t99
  %t101 = load ptr, ptr @macro_nparams
  %t102 = load i64, ptr %t11
  %t104 = sext i32 %t102 to i64
  %t103 = getelementptr ptr, ptr %t101, i64 %t104
  store i64 %t3, ptr %t103
  %t105 = load ptr, ptr @macro_funclike
  %t106 = load i64, ptr %t11
  %t108 = sext i32 %t106 to i64
  %t107 = getelementptr ptr, ptr %t105, i64 %t108
  store i64 %t4, ptr %t107
  %t109 = load ptr, ptr @macro_defined
  %t110 = load i64, ptr %t11
  %t112 = sext i32 %t110 to i64
  %t111 = getelementptr ptr, ptr %t109, i64 %t112
  %t113 = sext i32 1 to i64
  store i64 %t113, ptr %t111
  %t114 = alloca i64
  %t115 = sext i32 0 to i64
  store i64 %t115, ptr %t114
  br label %L25
L25:
  %t116 = load i64, ptr %t114
  %t118 = sext i32 %t116 to i64
  %t117 = icmp slt i64 %t118, %t3
  %t119 = zext i1 %t117 to i64
  %t120 = icmp ne i64 %t119, 0
  br i1 %t120, label %L29, label %L30
L29:
  %t121 = load i64, ptr %t114
  %t123 = sext i32 %t121 to i64
  %t124 = sext i32 16 to i64
  %t122 = icmp slt i64 %t123, %t124
  %t125 = zext i1 %t122 to i64
  %t126 = icmp ne i64 %t125, 0
  %t127 = zext i1 %t126 to i64
  br label %L31
L30:
  br label %L31
L31:
  %t128 = phi i64 [ %t127, %L29 ], [ 0, %L30 ]
  %t129 = icmp ne i64 %t128, 0
  br i1 %t129, label %L26, label %L28
L26:
  %t130 = load i64, ptr %t11
  %t131 = load i64, ptr %t114
  %t132 = load i64, ptr %t114
  %t133 = sext i32 %t132 to i64
  %t134 = getelementptr ptr, ptr %t2, i64 %t133
  %t135 = load ptr, ptr %t134
  %t136 = icmp ne ptr %t135, null
  br i1 %t136, label %L32, label %L33
L32:
  %t137 = load i64, ptr %t114
  %t138 = sext i32 %t137 to i64
  %t139 = getelementptr ptr, ptr %t2, i64 %t138
  %t140 = load ptr, ptr %t139
  %t141 = call ptr @strdup(ptr %t140)
  %t142 = ptrtoint ptr %t141 to i64
  br label %L34
L33:
  %t144 = sext i32 0 to i64
  %t143 = inttoptr i64 %t144 to ptr
  %t145 = ptrtoint ptr %t143 to i64
  br label %L34
L34:
  %t146 = phi i64 [ %t142, %L32 ], [ %t145, %L33 ]
  call void @macro_set_param(i64 %t130, i64 %t131, i64 %t146)
  br label %L27
L27:
  %t148 = load i64, ptr %t114
  %t150 = sext i32 %t148 to i64
  %t149 = add i64 %t150, 1
  store i64 %t149, ptr %t114
  br label %L25
L28:
  ret void
}

define internal void @macro_undef(ptr %t0) {
entry:
  %t1 = load ptr, ptr @macro_names
  %t3 = ptrtoint ptr %t1 to i64
  %t4 = icmp eq i64 %t3, 0
  %t2 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t2, 0
  br i1 %t5, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t6 = alloca i64
  %t7 = call i32 @macro_find_idx(ptr %t0)
  %t8 = sext i32 %t7 to i64
  store i64 %t8, ptr %t6
  %t9 = load i64, ptr %t6
  %t11 = sext i32 %t9 to i64
  %t12 = sext i32 0 to i64
  %t10 = icmp sge i64 %t11, %t12
  %t13 = zext i1 %t10 to i64
  %t14 = icmp ne i64 %t13, 0
  br i1 %t14, label %L4, label %L6
L4:
  %t15 = load ptr, ptr @macro_defined
  %t16 = load i64, ptr %t6
  %t18 = sext i32 %t16 to i64
  %t17 = getelementptr ptr, ptr %t15, i64 %t18
  %t19 = sext i32 0 to i64
  store i64 %t19, ptr %t17
  br label %L6
L6:
  ret void
}

define internal i64 @macro_find_ref(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = sext i32 0 to i64
  store i64 %t2, ptr %t1
  %t3 = alloca i64
  %t4 = call i32 @macro_find_idx(ptr %t0)
  %t5 = sext i32 %t4 to i64
  store i64 %t5, ptr %t3
  %t6 = load i64, ptr %t3
  %t8 = sext i32 %t6 to i64
  %t9 = sext i32 0 to i64
  %t7 = icmp sge i64 %t8, %t9
  %t10 = zext i1 %t7 to i64
  %t11 = icmp ne i64 %t10, 0
  br i1 %t11, label %L0, label %L2
L0:
  %t12 = load ptr, ptr @macro_names
  %t13 = load i64, ptr %t3
  %t14 = sext i32 %t13 to i64
  %t15 = getelementptr ptr, ptr %t12, i64 %t14
  %t16 = load ptr, ptr %t15
  %t17 = getelementptr i8, ptr %t1, i64 0
  store ptr %t16, ptr %t17
  %t18 = load ptr, ptr @macro_bodies
  %t19 = load i64, ptr %t3
  %t20 = sext i32 %t19 to i64
  %t21 = getelementptr ptr, ptr %t18, i64 %t20
  %t22 = load ptr, ptr %t21
  %t23 = getelementptr i8, ptr %t1, i64 0
  store ptr %t22, ptr %t23
  %t24 = load ptr, ptr @macro_nparams
  %t25 = load i64, ptr %t3
  %t26 = sext i32 %t25 to i64
  %t27 = getelementptr ptr, ptr %t24, i64 %t26
  %t28 = load ptr, ptr %t27
  %t29 = getelementptr i8, ptr %t1, i64 0
  store ptr %t28, ptr %t29
  %t30 = load ptr, ptr @macro_funclike
  %t31 = load i64, ptr %t3
  %t32 = sext i32 %t31 to i64
  %t33 = getelementptr ptr, ptr %t30, i64 %t32
  %t34 = load ptr, ptr %t33
  %t35 = getelementptr i8, ptr %t1, i64 0
  store ptr %t34, ptr %t35
  %t36 = load ptr, ptr @macro_defined
  %t37 = load i64, ptr %t3
  %t38 = sext i32 %t37 to i64
  %t39 = getelementptr ptr, ptr %t36, i64 %t38
  %t40 = load ptr, ptr %t39
  %t41 = getelementptr i8, ptr %t1, i64 0
  store ptr %t40, ptr %t41
  %t42 = load i64, ptr %t3
  %t43 = getelementptr i8, ptr %t1, i64 0
  %t44 = sext i32 %t42 to i64
  store i64 %t44, ptr %t43
  br label %L2
L2:
  %t45 = load i64, ptr %t1
  %t46 = sext i32 %t45 to i64
  ret i64 %t46
L3:
  ret i64 0
}

define internal ptr @skip_ws(ptr %t0) {
entry:
  br label %L0
L0:
  %t1 = load i64, ptr %t0
  %t3 = sext i32 32 to i64
  %t2 = icmp eq i64 %t1, %t3
  %t4 = zext i1 %t2 to i64
  %t5 = icmp ne i64 %t4, 0
  br i1 %t5, label %L3, label %L4
L3:
  br label %L5
L4:
  %t6 = load i64, ptr %t0
  %t8 = sext i32 9 to i64
  %t7 = icmp eq i64 %t6, %t8
  %t9 = zext i1 %t7 to i64
  %t10 = icmp ne i64 %t9, 0
  %t11 = zext i1 %t10 to i64
  br label %L5
L5:
  %t12 = phi i64 [ 1, %L3 ], [ %t11, %L4 ]
  %t13 = icmp ne i64 %t12, 0
  br i1 %t13, label %L1, label %L2
L1:
  %t15 = ptrtoint ptr %t0 to i64
  %t14 = add i64 %t15, 1
  store i64 %t14, ptr %t0
  br label %L0
L2:
  ret ptr %t0
L6:
  ret ptr null
}

define internal ptr @skip_to_eol(ptr %t0) {
entry:
  br label %L0
L0:
  %t1 = load i64, ptr %t0
  %t2 = icmp ne i64 %t1, 0
  br i1 %t2, label %L3, label %L4
L3:
  %t3 = load i64, ptr %t0
  %t5 = sext i32 10 to i64
  %t4 = icmp ne i64 %t3, %t5
  %t6 = zext i1 %t4 to i64
  %t7 = icmp ne i64 %t6, 0
  %t8 = zext i1 %t7 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t9 = phi i64 [ %t8, %L3 ], [ 0, %L4 ]
  %t10 = icmp ne i64 %t9, 0
  br i1 %t10, label %L1, label %L2
L1:
  %t12 = ptrtoint ptr %t0 to i64
  %t11 = add i64 %t12, 1
  store i64 %t11, ptr %t0
  br label %L0
L2:
  ret ptr %t0
L6:
  ret ptr null
}

define internal ptr @read_ident(ptr %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = alloca i64
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  %t5 = load i64, ptr %t0
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L3, label %L4
L3:
  %t7 = load i64, ptr %t0
  %t8 = add i64 %t7, 0
  %t9 = call i32 @isalnum(i64 %t8)
  %t10 = sext i32 %t9 to i64
  %t11 = icmp ne i64 %t10, 0
  br i1 %t11, label %L6, label %L7
L6:
  br label %L8
L7:
  %t12 = load i64, ptr %t0
  %t14 = sext i32 95 to i64
  %t13 = icmp eq i64 %t12, %t14
  %t15 = zext i1 %t13 to i64
  %t16 = icmp ne i64 %t15, 0
  %t17 = zext i1 %t16 to i64
  br label %L8
L8:
  %t18 = phi i64 [ 1, %L6 ], [ %t17, %L7 ]
  %t19 = icmp ne i64 %t18, 0
  %t20 = zext i1 %t19 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t21 = phi i64 [ %t20, %L3 ], [ 0, %L4 ]
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L9, label %L10
L9:
  %t23 = load i64, ptr %t3
  %t25 = ptrtoint ptr %t2 to i64
  %t26 = sext i32 1 to i64
  %t24 = sub i64 %t25, %t26
  %t28 = sext i32 %t23 to i64
  %t27 = icmp slt i64 %t28, %t24
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  %t31 = zext i1 %t30 to i64
  br label %L11
L10:
  br label %L11
L11:
  %t32 = phi i64 [ %t31, %L9 ], [ 0, %L10 ]
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L1, label %L2
L1:
  %t35 = ptrtoint ptr %t0 to i64
  %t34 = add i64 %t35, 1
  store i64 %t34, ptr %t0
  %t36 = load i64, ptr %t0
  %t37 = load i64, ptr %t3
  %t39 = sext i32 %t37 to i64
  %t38 = add i64 %t39, 1
  store i64 %t38, ptr %t3
  %t41 = sext i32 %t37 to i64
  %t40 = getelementptr ptr, ptr %t1, i64 %t41
  store i64 %t36, ptr %t40
  br label %L0
L2:
  %t42 = load i64, ptr %t3
  %t44 = sext i32 %t42 to i64
  %t43 = getelementptr ptr, ptr %t1, i64 %t44
  %t45 = sext i32 0 to i64
  store i64 %t45, ptr %t43
  ret ptr %t0
L12:
  ret ptr null
}

define internal void @expand_func_macro(i64 %t0, ptr %t1, ptr %t2, i64 %t3) {
entry:
  %t4 = alloca ptr
  %t5 = load ptr, ptr @macro_names
  %t6 = getelementptr ptr, ptr %t5, i64 %t0
  %t7 = load ptr, ptr %t6
  store ptr %t7, ptr %t4
  %t8 = alloca ptr
  %t9 = load ptr, ptr @macro_bodies
  %t10 = getelementptr ptr, ptr %t9, i64 %t0
  %t11 = load ptr, ptr %t10
  store ptr %t11, ptr %t8
  %t12 = alloca i64
  %t13 = load ptr, ptr @macro_nparams
  %t14 = getelementptr ptr, ptr %t13, i64 %t0
  %t15 = load ptr, ptr %t14
  store ptr %t15, ptr %t12
  %t16 = alloca i64
  %t17 = load i64, ptr %t12
  %t19 = sext i32 %t17 to i64
  %t20 = sext i32 0 to i64
  %t18 = icmp sgt i64 %t19, %t20
  %t21 = zext i1 %t18 to i64
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L0, label %L1
L0:
  %t23 = load i64, ptr %t12
  %t25 = sext i32 %t23 to i64
  %t26 = sext i32 1 to i64
  %t24 = sub i64 %t25, %t26
  %t27 = call ptr @macro_get_param(i64 %t0, i64 %t24)
  %t28 = ptrtoint ptr %t27 to i64
  %t29 = icmp ne i64 %t28, 0
  %t30 = zext i1 %t29 to i64
  br label %L2
L1:
  br label %L2
L2:
  %t31 = phi i64 [ %t30, %L0 ], [ 0, %L1 ]
  %t32 = icmp ne i64 %t31, 0
  br i1 %t32, label %L3, label %L4
L3:
  %t33 = load i64, ptr %t12
  %t35 = sext i32 %t33 to i64
  %t36 = sext i32 1 to i64
  %t34 = sub i64 %t35, %t36
  %t37 = call ptr @macro_get_param(i64 %t0, i64 %t34)
  %t38 = getelementptr [4 x i8], ptr @.str3, i64 0, i64 0
  %t39 = call i32 @strcmp(ptr %t37, ptr %t38)
  %t40 = sext i32 %t39 to i64
  %t42 = sext i32 0 to i64
  %t41 = icmp eq i64 %t40, %t42
  %t43 = zext i1 %t41 to i64
  %t44 = icmp ne i64 %t43, 0
  %t45 = zext i1 %t44 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t46 = phi i64 [ %t45, %L3 ], [ 0, %L4 ]
  store i64 %t46, ptr %t16
  %t47 = alloca ptr
  %t48 = load i64, ptr %t1
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t47
  %t50 = call ptr @skip_ws(ptr %t49)
  store ptr %t50, ptr %t47
  %t51 = load ptr, ptr %t47
  %t52 = load i64, ptr %t51
  %t54 = sext i32 40 to i64
  %t53 = icmp ne i64 %t52, %t54
  %t55 = zext i1 %t53 to i64
  %t56 = icmp ne i64 %t55, 0
  br i1 %t56, label %L6, label %L8
L6:
  %t57 = load ptr, ptr %t4
  %t58 = load ptr, ptr %t4
  %t59 = call i64 @strlen(ptr %t58)
  call void @buf_append(ptr %t2, ptr %t57, i64 %t59)
  ret void
L9:
  br label %L8
L8:
  %t61 = load ptr, ptr %t47
  %t63 = ptrtoint ptr %t61 to i64
  %t62 = add i64 %t63, 1
  store i64 %t62, ptr %t47
  %t64 = alloca ptr
  %t65 = sext i32 0 to i64
  store i64 %t65, ptr %t64
  %t66 = alloca i64
  %t67 = sext i32 0 to i64
  store i64 %t67, ptr %t66
  %t68 = alloca i64
  %t69 = sext i32 0 to i64
  store i64 %t69, ptr %t68
  %t70 = alloca i64
  %t71 = call i64 @buf_new()
  store i64 %t71, ptr %t70
  br label %L10
L10:
  %t72 = load ptr, ptr %t47
  %t73 = load i64, ptr %t72
  %t74 = icmp ne i64 %t73, 0
  br i1 %t74, label %L13, label %L14
L13:
  %t75 = load i64, ptr %t68
  %t77 = sext i32 %t75 to i64
  %t78 = sext i32 0 to i64
  %t76 = icmp eq i64 %t77, %t78
  %t79 = zext i1 %t76 to i64
  %t80 = icmp ne i64 %t79, 0
  br i1 %t80, label %L16, label %L17
L16:
  %t81 = load ptr, ptr %t47
  %t82 = load i64, ptr %t81
  %t84 = sext i32 41 to i64
  %t83 = icmp eq i64 %t82, %t84
  %t85 = zext i1 %t83 to i64
  %t86 = icmp ne i64 %t85, 0
  %t87 = zext i1 %t86 to i64
  br label %L18
L17:
  br label %L18
L18:
  %t88 = phi i64 [ %t87, %L16 ], [ 0, %L17 ]
  %t90 = icmp eq i64 %t88, 0
  %t89 = zext i1 %t90 to i64
  %t91 = icmp ne i64 %t89, 0
  %t92 = zext i1 %t91 to i64
  br label %L15
L14:
  br label %L15
L15:
  %t93 = phi i64 [ %t92, %L13 ], [ 0, %L14 ]
  %t94 = icmp ne i64 %t93, 0
  br i1 %t94, label %L11, label %L12
L11:
  %t95 = load ptr, ptr %t47
  %t96 = load i64, ptr %t95
  %t98 = sext i32 44 to i64
  %t97 = icmp eq i64 %t96, %t98
  %t99 = zext i1 %t97 to i64
  %t100 = icmp ne i64 %t99, 0
  br i1 %t100, label %L19, label %L20
L19:
  %t101 = load i64, ptr %t68
  %t103 = sext i32 %t101 to i64
  %t104 = sext i32 0 to i64
  %t102 = icmp eq i64 %t103, %t104
  %t105 = zext i1 %t102 to i64
  %t106 = icmp ne i64 %t105, 0
  %t107 = zext i1 %t106 to i64
  br label %L21
L20:
  br label %L21
L21:
  %t108 = phi i64 [ %t107, %L19 ], [ 0, %L20 ]
  %t109 = icmp ne i64 %t108, 0
  br i1 %t109, label %L22, label %L23
L22:
  %t110 = load i64, ptr %t66
  %t112 = sext i32 %t110 to i64
  %t113 = sext i32 16 to i64
  %t111 = icmp slt i64 %t112, %t113
  %t114 = zext i1 %t111 to i64
  %t115 = icmp ne i64 %t114, 0
  br i1 %t115, label %L25, label %L27
L25:
  %t116 = load i64, ptr %t70
  %t117 = inttoptr i64 %t116 to ptr
  %t118 = sext i32 0 to i64
  %t119 = getelementptr ptr, ptr %t117, i64 %t118
  %t120 = load ptr, ptr %t119
  %t121 = call ptr @strdup(ptr %t120)
  %t122 = load ptr, ptr %t64
  %t123 = load i64, ptr %t66
  %t125 = sext i32 %t123 to i64
  %t124 = add i64 %t125, 1
  store i64 %t124, ptr %t66
  %t127 = sext i32 %t123 to i64
  %t126 = getelementptr ptr, ptr %t122, i64 %t127
  store ptr %t121, ptr %t126
  br label %L27
L27:
  %t129 = sext i32 0 to i64
  %t128 = add i64 %t129, 0
  %t130 = inttoptr i64 %t128 to ptr
  %t131 = load i64, ptr %t70
  %t133 = inttoptr i64 %t131 to ptr
  %t134 = sext i32 1 to i64
  %t132 = getelementptr ptr, ptr %t133, i64 %t134
  store ptr %t130, ptr %t132
  %t135 = load i64, ptr %t70
  %t136 = inttoptr i64 %t135 to ptr
  %t137 = sext i32 0 to i64
  %t138 = getelementptr ptr, ptr %t136, i64 %t137
  %t139 = load ptr, ptr %t138
  %t141 = sext i32 0 to i64
  %t140 = getelementptr ptr, ptr %t139, i64 %t141
  %t142 = sext i32 0 to i64
  store i64 %t142, ptr %t140
  %t143 = load ptr, ptr %t47
  %t145 = ptrtoint ptr %t143 to i64
  %t144 = add i64 %t145, 1
  store i64 %t144, ptr %t47
  br label %L24
L23:
  %t146 = load ptr, ptr %t47
  %t147 = load i64, ptr %t146
  %t149 = sext i32 34 to i64
  %t148 = icmp eq i64 %t147, %t149
  %t150 = zext i1 %t148 to i64
  %t151 = icmp ne i64 %t150, 0
  br i1 %t151, label %L28, label %L29
L28:
  %t152 = load i64, ptr %t70
  %t153 = load ptr, ptr %t47
  %t155 = ptrtoint ptr %t153 to i64
  %t154 = add i64 %t155, 1
  store i64 %t154, ptr %t47
  %t156 = load i64, ptr %t153
  call void @buf_putc(i64 %t152, i64 %t156)
  br label %L31
L31:
  %t158 = load ptr, ptr %t47
  %t159 = load i64, ptr %t158
  %t160 = icmp ne i64 %t159, 0
  br i1 %t160, label %L34, label %L35
L34:
  %t161 = load ptr, ptr %t47
  %t162 = load i64, ptr %t161
  %t164 = sext i32 34 to i64
  %t163 = icmp ne i64 %t162, %t164
  %t165 = zext i1 %t163 to i64
  %t166 = icmp ne i64 %t165, 0
  %t167 = zext i1 %t166 to i64
  br label %L36
L35:
  br label %L36
L36:
  %t168 = phi i64 [ %t167, %L34 ], [ 0, %L35 ]
  %t169 = icmp ne i64 %t168, 0
  br i1 %t169, label %L32, label %L33
L32:
  %t170 = load ptr, ptr %t47
  %t171 = load i64, ptr %t170
  %t173 = sext i32 92 to i64
  %t172 = icmp eq i64 %t171, %t173
  %t174 = zext i1 %t172 to i64
  %t175 = icmp ne i64 %t174, 0
  br i1 %t175, label %L37, label %L38
L37:
  %t176 = load ptr, ptr %t47
  %t178 = ptrtoint ptr %t176 to i64
  %t179 = sext i32 1 to i64
  %t180 = inttoptr i64 %t178 to ptr
  %t177 = getelementptr i8, ptr %t180, i64 %t179
  %t181 = load i64, ptr %t177
  %t182 = icmp ne i64 %t181, 0
  %t183 = zext i1 %t182 to i64
  br label %L39
L38:
  br label %L39
L39:
  %t184 = phi i64 [ %t183, %L37 ], [ 0, %L38 ]
  %t185 = icmp ne i64 %t184, 0
  br i1 %t185, label %L40, label %L42
L40:
  %t186 = load i64, ptr %t70
  %t187 = load ptr, ptr %t47
  %t189 = ptrtoint ptr %t187 to i64
  %t188 = add i64 %t189, 1
  store i64 %t188, ptr %t47
  %t190 = load i64, ptr %t187
  call void @buf_putc(i64 %t186, i64 %t190)
  br label %L42
L42:
  %t192 = load i64, ptr %t70
  %t193 = load ptr, ptr %t47
  %t195 = ptrtoint ptr %t193 to i64
  %t194 = add i64 %t195, 1
  store i64 %t194, ptr %t47
  %t196 = load i64, ptr %t193
  call void @buf_putc(i64 %t192, i64 %t196)
  br label %L31
L33:
  %t198 = load ptr, ptr %t47
  %t199 = load i64, ptr %t198
  %t200 = icmp ne i64 %t199, 0
  br i1 %t200, label %L43, label %L45
L43:
  %t201 = load i64, ptr %t70
  %t202 = load ptr, ptr %t47
  %t204 = ptrtoint ptr %t202 to i64
  %t203 = add i64 %t204, 1
  store i64 %t203, ptr %t47
  %t205 = load i64, ptr %t202
  call void @buf_putc(i64 %t201, i64 %t205)
  br label %L45
L45:
  br label %L30
L29:
  %t207 = load ptr, ptr %t47
  %t208 = load i64, ptr %t207
  %t210 = sext i32 39 to i64
  %t209 = icmp eq i64 %t208, %t210
  %t211 = zext i1 %t209 to i64
  %t212 = icmp ne i64 %t211, 0
  br i1 %t212, label %L46, label %L47
L46:
  %t213 = load i64, ptr %t70
  %t214 = load ptr, ptr %t47
  %t216 = ptrtoint ptr %t214 to i64
  %t215 = add i64 %t216, 1
  store i64 %t215, ptr %t47
  %t217 = load i64, ptr %t214
  call void @buf_putc(i64 %t213, i64 %t217)
  %t219 = load ptr, ptr %t47
  %t220 = load i64, ptr %t219
  %t222 = sext i32 92 to i64
  %t221 = icmp eq i64 %t220, %t222
  %t223 = zext i1 %t221 to i64
  %t224 = icmp ne i64 %t223, 0
  br i1 %t224, label %L49, label %L50
L49:
  %t225 = load ptr, ptr %t47
  %t227 = ptrtoint ptr %t225 to i64
  %t228 = sext i32 1 to i64
  %t229 = inttoptr i64 %t227 to ptr
  %t226 = getelementptr i8, ptr %t229, i64 %t228
  %t230 = load i64, ptr %t226
  %t231 = icmp ne i64 %t230, 0
  %t232 = zext i1 %t231 to i64
  br label %L51
L50:
  br label %L51
L51:
  %t233 = phi i64 [ %t232, %L49 ], [ 0, %L50 ]
  %t234 = icmp ne i64 %t233, 0
  br i1 %t234, label %L52, label %L54
L52:
  %t235 = load i64, ptr %t70
  %t236 = load ptr, ptr %t47
  %t238 = ptrtoint ptr %t236 to i64
  %t237 = add i64 %t238, 1
  store i64 %t237, ptr %t47
  %t239 = load i64, ptr %t236
  call void @buf_putc(i64 %t235, i64 %t239)
  br label %L54
L54:
  %t241 = load ptr, ptr %t47
  %t242 = load i64, ptr %t241
  %t243 = icmp ne i64 %t242, 0
  br i1 %t243, label %L55, label %L57
L55:
  %t244 = load i64, ptr %t70
  %t245 = load ptr, ptr %t47
  %t247 = ptrtoint ptr %t245 to i64
  %t246 = add i64 %t247, 1
  store i64 %t246, ptr %t47
  %t248 = load i64, ptr %t245
  call void @buf_putc(i64 %t244, i64 %t248)
  br label %L57
L57:
  %t250 = load ptr, ptr %t47
  %t251 = load i64, ptr %t250
  %t253 = sext i32 39 to i64
  %t252 = icmp eq i64 %t251, %t253
  %t254 = zext i1 %t252 to i64
  %t255 = icmp ne i64 %t254, 0
  br i1 %t255, label %L58, label %L60
L58:
  %t256 = load i64, ptr %t70
  %t257 = load ptr, ptr %t47
  %t259 = ptrtoint ptr %t257 to i64
  %t258 = add i64 %t259, 1
  store i64 %t258, ptr %t47
  %t260 = load i64, ptr %t257
  call void @buf_putc(i64 %t256, i64 %t260)
  br label %L60
L60:
  br label %L48
L47:
  %t262 = load ptr, ptr %t47
  %t263 = load i64, ptr %t262
  %t265 = sext i32 40 to i64
  %t264 = icmp eq i64 %t263, %t265
  %t266 = zext i1 %t264 to i64
  %t267 = icmp ne i64 %t266, 0
  br i1 %t267, label %L61, label %L63
L61:
  %t268 = load i64, ptr %t68
  %t270 = sext i32 %t268 to i64
  %t269 = add i64 %t270, 1
  store i64 %t269, ptr %t68
  br label %L63
L63:
  %t271 = load ptr, ptr %t47
  %t272 = load i64, ptr %t271
  %t274 = sext i32 41 to i64
  %t273 = icmp eq i64 %t272, %t274
  %t275 = zext i1 %t273 to i64
  %t276 = icmp ne i64 %t275, 0
  br i1 %t276, label %L64, label %L66
L64:
  %t277 = load i64, ptr %t68
  %t279 = sext i32 %t277 to i64
  %t278 = sub i64 %t279, 1
  store i64 %t278, ptr %t68
  br label %L66
L66:
  %t280 = load i64, ptr %t68
  %t282 = sext i32 %t280 to i64
  %t283 = sext i32 0 to i64
  %t281 = icmp sge i64 %t282, %t283
  %t284 = zext i1 %t281 to i64
  %t285 = icmp ne i64 %t284, 0
  br i1 %t285, label %L67, label %L68
L67:
  %t286 = load i64, ptr %t70
  %t287 = load ptr, ptr %t47
  %t289 = ptrtoint ptr %t287 to i64
  %t288 = add i64 %t289, 1
  store i64 %t288, ptr %t47
  %t290 = load i64, ptr %t287
  call void @buf_putc(i64 %t286, i64 %t290)
  br label %L69
L68:
  br label %L12
L70:
  br label %L69
L69:
  br label %L48
L48:
  br label %L30
L30:
  br label %L24
L24:
  br label %L10
L12:
  %t292 = load i64, ptr %t70
  %t293 = inttoptr i64 %t292 to ptr
  %t294 = sext i32 1 to i64
  %t295 = getelementptr ptr, ptr %t293, i64 %t294
  %t296 = load ptr, ptr %t295
  %t297 = ptrtoint ptr %t296 to i64
  %t299 = sext i32 0 to i64
  %t298 = icmp sgt i64 %t297, %t299
  %t300 = zext i1 %t298 to i64
  %t301 = icmp ne i64 %t300, 0
  br i1 %t301, label %L71, label %L72
L71:
  br label %L73
L72:
  %t302 = load i64, ptr %t66
  %t304 = sext i32 %t302 to i64
  %t305 = sext i32 0 to i64
  %t303 = icmp sgt i64 %t304, %t305
  %t306 = zext i1 %t303 to i64
  %t307 = icmp ne i64 %t306, 0
  %t308 = zext i1 %t307 to i64
  br label %L73
L73:
  %t309 = phi i64 [ 1, %L71 ], [ %t308, %L72 ]
  %t310 = icmp ne i64 %t309, 0
  br i1 %t310, label %L74, label %L76
L74:
  %t311 = load i64, ptr %t66
  %t313 = sext i32 %t311 to i64
  %t314 = sext i32 16 to i64
  %t312 = icmp slt i64 %t313, %t314
  %t315 = zext i1 %t312 to i64
  %t316 = icmp ne i64 %t315, 0
  br i1 %t316, label %L77, label %L79
L77:
  %t317 = load i64, ptr %t70
  %t318 = inttoptr i64 %t317 to ptr
  %t319 = sext i32 0 to i64
  %t320 = getelementptr ptr, ptr %t318, i64 %t319
  %t321 = load ptr, ptr %t320
  %t322 = call ptr @strdup(ptr %t321)
  %t323 = load ptr, ptr %t64
  %t324 = load i64, ptr %t66
  %t326 = sext i32 %t324 to i64
  %t325 = add i64 %t326, 1
  store i64 %t325, ptr %t66
  %t328 = sext i32 %t324 to i64
  %t327 = getelementptr ptr, ptr %t323, i64 %t328
  store ptr %t322, ptr %t327
  br label %L79
L79:
  br label %L76
L76:
  %t329 = load i64, ptr %t70
  %t330 = inttoptr i64 %t329 to ptr
  %t331 = sext i32 0 to i64
  %t332 = getelementptr ptr, ptr %t330, i64 %t331
  %t333 = load ptr, ptr %t332
  call void @free(ptr %t333)
  %t335 = load i64, ptr %t70
  call void @free(i64 %t335)
  %t337 = load ptr, ptr %t47
  %t338 = load i64, ptr %t337
  %t340 = sext i32 41 to i64
  %t339 = icmp eq i64 %t338, %t340
  %t341 = zext i1 %t339 to i64
  %t342 = icmp ne i64 %t341, 0
  br i1 %t342, label %L80, label %L82
L80:
  %t343 = load ptr, ptr %t47
  %t345 = ptrtoint ptr %t343 to i64
  %t344 = add i64 %t345, 1
  store i64 %t344, ptr %t47
  br label %L82
L82:
  %t346 = load ptr, ptr %t47
  store ptr %t346, ptr %t1
  %t347 = alloca ptr
  %t348 = load ptr, ptr %t8
  store ptr %t348, ptr %t347
  %t349 = alloca i64
  %t350 = call i64 @buf_new()
  store i64 %t350, ptr %t349
  br label %L83
L83:
  %t351 = load ptr, ptr %t347
  %t352 = load i64, ptr %t351
  %t353 = icmp ne i64 %t352, 0
  br i1 %t353, label %L84, label %L85
L84:
  %t354 = load ptr, ptr %t347
  %t355 = load i64, ptr %t354
  %t357 = sext i32 35 to i64
  %t356 = icmp eq i64 %t355, %t357
  %t358 = zext i1 %t356 to i64
  %t359 = icmp ne i64 %t358, 0
  br i1 %t359, label %L86, label %L87
L86:
  %t360 = load ptr, ptr %t347
  %t362 = ptrtoint ptr %t360 to i64
  %t363 = sext i32 1 to i64
  %t364 = inttoptr i64 %t362 to ptr
  %t361 = getelementptr i8, ptr %t364, i64 %t363
  %t365 = load i64, ptr %t361
  %t367 = sext i32 35 to i64
  %t366 = icmp ne i64 %t365, %t367
  %t368 = zext i1 %t366 to i64
  %t369 = icmp ne i64 %t368, 0
  %t370 = zext i1 %t369 to i64
  br label %L88
L87:
  br label %L88
L88:
  %t371 = phi i64 [ %t370, %L86 ], [ 0, %L87 ]
  %t372 = icmp ne i64 %t371, 0
  br i1 %t372, label %L89, label %L91
L89:
  %t373 = load ptr, ptr %t347
  %t375 = ptrtoint ptr %t373 to i64
  %t374 = add i64 %t375, 1
  store i64 %t374, ptr %t347
  br label %L92
L92:
  %t376 = load ptr, ptr %t347
  %t377 = load i64, ptr %t376
  %t379 = sext i32 32 to i64
  %t378 = icmp eq i64 %t377, %t379
  %t380 = zext i1 %t378 to i64
  %t381 = icmp ne i64 %t380, 0
  br i1 %t381, label %L95, label %L96
L95:
  br label %L97
L96:
  %t382 = load ptr, ptr %t347
  %t383 = load i64, ptr %t382
  %t385 = sext i32 9 to i64
  %t384 = icmp eq i64 %t383, %t385
  %t386 = zext i1 %t384 to i64
  %t387 = icmp ne i64 %t386, 0
  %t388 = zext i1 %t387 to i64
  br label %L97
L97:
  %t389 = phi i64 [ 1, %L95 ], [ %t388, %L96 ]
  %t390 = icmp ne i64 %t389, 0
  br i1 %t390, label %L93, label %L94
L93:
  %t391 = load ptr, ptr %t347
  %t393 = ptrtoint ptr %t391 to i64
  %t392 = add i64 %t393, 1
  store i64 %t392, ptr %t347
  br label %L92
L94:
  %t394 = alloca ptr
  %t395 = alloca ptr
  %t396 = load ptr, ptr %t347
  %t397 = load ptr, ptr %t394
  %t398 = call ptr @read_ident(ptr %t396, ptr %t397, i64 8)
  store ptr %t398, ptr %t395
  %t399 = alloca i64
  %t400 = sext i32 0 to i64
  store i64 %t400, ptr %t399
  %t401 = alloca i64
  %t402 = sext i32 0 to i64
  store i64 %t402, ptr %t401
  br label %L98
L98:
  %t403 = load i64, ptr %t401
  %t404 = load i64, ptr %t12
  %t406 = sext i32 %t403 to i64
  %t407 = sext i32 %t404 to i64
  %t405 = icmp slt i64 %t406, %t407
  %t408 = zext i1 %t405 to i64
  %t409 = icmp ne i64 %t408, 0
  br i1 %t409, label %L102, label %L103
L102:
  %t410 = load i64, ptr %t401
  %t412 = sext i32 %t410 to i64
  %t413 = sext i32 16 to i64
  %t411 = icmp slt i64 %t412, %t413
  %t414 = zext i1 %t411 to i64
  %t415 = icmp ne i64 %t414, 0
  %t416 = zext i1 %t415 to i64
  br label %L104
L103:
  br label %L104
L104:
  %t417 = phi i64 [ %t416, %L102 ], [ 0, %L103 ]
  %t418 = icmp ne i64 %t417, 0
  br i1 %t418, label %L99, label %L101
L99:
  %t419 = alloca ptr
  %t420 = load i64, ptr %t401
  %t421 = call ptr @macro_get_param(i64 %t0, i64 %t420)
  store ptr %t421, ptr %t419
  %t422 = load ptr, ptr %t419
  %t423 = ptrtoint ptr %t422 to i64
  %t424 = icmp ne i64 %t423, 0
  br i1 %t424, label %L105, label %L106
L105:
  %t425 = load ptr, ptr %t419
  %t426 = load ptr, ptr %t394
  %t427 = call i32 @strcmp(ptr %t425, ptr %t426)
  %t428 = sext i32 %t427 to i64
  %t430 = sext i32 0 to i64
  %t429 = icmp eq i64 %t428, %t430
  %t431 = zext i1 %t429 to i64
  %t432 = icmp ne i64 %t431, 0
  %t433 = zext i1 %t432 to i64
  br label %L107
L106:
  br label %L107
L107:
  %t434 = phi i64 [ %t433, %L105 ], [ 0, %L106 ]
  %t435 = icmp ne i64 %t434, 0
  br i1 %t435, label %L108, label %L110
L108:
  %t436 = load i64, ptr %t349
  call void @buf_putc(i64 %t436, i64 34)
  %t438 = load i64, ptr %t401
  %t439 = load i64, ptr %t66
  %t441 = sext i32 %t438 to i64
  %t442 = sext i32 %t439 to i64
  %t440 = icmp slt i64 %t441, %t442
  %t443 = zext i1 %t440 to i64
  %t444 = icmp ne i64 %t443, 0
  br i1 %t444, label %L111, label %L112
L111:
  %t445 = load ptr, ptr %t64
  %t446 = load i64, ptr %t401
  %t447 = sext i32 %t446 to i64
  %t448 = getelementptr ptr, ptr %t445, i64 %t447
  %t449 = load ptr, ptr %t448
  %t450 = ptrtoint ptr %t449 to i64
  %t451 = icmp ne i64 %t450, 0
  %t452 = zext i1 %t451 to i64
  br label %L113
L112:
  br label %L113
L113:
  %t453 = phi i64 [ %t452, %L111 ], [ 0, %L112 ]
  %t454 = icmp ne i64 %t453, 0
  br i1 %t454, label %L114, label %L116
L114:
  %t455 = alloca ptr
  %t456 = load ptr, ptr %t64
  %t457 = load i64, ptr %t401
  %t458 = sext i32 %t457 to i64
  %t459 = getelementptr ptr, ptr %t456, i64 %t458
  %t460 = load ptr, ptr %t459
  store ptr %t460, ptr %t455
  br label %L117
L117:
  %t461 = load ptr, ptr %t455
  %t462 = load i64, ptr %t461
  %t463 = icmp ne i64 %t462, 0
  br i1 %t463, label %L118, label %L119
L118:
  %t464 = load ptr, ptr %t455
  %t465 = load i64, ptr %t464
  %t467 = sext i32 34 to i64
  %t466 = icmp eq i64 %t465, %t467
  %t468 = zext i1 %t466 to i64
  %t469 = icmp ne i64 %t468, 0
  br i1 %t469, label %L120, label %L121
L120:
  br label %L122
L121:
  %t470 = load ptr, ptr %t455
  %t471 = load i64, ptr %t470
  %t473 = sext i32 92 to i64
  %t472 = icmp eq i64 %t471, %t473
  %t474 = zext i1 %t472 to i64
  %t475 = icmp ne i64 %t474, 0
  %t476 = zext i1 %t475 to i64
  br label %L122
L122:
  %t477 = phi i64 [ 1, %L120 ], [ %t476, %L121 ]
  %t478 = icmp ne i64 %t477, 0
  br i1 %t478, label %L123, label %L125
L123:
  %t479 = load i64, ptr %t349
  call void @buf_putc(i64 %t479, i64 92)
  br label %L125
L125:
  %t481 = load i64, ptr %t349
  %t482 = load ptr, ptr %t455
  %t484 = ptrtoint ptr %t482 to i64
  %t483 = add i64 %t484, 1
  store i64 %t483, ptr %t455
  %t485 = load i64, ptr %t482
  call void @buf_putc(i64 %t481, i64 %t485)
  br label %L117
L119:
  br label %L116
L116:
  %t487 = load i64, ptr %t349
  call void @buf_putc(i64 %t487, i64 34)
  %t489 = sext i32 1 to i64
  store i64 %t489, ptr %t399
  br label %L101
L126:
  br label %L110
L110:
  br label %L100
L100:
  %t490 = load i64, ptr %t401
  %t492 = sext i32 %t490 to i64
  %t491 = add i64 %t492, 1
  store i64 %t491, ptr %t401
  br label %L98
L101:
  %t493 = load i64, ptr %t399
  %t495 = sext i32 %t493 to i64
  %t496 = icmp eq i64 %t495, 0
  %t494 = zext i1 %t496 to i64
  %t497 = icmp ne i64 %t494, 0
  br i1 %t497, label %L127, label %L129
L127:
  %t498 = load i64, ptr %t349
  call void @buf_putc(i64 %t498, i64 34)
  %t500 = load i64, ptr %t349
  call void @buf_putc(i64 %t500, i64 34)
  br label %L129
L129:
  %t502 = load ptr, ptr %t395
  store ptr %t502, ptr %t347
  br label %L83
L130:
  br label %L91
L91:
  %t503 = load ptr, ptr %t347
  %t504 = load i64, ptr %t503
  %t506 = sext i32 35 to i64
  %t505 = icmp eq i64 %t504, %t506
  %t507 = zext i1 %t505 to i64
  %t508 = icmp ne i64 %t507, 0
  br i1 %t508, label %L131, label %L132
L131:
  %t509 = load ptr, ptr %t347
  %t511 = ptrtoint ptr %t509 to i64
  %t512 = sext i32 1 to i64
  %t513 = inttoptr i64 %t511 to ptr
  %t510 = getelementptr i8, ptr %t513, i64 %t512
  %t514 = load i64, ptr %t510
  %t516 = sext i32 35 to i64
  %t515 = icmp eq i64 %t514, %t516
  %t517 = zext i1 %t515 to i64
  %t518 = icmp ne i64 %t517, 0
  %t519 = zext i1 %t518 to i64
  br label %L133
L132:
  br label %L133
L133:
  %t520 = phi i64 [ %t519, %L131 ], [ 0, %L132 ]
  %t521 = icmp ne i64 %t520, 0
  br i1 %t521, label %L134, label %L136
L134:
  %t522 = load ptr, ptr %t347
  %t524 = ptrtoint ptr %t522 to i64
  %t525 = sext i32 2 to i64
  %t523 = add i64 %t524, %t525
  store i64 %t523, ptr %t347
  br label %L137
L137:
  %t526 = load ptr, ptr %t347
  %t527 = load i64, ptr %t526
  %t529 = sext i32 32 to i64
  %t528 = icmp eq i64 %t527, %t529
  %t530 = zext i1 %t528 to i64
  %t531 = icmp ne i64 %t530, 0
  br i1 %t531, label %L140, label %L141
L140:
  br label %L142
L141:
  %t532 = load ptr, ptr %t347
  %t533 = load i64, ptr %t532
  %t535 = sext i32 9 to i64
  %t534 = icmp eq i64 %t533, %t535
  %t536 = zext i1 %t534 to i64
  %t537 = icmp ne i64 %t536, 0
  %t538 = zext i1 %t537 to i64
  br label %L142
L142:
  %t539 = phi i64 [ 1, %L140 ], [ %t538, %L141 ]
  %t540 = icmp ne i64 %t539, 0
  br i1 %t540, label %L138, label %L139
L138:
  %t541 = load ptr, ptr %t347
  %t543 = ptrtoint ptr %t541 to i64
  %t542 = add i64 %t543, 1
  store i64 %t542, ptr %t347
  br label %L137
L139:
  br label %L83
L143:
  br label %L136
L136:
  %t544 = load ptr, ptr %t347
  %t545 = load i64, ptr %t544
  %t546 = add i64 %t545, 0
  %t547 = call i32 @isalpha(i64 %t546)
  %t548 = sext i32 %t547 to i64
  %t549 = icmp ne i64 %t548, 0
  br i1 %t549, label %L144, label %L145
L144:
  br label %L146
L145:
  %t550 = load ptr, ptr %t347
  %t551 = load i64, ptr %t550
  %t553 = sext i32 95 to i64
  %t552 = icmp eq i64 %t551, %t553
  %t554 = zext i1 %t552 to i64
  %t555 = icmp ne i64 %t554, 0
  %t556 = zext i1 %t555 to i64
  br label %L146
L146:
  %t557 = phi i64 [ 1, %L144 ], [ %t556, %L145 ]
  %t558 = icmp ne i64 %t557, 0
  br i1 %t558, label %L147, label %L148
L147:
  %t559 = alloca ptr
  %t560 = alloca ptr
  %t561 = load ptr, ptr %t347
  %t562 = load ptr, ptr %t559
  %t563 = call ptr @read_ident(ptr %t561, ptr %t562, i64 8)
  store ptr %t563, ptr %t560
  %t564 = alloca i64
  %t565 = sext i32 0 to i64
  store i64 %t565, ptr %t564
  %t566 = load ptr, ptr %t559
  %t567 = getelementptr [12 x i8], ptr @.str4, i64 0, i64 0
  %t568 = call i32 @strcmp(ptr %t566, ptr %t567)
  %t569 = sext i32 %t568 to i64
  %t571 = sext i32 0 to i64
  %t570 = icmp eq i64 %t569, %t571
  %t572 = zext i1 %t570 to i64
  %t573 = icmp ne i64 %t572, 0
  br i1 %t573, label %L150, label %L152
L150:
  %t574 = alloca i64
  %t575 = load i64, ptr %t12
  %t576 = sext i32 %t575 to i64
  store i64 %t576, ptr %t574
  br label %L153
L153:
  %t577 = load i64, ptr %t574
  %t578 = load i64, ptr %t66
  %t580 = sext i32 %t577 to i64
  %t581 = sext i32 %t578 to i64
  %t579 = icmp slt i64 %t580, %t581
  %t582 = zext i1 %t579 to i64
  %t583 = icmp ne i64 %t582, 0
  br i1 %t583, label %L154, label %L156
L154:
  %t584 = load i64, ptr %t574
  %t585 = load i64, ptr %t12
  %t587 = sext i32 %t584 to i64
  %t588 = sext i32 %t585 to i64
  %t586 = icmp sgt i64 %t587, %t588
  %t589 = zext i1 %t586 to i64
  %t590 = icmp ne i64 %t589, 0
  br i1 %t590, label %L157, label %L159
L157:
  %t591 = load i64, ptr %t349
  call void @buf_putc(i64 %t591, i64 44)
  br label %L159
L159:
  %t593 = load ptr, ptr %t64
  %t594 = load i64, ptr %t574
  %t595 = sext i32 %t594 to i64
  %t596 = getelementptr ptr, ptr %t593, i64 %t595
  %t597 = load ptr, ptr %t596
  %t598 = icmp ne ptr %t597, null
  br i1 %t598, label %L160, label %L162
L160:
  %t599 = load i64, ptr %t349
  %t600 = load ptr, ptr %t64
  %t601 = load i64, ptr %t574
  %t602 = sext i32 %t601 to i64
  %t603 = getelementptr ptr, ptr %t600, i64 %t602
  %t604 = load ptr, ptr %t603
  %t605 = load ptr, ptr %t64
  %t606 = load i64, ptr %t574
  %t607 = sext i32 %t606 to i64
  %t608 = getelementptr ptr, ptr %t605, i64 %t607
  %t609 = load ptr, ptr %t608
  %t610 = call i64 @strlen(ptr %t609)
  call void @buf_append(i64 %t599, ptr %t604, i64 %t610)
  br label %L162
L162:
  br label %L155
L155:
  %t612 = load i64, ptr %t574
  %t614 = sext i32 %t612 to i64
  %t613 = add i64 %t614, 1
  store i64 %t613, ptr %t574
  br label %L153
L156:
  %t615 = sext i32 1 to i64
  store i64 %t615, ptr %t564
  br label %L152
L152:
  %t616 = load i64, ptr %t564
  %t618 = sext i32 %t616 to i64
  %t619 = icmp eq i64 %t618, 0
  %t617 = zext i1 %t619 to i64
  %t620 = icmp ne i64 %t617, 0
  br i1 %t620, label %L163, label %L165
L163:
  %t621 = alloca i64
  %t622 = sext i32 0 to i64
  store i64 %t622, ptr %t621
  br label %L166
L166:
  %t623 = load i64, ptr %t621
  %t624 = load i64, ptr %t12
  %t626 = sext i32 %t623 to i64
  %t627 = sext i32 %t624 to i64
  %t625 = icmp slt i64 %t626, %t627
  %t628 = zext i1 %t625 to i64
  %t629 = icmp ne i64 %t628, 0
  br i1 %t629, label %L170, label %L171
L170:
  %t630 = load i64, ptr %t621
  %t632 = sext i32 %t630 to i64
  %t633 = sext i32 16 to i64
  %t631 = icmp slt i64 %t632, %t633
  %t634 = zext i1 %t631 to i64
  %t635 = icmp ne i64 %t634, 0
  %t636 = zext i1 %t635 to i64
  br label %L172
L171:
  br label %L172
L172:
  %t637 = phi i64 [ %t636, %L170 ], [ 0, %L171 ]
  %t638 = icmp ne i64 %t637, 0
  br i1 %t638, label %L167, label %L169
L167:
  %t639 = alloca ptr
  %t640 = load i64, ptr %t621
  %t641 = call ptr @macro_get_param(i64 %t0, i64 %t640)
  store ptr %t641, ptr %t639
  %t642 = load ptr, ptr %t639
  %t643 = ptrtoint ptr %t642 to i64
  %t644 = icmp ne i64 %t643, 0
  br i1 %t644, label %L173, label %L174
L173:
  %t645 = load ptr, ptr %t639
  %t646 = load ptr, ptr %t559
  %t647 = call i32 @strcmp(ptr %t645, ptr %t646)
  %t648 = sext i32 %t647 to i64
  %t650 = sext i32 0 to i64
  %t649 = icmp eq i64 %t648, %t650
  %t651 = zext i1 %t649 to i64
  %t652 = icmp ne i64 %t651, 0
  %t653 = zext i1 %t652 to i64
  br label %L175
L174:
  br label %L175
L175:
  %t654 = phi i64 [ %t653, %L173 ], [ 0, %L174 ]
  %t655 = icmp ne i64 %t654, 0
  br i1 %t655, label %L176, label %L178
L176:
  %t656 = load i64, ptr %t621
  %t657 = load i64, ptr %t66
  %t659 = sext i32 %t656 to i64
  %t660 = sext i32 %t657 to i64
  %t658 = icmp slt i64 %t659, %t660
  %t661 = zext i1 %t658 to i64
  %t662 = icmp ne i64 %t661, 0
  br i1 %t662, label %L179, label %L180
L179:
  %t663 = load ptr, ptr %t64
  %t664 = load i64, ptr %t621
  %t665 = sext i32 %t664 to i64
  %t666 = getelementptr ptr, ptr %t663, i64 %t665
  %t667 = load ptr, ptr %t666
  %t668 = ptrtoint ptr %t667 to i64
  %t669 = icmp ne i64 %t668, 0
  %t670 = zext i1 %t669 to i64
  br label %L181
L180:
  br label %L181
L181:
  %t671 = phi i64 [ %t670, %L179 ], [ 0, %L180 ]
  %t672 = icmp ne i64 %t671, 0
  br i1 %t672, label %L182, label %L184
L182:
  %t673 = load i64, ptr %t349
  %t674 = load ptr, ptr %t64
  %t675 = load i64, ptr %t621
  %t676 = sext i32 %t675 to i64
  %t677 = getelementptr ptr, ptr %t674, i64 %t676
  %t678 = load ptr, ptr %t677
  %t679 = load ptr, ptr %t64
  %t680 = load i64, ptr %t621
  %t681 = sext i32 %t680 to i64
  %t682 = getelementptr ptr, ptr %t679, i64 %t681
  %t683 = load ptr, ptr %t682
  %t684 = call i64 @strlen(ptr %t683)
  call void @buf_append(i64 %t673, ptr %t678, i64 %t684)
  br label %L184
L184:
  %t686 = sext i32 1 to i64
  store i64 %t686, ptr %t564
  br label %L169
L185:
  br label %L178
L178:
  br label %L168
L168:
  %t687 = load i64, ptr %t621
  %t689 = sext i32 %t687 to i64
  %t688 = add i64 %t689, 1
  store i64 %t688, ptr %t621
  br label %L166
L169:
  br label %L165
L165:
  %t690 = load i64, ptr %t564
  %t692 = sext i32 %t690 to i64
  %t693 = icmp eq i64 %t692, 0
  %t691 = zext i1 %t693 to i64
  %t694 = icmp ne i64 %t691, 0
  br i1 %t694, label %L186, label %L188
L186:
  %t695 = load i64, ptr %t349
  %t696 = load ptr, ptr %t559
  %t697 = load ptr, ptr %t559
  %t698 = call i64 @strlen(ptr %t697)
  call void @buf_append(i64 %t695, ptr %t696, i64 %t698)
  br label %L188
L188:
  %t700 = load ptr, ptr %t560
  store ptr %t700, ptr %t347
  br label %L149
L148:
  %t701 = load i64, ptr %t349
  %t702 = load ptr, ptr %t347
  %t704 = ptrtoint ptr %t702 to i64
  %t703 = add i64 %t704, 1
  store i64 %t703, ptr %t347
  %t705 = load i64, ptr %t702
  call void @buf_putc(i64 %t701, i64 %t705)
  br label %L149
L149:
  br label %L83
L85:
  %t707 = load i64, ptr %t349
  %t708 = inttoptr i64 %t707 to ptr
  %t709 = sext i32 0 to i64
  %t710 = getelementptr ptr, ptr %t708, i64 %t709
  %t711 = load ptr, ptr %t710
  %t713 = sext i32 1 to i64
  %t712 = add i64 %t3, %t713
  call void @expand_text(ptr %t711, ptr %t2, i64 %t712)
  %t715 = load i64, ptr %t349
  %t716 = inttoptr i64 %t715 to ptr
  %t717 = sext i32 0 to i64
  %t718 = getelementptr ptr, ptr %t716, i64 %t717
  %t719 = load ptr, ptr %t718
  call void @free(ptr %t719)
  %t721 = load i64, ptr %t349
  call void @free(i64 %t721)
  %t723 = alloca i64
  %t724 = sext i32 0 to i64
  store i64 %t724, ptr %t723
  br label %L189
L189:
  %t725 = load i64, ptr %t723
  %t726 = load i64, ptr %t66
  %t728 = sext i32 %t725 to i64
  %t729 = sext i32 %t726 to i64
  %t727 = icmp slt i64 %t728, %t729
  %t730 = zext i1 %t727 to i64
  %t731 = icmp ne i64 %t730, 0
  br i1 %t731, label %L190, label %L192
L190:
  %t732 = load ptr, ptr %t64
  %t733 = load i64, ptr %t723
  %t734 = sext i32 %t733 to i64
  %t735 = getelementptr ptr, ptr %t732, i64 %t734
  %t736 = load ptr, ptr %t735
  call void @free(ptr %t736)
  br label %L191
L191:
  %t738 = load i64, ptr %t723
  %t740 = sext i32 %t738 to i64
  %t739 = add i64 %t740, 1
  store i64 %t739, ptr %t723
  br label %L189
L192:
  ret void
}

define internal void @expand_text(ptr %t0, ptr %t1, i64 %t2) {
entry:
  %t4 = sext i32 64 to i64
  %t3 = icmp sgt i64 %t2, %t4
  %t5 = zext i1 %t3 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L2
L0:
  %t7 = call i64 @strlen(ptr %t0)
  call void @buf_append(ptr %t1, ptr %t0, i64 %t7)
  ret void
L3:
  br label %L2
L2:
  %t9 = alloca ptr
  store ptr %t0, ptr %t9
  br label %L4
L4:
  %t10 = load ptr, ptr %t9
  %t11 = load i64, ptr %t10
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L5, label %L6
L5:
  %t13 = load ptr, ptr %t9
  %t14 = load i64, ptr %t13
  %t16 = sext i32 34 to i64
  %t15 = icmp eq i64 %t14, %t16
  %t17 = zext i1 %t15 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L7, label %L9
L7:
  %t19 = load ptr, ptr %t9
  %t21 = ptrtoint ptr %t19 to i64
  %t20 = add i64 %t21, 1
  store i64 %t20, ptr %t9
  %t22 = load i64, ptr %t19
  call void @buf_putc(ptr %t1, i64 %t22)
  br label %L10
L10:
  %t24 = load ptr, ptr %t9
  %t25 = load i64, ptr %t24
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L13, label %L14
L13:
  %t27 = load ptr, ptr %t9
  %t28 = load i64, ptr %t27
  %t30 = sext i32 34 to i64
  %t29 = icmp ne i64 %t28, %t30
  %t31 = zext i1 %t29 to i64
  %t32 = icmp ne i64 %t31, 0
  %t33 = zext i1 %t32 to i64
  br label %L15
L14:
  br label %L15
L15:
  %t34 = phi i64 [ %t33, %L13 ], [ 0, %L14 ]
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L11, label %L12
L11:
  %t36 = load ptr, ptr %t9
  %t37 = load i64, ptr %t36
  %t39 = sext i32 92 to i64
  %t38 = icmp eq i64 %t37, %t39
  %t40 = zext i1 %t38 to i64
  %t41 = icmp ne i64 %t40, 0
  br i1 %t41, label %L16, label %L18
L16:
  %t42 = load ptr, ptr %t9
  %t44 = ptrtoint ptr %t42 to i64
  %t43 = add i64 %t44, 1
  store i64 %t43, ptr %t9
  %t45 = load i64, ptr %t42
  call void @buf_putc(ptr %t1, i64 %t45)
  br label %L18
L18:
  %t47 = load ptr, ptr %t9
  %t48 = load i64, ptr %t47
  %t49 = icmp ne i64 %t48, 0
  br i1 %t49, label %L19, label %L21
L19:
  %t50 = load ptr, ptr %t9
  %t52 = ptrtoint ptr %t50 to i64
  %t51 = add i64 %t52, 1
  store i64 %t51, ptr %t9
  %t53 = load i64, ptr %t50
  call void @buf_putc(ptr %t1, i64 %t53)
  br label %L21
L21:
  br label %L10
L12:
  %t55 = load ptr, ptr %t9
  %t56 = load i64, ptr %t55
  %t57 = icmp ne i64 %t56, 0
  br i1 %t57, label %L22, label %L24
L22:
  %t58 = load ptr, ptr %t9
  %t60 = ptrtoint ptr %t58 to i64
  %t59 = add i64 %t60, 1
  store i64 %t59, ptr %t9
  %t61 = load i64, ptr %t58
  call void @buf_putc(ptr %t1, i64 %t61)
  br label %L24
L24:
  br label %L4
L25:
  br label %L9
L9:
  %t63 = load ptr, ptr %t9
  %t64 = load i64, ptr %t63
  %t66 = sext i32 39 to i64
  %t65 = icmp eq i64 %t64, %t66
  %t67 = zext i1 %t65 to i64
  %t68 = icmp ne i64 %t67, 0
  br i1 %t68, label %L26, label %L28
L26:
  %t69 = load ptr, ptr %t9
  %t71 = ptrtoint ptr %t69 to i64
  %t70 = add i64 %t71, 1
  store i64 %t70, ptr %t9
  %t72 = load i64, ptr %t69
  call void @buf_putc(ptr %t1, i64 %t72)
  br label %L29
L29:
  %t74 = load ptr, ptr %t9
  %t75 = load i64, ptr %t74
  %t76 = icmp ne i64 %t75, 0
  br i1 %t76, label %L32, label %L33
L32:
  %t77 = load ptr, ptr %t9
  %t78 = load i64, ptr %t77
  %t80 = sext i32 39 to i64
  %t79 = icmp ne i64 %t78, %t80
  %t81 = zext i1 %t79 to i64
  %t82 = icmp ne i64 %t81, 0
  %t83 = zext i1 %t82 to i64
  br label %L34
L33:
  br label %L34
L34:
  %t84 = phi i64 [ %t83, %L32 ], [ 0, %L33 ]
  %t85 = icmp ne i64 %t84, 0
  br i1 %t85, label %L30, label %L31
L30:
  %t86 = load ptr, ptr %t9
  %t87 = load i64, ptr %t86
  %t89 = sext i32 92 to i64
  %t88 = icmp eq i64 %t87, %t89
  %t90 = zext i1 %t88 to i64
  %t91 = icmp ne i64 %t90, 0
  br i1 %t91, label %L35, label %L37
L35:
  %t92 = load ptr, ptr %t9
  %t94 = ptrtoint ptr %t92 to i64
  %t93 = add i64 %t94, 1
  store i64 %t93, ptr %t9
  %t95 = load i64, ptr %t92
  call void @buf_putc(ptr %t1, i64 %t95)
  br label %L37
L37:
  %t97 = load ptr, ptr %t9
  %t98 = load i64, ptr %t97
  %t99 = icmp ne i64 %t98, 0
  br i1 %t99, label %L38, label %L40
L38:
  %t100 = load ptr, ptr %t9
  %t102 = ptrtoint ptr %t100 to i64
  %t101 = add i64 %t102, 1
  store i64 %t101, ptr %t9
  %t103 = load i64, ptr %t100
  call void @buf_putc(ptr %t1, i64 %t103)
  br label %L40
L40:
  br label %L29
L31:
  %t105 = load ptr, ptr %t9
  %t106 = load i64, ptr %t105
  %t107 = icmp ne i64 %t106, 0
  br i1 %t107, label %L41, label %L43
L41:
  %t108 = load ptr, ptr %t9
  %t110 = ptrtoint ptr %t108 to i64
  %t109 = add i64 %t110, 1
  store i64 %t109, ptr %t9
  %t111 = load i64, ptr %t108
  call void @buf_putc(ptr %t1, i64 %t111)
  br label %L43
L43:
  br label %L4
L44:
  br label %L28
L28:
  %t113 = load ptr, ptr %t9
  %t114 = load i64, ptr %t113
  %t115 = add i64 %t114, 0
  %t116 = call i32 @isalpha(i64 %t115)
  %t117 = sext i32 %t116 to i64
  %t118 = icmp ne i64 %t117, 0
  br i1 %t118, label %L45, label %L46
L45:
  br label %L47
L46:
  %t119 = load ptr, ptr %t9
  %t120 = load i64, ptr %t119
  %t122 = sext i32 95 to i64
  %t121 = icmp eq i64 %t120, %t122
  %t123 = zext i1 %t121 to i64
  %t124 = icmp ne i64 %t123, 0
  %t125 = zext i1 %t124 to i64
  br label %L47
L47:
  %t126 = phi i64 [ 1, %L45 ], [ %t125, %L46 ]
  %t127 = icmp ne i64 %t126, 0
  br i1 %t127, label %L48, label %L50
L48:
  %t128 = alloca ptr
  %t129 = alloca ptr
  %t130 = load ptr, ptr %t9
  %t131 = load ptr, ptr %t128
  %t132 = call ptr @read_ident(ptr %t130, ptr %t131, i64 8)
  store ptr %t132, ptr %t129
  %t133 = alloca i64
  %t134 = load ptr, ptr %t128
  %t135 = call i32 @macro_find_idx(ptr %t134)
  %t136 = sext i32 %t135 to i64
  store i64 %t136, ptr %t133
  %t137 = load i64, ptr %t133
  %t139 = sext i32 %t137 to i64
  %t140 = sext i32 0 to i64
  %t138 = icmp sge i64 %t139, %t140
  %t141 = zext i1 %t138 to i64
  %t142 = icmp ne i64 %t141, 0
  br i1 %t142, label %L51, label %L52
L51:
  %t143 = load ptr, ptr @macro_funclike
  %t144 = load i64, ptr %t133
  %t145 = sext i32 %t144 to i64
  %t146 = getelementptr ptr, ptr %t143, i64 %t145
  %t147 = load ptr, ptr %t146
  %t148 = ptrtoint ptr %t147 to i64
  %t149 = icmp ne i64 %t148, 0
  %t150 = zext i1 %t149 to i64
  br label %L53
L52:
  br label %L53
L53:
  %t151 = phi i64 [ %t150, %L51 ], [ 0, %L52 ]
  %t152 = icmp ne i64 %t151, 0
  br i1 %t152, label %L54, label %L56
L54:
  %t153 = alloca ptr
  %t154 = load ptr, ptr %t129
  store ptr %t154, ptr %t153
  %t155 = load ptr, ptr %t153
  %t156 = call ptr @skip_ws(ptr %t155)
  store ptr %t156, ptr %t153
  %t157 = load ptr, ptr %t153
  %t158 = load i64, ptr %t157
  %t160 = sext i32 40 to i64
  %t159 = icmp eq i64 %t158, %t160
  %t161 = zext i1 %t159 to i64
  %t162 = icmp ne i64 %t161, 0
  br i1 %t162, label %L57, label %L59
L57:
  %t163 = load ptr, ptr %t129
  store ptr %t163, ptr %t9
  %t164 = load i64, ptr %t133
  %t166 = sext i32 1 to i64
  %t165 = add i64 %t2, %t166
  call void @expand_func_macro(i64 %t164, ptr %t9, ptr %t1, i64 %t165)
  br label %L4
L60:
  br label %L59
L59:
  br label %L56
L56:
  %t168 = load i64, ptr %t133
  %t170 = sext i32 %t168 to i64
  %t171 = sext i32 0 to i64
  %t169 = icmp sge i64 %t170, %t171
  %t172 = zext i1 %t169 to i64
  %t173 = icmp ne i64 %t172, 0
  br i1 %t173, label %L61, label %L62
L61:
  %t174 = load ptr, ptr @macro_funclike
  %t175 = load i64, ptr %t133
  %t176 = sext i32 %t175 to i64
  %t177 = getelementptr ptr, ptr %t174, i64 %t176
  %t178 = load ptr, ptr %t177
  %t180 = ptrtoint ptr %t178 to i64
  %t181 = icmp eq i64 %t180, 0
  %t179 = zext i1 %t181 to i64
  %t182 = icmp ne i64 %t179, 0
  %t183 = zext i1 %t182 to i64
  br label %L63
L62:
  br label %L63
L63:
  %t184 = phi i64 [ %t183, %L61 ], [ 0, %L62 ]
  %t185 = icmp ne i64 %t184, 0
  br i1 %t185, label %L64, label %L66
L64:
  %t186 = load ptr, ptr @macro_bodies
  %t187 = load i64, ptr %t133
  %t188 = sext i32 %t187 to i64
  %t189 = getelementptr ptr, ptr %t186, i64 %t188
  %t190 = load ptr, ptr %t189
  %t192 = sext i32 1 to i64
  %t191 = add i64 %t2, %t192
  call void @expand_text(ptr %t190, ptr %t1, i64 %t191)
  %t194 = load ptr, ptr %t129
  store ptr %t194, ptr %t9
  br label %L4
L67:
  br label %L66
L66:
  %t195 = load ptr, ptr %t128
  %t196 = load ptr, ptr %t128
  %t197 = call i64 @strlen(ptr %t196)
  call void @buf_append(ptr %t1, ptr %t195, i64 %t197)
  %t199 = load ptr, ptr %t129
  store ptr %t199, ptr %t9
  br label %L4
L68:
  br label %L50
L50:
  %t200 = load ptr, ptr %t9
  %t202 = ptrtoint ptr %t200 to i64
  %t201 = add i64 %t202, 1
  store i64 %t201, ptr %t9
  %t203 = load i64, ptr %t200
  call void @buf_putc(ptr %t1, i64 %t203)
  br label %L4
L6:
  ret void
}

define internal ptr @read_file(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = getelementptr [2 x i8], ptr @.str5, i64 0, i64 0
  %t3 = call ptr @fopen(ptr %t0, ptr %t2)
  store ptr %t3, ptr %t1
  %t4 = load ptr, ptr %t1
  %t6 = ptrtoint ptr %t4 to i64
  %t7 = icmp eq i64 %t6, 0
  %t5 = zext i1 %t7 to i64
  %t8 = icmp ne i64 %t5, 0
  br i1 %t8, label %L0, label %L2
L0:
  %t10 = sext i32 0 to i64
  %t9 = inttoptr i64 %t10 to ptr
  ret ptr %t9
L3:
  br label %L2
L2:
  %t11 = load ptr, ptr %t1
  %t12 = call i64 @fseek(ptr %t11, i64 0, i64 2)
  %t13 = alloca i64
  %t14 = load ptr, ptr %t1
  %t15 = call i64 @ftell(ptr %t14)
  store i64 %t15, ptr %t13
  %t16 = load ptr, ptr %t1
  %t17 = call i64 @fseek(ptr %t16, i64 0, i64 0)
  %t18 = alloca ptr
  %t19 = load i64, ptr %t13
  %t21 = sext i32 1 to i64
  %t20 = add i64 %t19, %t21
  %t22 = call ptr @malloc(i64 %t20)
  store ptr %t22, ptr %t18
  %t23 = load ptr, ptr %t18
  %t25 = ptrtoint ptr %t23 to i64
  %t26 = icmp eq i64 %t25, 0
  %t24 = zext i1 %t26 to i64
  %t27 = icmp ne i64 %t24, 0
  br i1 %t27, label %L4, label %L6
L4:
  %t28 = load ptr, ptr %t1
  %t29 = call i32 @fclose(ptr %t28)
  %t30 = sext i32 %t29 to i64
  %t32 = sext i32 0 to i64
  %t31 = inttoptr i64 %t32 to ptr
  ret ptr %t31
L7:
  br label %L6
L6:
  %t33 = alloca i64
  %t34 = load ptr, ptr %t18
  %t35 = load i64, ptr %t13
  %t36 = load ptr, ptr %t1
  %t37 = call i64 @fread(ptr %t34, i64 1, i64 %t35, ptr %t36)
  store i64 %t37, ptr %t33
  %t38 = load ptr, ptr %t18
  %t39 = load i64, ptr %t33
  %t41 = sext i32 %t39 to i64
  %t40 = getelementptr ptr, ptr %t38, i64 %t41
  %t42 = sext i32 0 to i64
  store i64 %t42, ptr %t40
  %t43 = load ptr, ptr %t1
  %t44 = call i32 @fclose(ptr %t43)
  %t45 = sext i32 %t44 to i64
  %t46 = load ptr, ptr %t18
  ret ptr %t46
L8:
  ret ptr null
}

define internal ptr @get_include_paths() {
entry:
  %t0 = alloca ptr
  %t1 = alloca i64
  %t2 = sext i32 0 to i64
  store i64 %t2, ptr %t1
  %t3 = load i64, ptr %t1
  %t5 = sext i32 %t3 to i64
  %t6 = icmp eq i64 %t5, 0
  %t4 = zext i1 %t6 to i64
  %t7 = icmp ne i64 %t4, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = getelementptr [13 x i8], ptr @.str6, i64 0, i64 0
  %t9 = load ptr, ptr %t0
  %t11 = sext i32 0 to i64
  %t10 = getelementptr ptr, ptr %t9, i64 %t11
  store ptr %t8, ptr %t10
  %t12 = getelementptr [19 x i8], ptr @.str7, i64 0, i64 0
  %t13 = load ptr, ptr %t0
  %t15 = sext i32 1 to i64
  %t14 = getelementptr ptr, ptr %t13, i64 %t15
  store ptr %t12, ptr %t14
  %t16 = getelementptr [2 x i8], ptr @.str8, i64 0, i64 0
  %t17 = load ptr, ptr %t0
  %t19 = sext i32 2 to i64
  %t18 = getelementptr ptr, ptr %t17, i64 %t19
  store ptr %t16, ptr %t18
  %t21 = sext i32 0 to i64
  %t20 = inttoptr i64 %t21 to ptr
  %t22 = load ptr, ptr %t0
  %t24 = sext i32 3 to i64
  %t23 = getelementptr ptr, ptr %t22, i64 %t24
  store ptr %t20, ptr %t23
  %t25 = sext i32 1 to i64
  store i64 %t25, ptr %t1
  br label %L2
L2:
  %t26 = load ptr, ptr %t0
  ret ptr %t26
L3:
  ret ptr null
}

define internal ptr @find_include(ptr %t0, i64 %t1) {
entry:
  %t3 = icmp eq i64 %t1, 0
  %t2 = zext i1 %t3 to i64
  %t4 = icmp ne i64 %t2, 0
  br i1 %t4, label %L0, label %L2
L0:
  %t5 = alloca ptr
  %t6 = call ptr @read_file(ptr %t0)
  store ptr %t6, ptr %t5
  %t7 = load ptr, ptr %t5
  %t8 = icmp ne ptr %t7, null
  br i1 %t8, label %L3, label %L5
L3:
  %t9 = load ptr, ptr %t5
  ret ptr %t9
L6:
  br label %L5
L5:
  br label %L2
L2:
  %t10 = alloca ptr
  %t11 = call ptr @get_include_paths()
  store ptr %t11, ptr %t10
  %t12 = alloca i64
  %t13 = sext i32 0 to i64
  store i64 %t13, ptr %t12
  br label %L7
L7:
  %t14 = load ptr, ptr %t10
  %t15 = load i64, ptr %t12
  %t16 = sext i32 %t15 to i64
  %t17 = getelementptr ptr, ptr %t14, i64 %t16
  %t18 = load ptr, ptr %t17
  %t19 = icmp ne ptr %t18, null
  br i1 %t19, label %L8, label %L10
L8:
  %t20 = alloca ptr
  %t21 = load ptr, ptr %t20
  %t22 = getelementptr [6 x i8], ptr @.str9, i64 0, i64 0
  %t23 = load ptr, ptr %t10
  %t24 = load i64, ptr %t12
  %t25 = sext i32 %t24 to i64
  %t26 = getelementptr ptr, ptr %t23, i64 %t25
  %t27 = load ptr, ptr %t26
  %t28 = call i32 (ptr, ...) @snprintf(ptr %t21, i64 8, ptr %t22, ptr %t27, ptr %t0)
  %t29 = sext i32 %t28 to i64
  %t30 = alloca ptr
  %t31 = load ptr, ptr %t20
  %t32 = call ptr @read_file(ptr %t31)
  store ptr %t32, ptr %t30
  %t33 = load ptr, ptr %t30
  %t34 = icmp ne ptr %t33, null
  br i1 %t34, label %L11, label %L13
L11:
  %t35 = load ptr, ptr %t30
  ret ptr %t35
L14:
  br label %L13
L13:
  br label %L9
L9:
  %t36 = load i64, ptr %t12
  %t38 = sext i32 %t36 to i64
  %t37 = add i64 %t38, 1
  store i64 %t37, ptr %t12
  br label %L7
L10:
  %t40 = sext i32 0 to i64
  %t39 = inttoptr i64 %t40 to ptr
  ret ptr %t39
L15:
  ret ptr null
}

define internal void @process_directive(ptr %t0, ptr %t1, ptr %t2, i64 %t3, ptr %t4, ptr %t5) {
entry:
  %t6 = alloca ptr
  %t8 = ptrtoint ptr %t0 to i64
  %t9 = sext i32 1 to i64
  %t10 = inttoptr i64 %t8 to ptr
  %t7 = getelementptr i8, ptr %t10, i64 %t9
  %t11 = call ptr @skip_ws(ptr %t7)
  store ptr %t11, ptr %t6
  %t12 = alloca ptr
  %t13 = load ptr, ptr %t6
  %t14 = load ptr, ptr %t12
  %t15 = call ptr @read_ident(ptr %t13, ptr %t14, i64 8)
  store ptr %t15, ptr %t6
  %t16 = load ptr, ptr %t6
  %t17 = call ptr @skip_ws(ptr %t16)
  store ptr %t17, ptr %t6
  %t18 = load ptr, ptr %t12
  %t19 = getelementptr [6 x i8], ptr @.str10, i64 0, i64 0
  %t20 = call i32 @strcmp(ptr %t18, ptr %t19)
  %t21 = sext i32 %t20 to i64
  %t23 = sext i32 0 to i64
  %t22 = icmp eq i64 %t21, %t23
  %t24 = zext i1 %t22 to i64
  %t25 = icmp ne i64 %t24, 0
  br i1 %t25, label %L0, label %L2
L0:
  %t26 = alloca ptr
  %t27 = load ptr, ptr %t6
  %t28 = load ptr, ptr %t26
  %t29 = call ptr @read_ident(ptr %t27, ptr %t28, i64 8)
  %t30 = alloca i64
  %t31 = load ptr, ptr %t26
  %t32 = call i32 @macro_find_idx(ptr %t31)
  %t33 = sext i32 %t32 to i64
  %t35 = sext i32 0 to i64
  %t34 = icmp sge i64 %t33, %t35
  %t36 = zext i1 %t34 to i64
  store i64 %t36, ptr %t30
  %t37 = load i64, ptr %t5
  %t39 = sext i32 32 to i64
  %t38 = icmp slt i64 %t37, %t39
  %t40 = zext i1 %t38 to i64
  %t41 = icmp ne i64 %t40, 0
  br i1 %t41, label %L3, label %L5
L3:
  %t42 = load i64, ptr %t30
  %t43 = load i64, ptr %t5
  %t44 = add i64 %t43, 1
  store i64 %t44, ptr %t5
  %t45 = getelementptr ptr, ptr %t4, i64 %t43
  %t46 = sext i32 %t42 to i64
  store i64 %t46, ptr %t45
  br label %L5
L5:
  ret void
L6:
  br label %L2
L2:
  %t47 = load ptr, ptr %t12
  %t48 = getelementptr [7 x i8], ptr @.str11, i64 0, i64 0
  %t49 = call i32 @strcmp(ptr %t47, ptr %t48)
  %t50 = sext i32 %t49 to i64
  %t52 = sext i32 0 to i64
  %t51 = icmp eq i64 %t50, %t52
  %t53 = zext i1 %t51 to i64
  %t54 = icmp ne i64 %t53, 0
  br i1 %t54, label %L7, label %L9
L7:
  %t55 = alloca ptr
  %t56 = load ptr, ptr %t6
  %t57 = load ptr, ptr %t55
  %t58 = call ptr @read_ident(ptr %t56, ptr %t57, i64 8)
  %t59 = alloca i64
  %t60 = load ptr, ptr %t55
  %t61 = call i32 @macro_find_idx(ptr %t60)
  %t62 = sext i32 %t61 to i64
  %t64 = sext i32 0 to i64
  %t63 = icmp slt i64 %t62, %t64
  %t65 = zext i1 %t63 to i64
  store i64 %t65, ptr %t59
  %t66 = load i64, ptr %t5
  %t68 = sext i32 32 to i64
  %t67 = icmp slt i64 %t66, %t68
  %t69 = zext i1 %t67 to i64
  %t70 = icmp ne i64 %t69, 0
  br i1 %t70, label %L10, label %L12
L10:
  %t71 = load i64, ptr %t59
  %t72 = load i64, ptr %t5
  %t73 = add i64 %t72, 1
  store i64 %t73, ptr %t5
  %t74 = getelementptr ptr, ptr %t4, i64 %t72
  %t75 = sext i32 %t71 to i64
  store i64 %t75, ptr %t74
  br label %L12
L12:
  ret void
L13:
  br label %L9
L9:
  %t76 = load ptr, ptr %t12
  %t77 = getelementptr [3 x i8], ptr @.str12, i64 0, i64 0
  %t78 = call i32 @strcmp(ptr %t76, ptr %t77)
  %t79 = sext i32 %t78 to i64
  %t81 = sext i32 0 to i64
  %t80 = icmp eq i64 %t79, %t81
  %t82 = zext i1 %t80 to i64
  %t83 = icmp ne i64 %t82, 0
  br i1 %t83, label %L14, label %L16
L14:
  %t84 = alloca i64
  %t85 = sext i32 0 to i64
  store i64 %t85, ptr %t84
  %t86 = load ptr, ptr %t6
  %t87 = getelementptr [8 x i8], ptr @.str13, i64 0, i64 0
  %t88 = call i32 @strncmp(ptr %t86, ptr %t87, i64 7)
  %t89 = sext i32 %t88 to i64
  %t91 = sext i32 0 to i64
  %t90 = icmp eq i64 %t89, %t91
  %t92 = zext i1 %t90 to i64
  %t93 = icmp ne i64 %t92, 0
  br i1 %t93, label %L17, label %L18
L17:
  %t94 = load ptr, ptr %t6
  %t96 = ptrtoint ptr %t94 to i64
  %t97 = sext i32 7 to i64
  %t95 = add i64 %t96, %t97
  store i64 %t95, ptr %t6
  %t98 = load ptr, ptr %t6
  %t99 = call ptr @skip_ws(ptr %t98)
  store ptr %t99, ptr %t6
  %t100 = load ptr, ptr %t6
  %t101 = load i64, ptr %t100
  %t103 = sext i32 40 to i64
  %t102 = icmp eq i64 %t101, %t103
  %t104 = zext i1 %t102 to i64
  %t105 = icmp ne i64 %t104, 0
  br i1 %t105, label %L20, label %L22
L20:
  %t106 = load ptr, ptr %t6
  %t108 = ptrtoint ptr %t106 to i64
  %t107 = add i64 %t108, 1
  store i64 %t107, ptr %t6
  br label %L22
L22:
  %t109 = load ptr, ptr %t6
  %t110 = call ptr @skip_ws(ptr %t109)
  store ptr %t110, ptr %t6
  %t111 = alloca ptr
  %t112 = alloca ptr
  %t113 = load ptr, ptr %t6
  %t114 = load ptr, ptr %t111
  %t115 = call ptr @read_ident(ptr %t113, ptr %t114, i64 8)
  store ptr %t115, ptr %t112
  %t116 = load ptr, ptr %t112
  %t117 = ptrtoint ptr %t116 to i64
  %t118 = load ptr, ptr %t111
  %t119 = call i32 @macro_find_idx(ptr %t118)
  %t120 = sext i32 %t119 to i64
  %t122 = sext i32 0 to i64
  %t121 = icmp sge i64 %t120, %t122
  %t123 = zext i1 %t121 to i64
  store i64 %t123, ptr %t84
  br label %L19
L18:
  %t124 = load ptr, ptr %t6
  %t125 = call i32 @atoi(ptr %t124)
  %t126 = sext i32 %t125 to i64
  %t128 = sext i32 0 to i64
  %t127 = icmp ne i64 %t126, %t128
  %t129 = zext i1 %t127 to i64
  store i64 %t129, ptr %t84
  br label %L19
L19:
  %t130 = load i64, ptr %t5
  %t132 = sext i32 32 to i64
  %t131 = icmp slt i64 %t130, %t132
  %t133 = zext i1 %t131 to i64
  %t134 = icmp ne i64 %t133, 0
  br i1 %t134, label %L23, label %L25
L23:
  %t135 = load i64, ptr %t84
  %t136 = load i64, ptr %t5
  %t137 = add i64 %t136, 1
  store i64 %t137, ptr %t5
  %t138 = getelementptr ptr, ptr %t4, i64 %t136
  %t139 = sext i32 %t135 to i64
  store i64 %t139, ptr %t138
  br label %L25
L25:
  ret void
L26:
  br label %L16
L16:
  %t140 = load ptr, ptr %t12
  %t141 = getelementptr [5 x i8], ptr @.str14, i64 0, i64 0
  %t142 = call i32 @strcmp(ptr %t140, ptr %t141)
  %t143 = sext i32 %t142 to i64
  %t145 = sext i32 0 to i64
  %t144 = icmp eq i64 %t143, %t145
  %t146 = zext i1 %t144 to i64
  %t147 = icmp ne i64 %t146, 0
  br i1 %t147, label %L27, label %L29
L27:
  %t148 = load i64, ptr %t5
  %t150 = sext i32 0 to i64
  %t149 = icmp sgt i64 %t148, %t150
  %t151 = zext i1 %t149 to i64
  %t152 = icmp ne i64 %t151, 0
  br i1 %t152, label %L30, label %L32
L30:
  %t153 = alloca i64
  %t154 = load ptr, ptr %t6
  %t155 = call i32 @atoi(ptr %t154)
  %t156 = sext i32 %t155 to i64
  %t158 = sext i32 0 to i64
  %t157 = icmp ne i64 %t156, %t158
  %t159 = zext i1 %t157 to i64
  store i64 %t159, ptr %t153
  %t160 = load i64, ptr %t153
  %t161 = load i64, ptr %t5
  %t163 = sext i32 1 to i64
  %t162 = sub i64 %t161, %t163
  %t164 = getelementptr ptr, ptr %t4, i64 %t162
  %t165 = sext i32 %t160 to i64
  store i64 %t165, ptr %t164
  br label %L32
L32:
  ret void
L33:
  br label %L29
L29:
  %t166 = load ptr, ptr %t12
  %t167 = getelementptr [5 x i8], ptr @.str15, i64 0, i64 0
  %t168 = call i32 @strcmp(ptr %t166, ptr %t167)
  %t169 = sext i32 %t168 to i64
  %t171 = sext i32 0 to i64
  %t170 = icmp eq i64 %t169, %t171
  %t172 = zext i1 %t170 to i64
  %t173 = icmp ne i64 %t172, 0
  br i1 %t173, label %L34, label %L36
L34:
  %t174 = load i64, ptr %t5
  %t176 = sext i32 0 to i64
  %t175 = icmp sgt i64 %t174, %t176
  %t177 = zext i1 %t175 to i64
  %t178 = icmp ne i64 %t177, 0
  br i1 %t178, label %L37, label %L39
L37:
  %t179 = load i64, ptr %t5
  %t181 = sext i32 1 to i64
  %t180 = sub i64 %t179, %t181
  %t182 = getelementptr ptr, ptr %t4, i64 %t180
  %t183 = load ptr, ptr %t182
  %t185 = ptrtoint ptr %t183 to i64
  %t186 = sext i32 1 to i64
  %t184 = xor i64 %t185, %t186
  %t187 = load i64, ptr %t5
  %t189 = sext i32 1 to i64
  %t188 = sub i64 %t187, %t189
  %t190 = getelementptr ptr, ptr %t4, i64 %t188
  store i64 %t184, ptr %t190
  br label %L39
L39:
  ret void
L40:
  br label %L36
L36:
  %t191 = load ptr, ptr %t12
  %t192 = getelementptr [6 x i8], ptr @.str16, i64 0, i64 0
  %t193 = call i32 @strcmp(ptr %t191, ptr %t192)
  %t194 = sext i32 %t193 to i64
  %t196 = sext i32 0 to i64
  %t195 = icmp eq i64 %t194, %t196
  %t197 = zext i1 %t195 to i64
  %t198 = icmp ne i64 %t197, 0
  br i1 %t198, label %L41, label %L43
L41:
  %t199 = load i64, ptr %t5
  %t201 = sext i32 0 to i64
  %t200 = icmp sgt i64 %t199, %t201
  %t202 = zext i1 %t200 to i64
  %t203 = icmp ne i64 %t202, 0
  br i1 %t203, label %L44, label %L46
L44:
  %t204 = load i64, ptr %t5
  %t205 = sub i64 %t204, 1
  store i64 %t205, ptr %t5
  br label %L46
L46:
  ret void
L47:
  br label %L43
L43:
  %t206 = alloca i64
  %t207 = sext i32 1 to i64
  store i64 %t207, ptr %t206
  %t208 = alloca i64
  %t209 = sext i32 0 to i64
  store i64 %t209, ptr %t208
  br label %L48
L48:
  %t210 = load i64, ptr %t208
  %t211 = load i64, ptr %t5
  %t213 = sext i32 %t210 to i64
  %t212 = icmp slt i64 %t213, %t211
  %t214 = zext i1 %t212 to i64
  %t215 = icmp ne i64 %t214, 0
  br i1 %t215, label %L49, label %L51
L49:
  %t216 = load i64, ptr %t208
  %t217 = sext i32 %t216 to i64
  %t218 = getelementptr ptr, ptr %t4, i64 %t217
  %t219 = load ptr, ptr %t218
  %t221 = ptrtoint ptr %t219 to i64
  %t222 = icmp eq i64 %t221, 0
  %t220 = zext i1 %t222 to i64
  %t223 = icmp ne i64 %t220, 0
  br i1 %t223, label %L52, label %L54
L52:
  %t224 = sext i32 0 to i64
  store i64 %t224, ptr %t206
  br label %L51
L55:
  br label %L54
L54:
  br label %L50
L50:
  %t225 = load i64, ptr %t208
  %t227 = sext i32 %t225 to i64
  %t226 = add i64 %t227, 1
  store i64 %t226, ptr %t208
  br label %L48
L51:
  %t228 = load i64, ptr %t206
  %t230 = sext i32 %t228 to i64
  %t231 = icmp eq i64 %t230, 0
  %t229 = zext i1 %t231 to i64
  %t232 = icmp ne i64 %t229, 0
  br i1 %t232, label %L56, label %L58
L56:
  ret void
L59:
  br label %L58
L58:
  %t233 = load ptr, ptr %t12
  %t234 = getelementptr [7 x i8], ptr @.str17, i64 0, i64 0
  %t235 = call i32 @strcmp(ptr %t233, ptr %t234)
  %t236 = sext i32 %t235 to i64
  %t238 = sext i32 0 to i64
  %t237 = icmp eq i64 %t236, %t238
  %t239 = zext i1 %t237 to i64
  %t240 = icmp ne i64 %t239, 0
  br i1 %t240, label %L60, label %L62
L60:
  %t241 = alloca ptr
  %t242 = load ptr, ptr %t6
  %t243 = load ptr, ptr %t241
  %t244 = call ptr @read_ident(ptr %t242, ptr %t243, i64 8)
  store ptr %t244, ptr %t6
  %t245 = load ptr, ptr %t6
  %t246 = load i64, ptr %t245
  %t248 = sext i32 40 to i64
  %t247 = icmp eq i64 %t246, %t248
  %t249 = zext i1 %t247 to i64
  %t250 = icmp ne i64 %t249, 0
  br i1 %t250, label %L63, label %L64
L63:
  %t251 = load ptr, ptr %t6
  %t253 = ptrtoint ptr %t251 to i64
  %t252 = add i64 %t253, 1
  store i64 %t252, ptr %t6
  %t254 = alloca ptr
  %t255 = sext i32 0 to i64
  store i64 %t255, ptr %t254
  %t256 = alloca i64
  %t257 = sext i32 0 to i64
  store i64 %t257, ptr %t256
  %t258 = alloca i64
  %t259 = sext i32 0 to i64
  store i64 %t259, ptr %t258
  br label %L66
L66:
  %t260 = load ptr, ptr %t6
  %t261 = load i64, ptr %t260
  %t262 = icmp ne i64 %t261, 0
  br i1 %t262, label %L69, label %L70
L69:
  %t263 = load ptr, ptr %t6
  %t264 = load i64, ptr %t263
  %t266 = sext i32 41 to i64
  %t265 = icmp ne i64 %t264, %t266
  %t267 = zext i1 %t265 to i64
  %t268 = icmp ne i64 %t267, 0
  %t269 = zext i1 %t268 to i64
  br label %L71
L70:
  br label %L71
L71:
  %t270 = phi i64 [ %t269, %L69 ], [ 0, %L70 ]
  %t271 = icmp ne i64 %t270, 0
  br i1 %t271, label %L67, label %L68
L67:
  %t272 = load ptr, ptr %t6
  %t273 = call ptr @skip_ws(ptr %t272)
  store ptr %t273, ptr %t6
  %t274 = load ptr, ptr %t6
  %t275 = load i64, ptr %t274
  %t277 = sext i32 41 to i64
  %t276 = icmp eq i64 %t275, %t277
  %t278 = zext i1 %t276 to i64
  %t279 = icmp ne i64 %t278, 0
  br i1 %t279, label %L72, label %L74
L72:
  br label %L68
L75:
  br label %L74
L74:
  %t280 = load ptr, ptr %t6
  %t281 = load i64, ptr %t280
  %t283 = sext i32 46 to i64
  %t282 = icmp eq i64 %t281, %t283
  %t284 = zext i1 %t282 to i64
  %t285 = icmp ne i64 %t284, 0
  br i1 %t285, label %L76, label %L77
L76:
  %t286 = load ptr, ptr %t6
  %t288 = ptrtoint ptr %t286 to i64
  %t289 = sext i32 1 to i64
  %t290 = inttoptr i64 %t288 to ptr
  %t287 = getelementptr i8, ptr %t290, i64 %t289
  %t291 = load i64, ptr %t287
  %t293 = sext i32 46 to i64
  %t292 = icmp eq i64 %t291, %t293
  %t294 = zext i1 %t292 to i64
  %t295 = icmp ne i64 %t294, 0
  %t296 = zext i1 %t295 to i64
  br label %L78
L77:
  br label %L78
L78:
  %t297 = phi i64 [ %t296, %L76 ], [ 0, %L77 ]
  %t298 = icmp ne i64 %t297, 0
  br i1 %t298, label %L79, label %L80
L79:
  %t299 = load ptr, ptr %t6
  %t301 = ptrtoint ptr %t299 to i64
  %t302 = sext i32 2 to i64
  %t303 = inttoptr i64 %t301 to ptr
  %t300 = getelementptr i8, ptr %t303, i64 %t302
  %t304 = load i64, ptr %t300
  %t306 = sext i32 46 to i64
  %t305 = icmp eq i64 %t304, %t306
  %t307 = zext i1 %t305 to i64
  %t308 = icmp ne i64 %t307, 0
  %t309 = zext i1 %t308 to i64
  br label %L81
L80:
  br label %L81
L81:
  %t310 = phi i64 [ %t309, %L79 ], [ 0, %L80 ]
  %t311 = icmp ne i64 %t310, 0
  br i1 %t311, label %L82, label %L84
L82:
  %t312 = sext i32 1 to i64
  store i64 %t312, ptr %t258
  %t313 = load ptr, ptr %t6
  %t315 = ptrtoint ptr %t313 to i64
  %t316 = sext i32 3 to i64
  %t314 = add i64 %t315, %t316
  store i64 %t314, ptr %t6
  %t317 = load ptr, ptr %t6
  %t318 = call ptr @skip_ws(ptr %t317)
  store ptr %t318, ptr %t6
  br label %L68
L85:
  br label %L84
L84:
  %t319 = alloca ptr
  %t320 = load ptr, ptr %t6
  %t321 = load ptr, ptr %t319
  %t322 = call ptr @read_ident(ptr %t320, ptr %t321, i64 8)
  store ptr %t322, ptr %t6
  %t323 = load ptr, ptr %t319
  %t324 = load i64, ptr %t323
  %t325 = icmp ne i64 %t324, 0
  br i1 %t325, label %L86, label %L87
L86:
  %t326 = load i64, ptr %t256
  %t328 = sext i32 %t326 to i64
  %t329 = sext i32 16 to i64
  %t327 = icmp slt i64 %t328, %t329
  %t330 = zext i1 %t327 to i64
  %t331 = icmp ne i64 %t330, 0
  %t332 = zext i1 %t331 to i64
  br label %L88
L87:
  br label %L88
L88:
  %t333 = phi i64 [ %t332, %L86 ], [ 0, %L87 ]
  %t334 = icmp ne i64 %t333, 0
  br i1 %t334, label %L89, label %L91
L89:
  %t335 = load ptr, ptr %t319
  %t336 = call ptr @strdup(ptr %t335)
  %t337 = load ptr, ptr %t254
  %t338 = load i64, ptr %t256
  %t340 = sext i32 %t338 to i64
  %t339 = add i64 %t340, 1
  store i64 %t339, ptr %t256
  %t342 = sext i32 %t338 to i64
  %t341 = getelementptr ptr, ptr %t337, i64 %t342
  store ptr %t336, ptr %t341
  br label %L91
L91:
  %t343 = load ptr, ptr %t6
  %t344 = call ptr @skip_ws(ptr %t343)
  store ptr %t344, ptr %t6
  %t345 = load ptr, ptr %t6
  %t346 = load i64, ptr %t345
  %t348 = sext i32 44 to i64
  %t347 = icmp eq i64 %t346, %t348
  %t349 = zext i1 %t347 to i64
  %t350 = icmp ne i64 %t349, 0
  br i1 %t350, label %L92, label %L94
L92:
  %t351 = load ptr, ptr %t6
  %t353 = ptrtoint ptr %t351 to i64
  %t352 = add i64 %t353, 1
  store i64 %t352, ptr %t6
  br label %L94
L94:
  br label %L66
L68:
  %t354 = load i64, ptr %t258
  %t356 = sext i32 %t354 to i64
  %t355 = add i64 %t356, 0
  %t357 = load ptr, ptr %t6
  %t358 = load i64, ptr %t357
  %t360 = sext i32 41 to i64
  %t359 = icmp eq i64 %t358, %t360
  %t361 = zext i1 %t359 to i64
  %t362 = icmp ne i64 %t361, 0
  br i1 %t362, label %L95, label %L97
L95:
  %t363 = load ptr, ptr %t6
  %t365 = ptrtoint ptr %t363 to i64
  %t364 = add i64 %t365, 1
  store i64 %t364, ptr %t6
  br label %L97
L97:
  %t366 = load ptr, ptr %t6
  %t367 = call ptr @skip_ws(ptr %t366)
  store ptr %t367, ptr %t6
  %t368 = alloca ptr
  %t369 = load ptr, ptr %t6
  store ptr %t369, ptr %t368
  %t370 = alloca ptr
  %t371 = load ptr, ptr %t6
  %t372 = call ptr @skip_to_eol(ptr %t371)
  store ptr %t372, ptr %t370
  %t373 = alloca ptr
  %t374 = load ptr, ptr %t370
  %t375 = load ptr, ptr %t368
  %t377 = ptrtoint ptr %t374 to i64
  %t378 = ptrtoint ptr %t375 to i64
  %t376 = sub i64 %t377, %t378
  %t380 = sext i32 1 to i64
  %t379 = add i64 %t376, %t380
  %t381 = call ptr @malloc(i64 %t379)
  store ptr %t381, ptr %t373
  %t382 = load ptr, ptr %t373
  %t383 = load ptr, ptr %t368
  %t384 = load ptr, ptr %t370
  %t385 = load ptr, ptr %t368
  %t387 = ptrtoint ptr %t384 to i64
  %t388 = ptrtoint ptr %t385 to i64
  %t386 = sub i64 %t387, %t388
  %t389 = call ptr @memcpy(ptr %t382, ptr %t383, i64 %t386)
  %t390 = load ptr, ptr %t373
  %t391 = load ptr, ptr %t370
  %t392 = load ptr, ptr %t368
  %t394 = ptrtoint ptr %t391 to i64
  %t395 = ptrtoint ptr %t392 to i64
  %t393 = sub i64 %t394, %t395
  %t396 = getelementptr ptr, ptr %t390, i64 %t393
  %t397 = sext i32 0 to i64
  store i64 %t397, ptr %t396
  %t398 = load ptr, ptr %t241
  %t399 = load ptr, ptr %t373
  %t400 = load ptr, ptr %t254
  %t401 = load i64, ptr %t256
  call void @macro_define(ptr %t398, ptr %t399, ptr %t400, i64 %t401, i64 1)
  %t403 = load ptr, ptr %t373
  call void @free(ptr %t403)
  %t405 = alloca i64
  %t406 = sext i32 0 to i64
  store i64 %t406, ptr %t405
  br label %L98
L98:
  %t407 = load i64, ptr %t405
  %t408 = load i64, ptr %t256
  %t410 = sext i32 %t407 to i64
  %t411 = sext i32 %t408 to i64
  %t409 = icmp slt i64 %t410, %t411
  %t412 = zext i1 %t409 to i64
  %t413 = icmp ne i64 %t412, 0
  br i1 %t413, label %L99, label %L101
L99:
  %t414 = load ptr, ptr %t254
  %t415 = load i64, ptr %t405
  %t416 = sext i32 %t415 to i64
  %t417 = getelementptr ptr, ptr %t414, i64 %t416
  %t418 = load ptr, ptr %t417
  call void @free(ptr %t418)
  br label %L100
L100:
  %t420 = load i64, ptr %t405
  %t422 = sext i32 %t420 to i64
  %t421 = add i64 %t422, 1
  store i64 %t421, ptr %t405
  br label %L98
L101:
  br label %L65
L64:
  %t423 = load ptr, ptr %t6
  %t424 = load i64, ptr %t423
  %t426 = sext i32 32 to i64
  %t425 = icmp eq i64 %t424, %t426
  %t427 = zext i1 %t425 to i64
  %t428 = icmp ne i64 %t427, 0
  br i1 %t428, label %L102, label %L103
L102:
  br label %L104
L103:
  %t429 = load ptr, ptr %t6
  %t430 = load i64, ptr %t429
  %t432 = sext i32 9 to i64
  %t431 = icmp eq i64 %t430, %t432
  %t433 = zext i1 %t431 to i64
  %t434 = icmp ne i64 %t433, 0
  %t435 = zext i1 %t434 to i64
  br label %L104
L104:
  %t436 = phi i64 [ 1, %L102 ], [ %t435, %L103 ]
  %t437 = icmp ne i64 %t436, 0
  br i1 %t437, label %L105, label %L107
L105:
  %t438 = load ptr, ptr %t6
  %t440 = ptrtoint ptr %t438 to i64
  %t439 = add i64 %t440, 1
  store i64 %t439, ptr %t6
  br label %L107
L107:
  %t441 = alloca ptr
  %t442 = load ptr, ptr %t6
  store ptr %t442, ptr %t441
  %t443 = alloca ptr
  %t444 = load ptr, ptr %t6
  %t445 = call ptr @skip_to_eol(ptr %t444)
  store ptr %t445, ptr %t443
  %t446 = alloca ptr
  %t447 = load ptr, ptr %t443
  %t448 = load ptr, ptr %t441
  %t450 = ptrtoint ptr %t447 to i64
  %t451 = ptrtoint ptr %t448 to i64
  %t449 = sub i64 %t450, %t451
  %t453 = sext i32 1 to i64
  %t452 = add i64 %t449, %t453
  %t454 = call ptr @malloc(i64 %t452)
  store ptr %t454, ptr %t446
  %t455 = load ptr, ptr %t446
  %t456 = load ptr, ptr %t441
  %t457 = load ptr, ptr %t443
  %t458 = load ptr, ptr %t441
  %t460 = ptrtoint ptr %t457 to i64
  %t461 = ptrtoint ptr %t458 to i64
  %t459 = sub i64 %t460, %t461
  %t462 = call ptr @memcpy(ptr %t455, ptr %t456, i64 %t459)
  %t463 = load ptr, ptr %t446
  %t464 = load ptr, ptr %t443
  %t465 = load ptr, ptr %t441
  %t467 = ptrtoint ptr %t464 to i64
  %t468 = ptrtoint ptr %t465 to i64
  %t466 = sub i64 %t467, %t468
  %t469 = getelementptr ptr, ptr %t463, i64 %t466
  %t470 = sext i32 0 to i64
  store i64 %t470, ptr %t469
  %t471 = load ptr, ptr %t241
  %t472 = load ptr, ptr %t446
  %t474 = sext i32 0 to i64
  %t473 = inttoptr i64 %t474 to ptr
  call void @macro_define(ptr %t471, ptr %t472, ptr %t473, i64 0, i64 0)
  %t476 = load ptr, ptr %t446
  call void @free(ptr %t476)
  br label %L65
L65:
  ret void
L108:
  br label %L62
L62:
  %t478 = load ptr, ptr %t12
  %t479 = getelementptr [6 x i8], ptr @.str18, i64 0, i64 0
  %t480 = call i32 @strcmp(ptr %t478, ptr %t479)
  %t481 = sext i32 %t480 to i64
  %t483 = sext i32 0 to i64
  %t482 = icmp eq i64 %t481, %t483
  %t484 = zext i1 %t482 to i64
  %t485 = icmp ne i64 %t484, 0
  br i1 %t485, label %L109, label %L111
L109:
  %t486 = alloca ptr
  %t487 = load ptr, ptr %t6
  %t488 = load ptr, ptr %t486
  %t489 = call ptr @read_ident(ptr %t487, ptr %t488, i64 8)
  %t490 = load ptr, ptr %t486
  call void @macro_undef(ptr %t490)
  ret void
L112:
  br label %L111
L111:
  %t492 = load ptr, ptr %t12
  %t493 = getelementptr [8 x i8], ptr @.str19, i64 0, i64 0
  %t494 = call i32 @strcmp(ptr %t492, ptr %t493)
  %t495 = sext i32 %t494 to i64
  %t497 = sext i32 0 to i64
  %t496 = icmp eq i64 %t495, %t497
  %t498 = zext i1 %t496 to i64
  %t499 = icmp ne i64 %t498, 0
  br i1 %t499, label %L113, label %L115
L113:
  %t501 = sext i32 32 to i64
  %t500 = icmp sgt i64 %t3, %t501
  %t502 = zext i1 %t500 to i64
  %t503 = icmp ne i64 %t502, 0
  br i1 %t503, label %L116, label %L118
L116:
  %t504 = call ptr @__c0c_stderr()
  %t505 = getelementptr [36 x i8], ptr @.str20, i64 0, i64 0
  %t506 = call i32 (ptr, ...) @fprintf(ptr %t504, ptr %t505)
  %t507 = sext i32 %t506 to i64
  ret void
L119:
  br label %L118
L118:
  %t508 = alloca i64
  %t509 = sext i32 0 to i64
  store i64 %t509, ptr %t508
  %t510 = alloca ptr
  %t511 = load ptr, ptr %t6
  %t512 = load i64, ptr %t511
  %t514 = sext i32 34 to i64
  %t513 = icmp eq i64 %t512, %t514
  %t515 = zext i1 %t513 to i64
  %t516 = icmp ne i64 %t515, 0
  br i1 %t516, label %L120, label %L121
L120:
  %t517 = load ptr, ptr %t6
  %t519 = ptrtoint ptr %t517 to i64
  %t518 = add i64 %t519, 1
  store i64 %t518, ptr %t6
  %t520 = alloca ptr
  %t521 = load ptr, ptr %t6
  %t522 = call ptr @strchr(ptr %t521, i64 34)
  store ptr %t522, ptr %t520
  %t523 = load ptr, ptr %t520
  %t525 = ptrtoint ptr %t523 to i64
  %t526 = icmp eq i64 %t525, 0
  %t524 = zext i1 %t526 to i64
  %t527 = icmp ne i64 %t524, 0
  br i1 %t527, label %L123, label %L125
L123:
  ret void
L126:
  br label %L125
L125:
  %t528 = alloca i64
  %t529 = load ptr, ptr %t520
  %t530 = load ptr, ptr %t6
  %t532 = ptrtoint ptr %t529 to i64
  %t533 = ptrtoint ptr %t530 to i64
  %t531 = sub i64 %t532, %t533
  %t534 = add i64 %t531, 0
  store i64 %t534, ptr %t528
  %t535 = load ptr, ptr %t510
  %t536 = load ptr, ptr %t6
  %t537 = load i64, ptr %t528
  %t538 = call ptr @memcpy(ptr %t535, ptr %t536, i64 %t537)
  %t539 = load ptr, ptr %t510
  %t540 = load i64, ptr %t528
  %t542 = sext i32 %t540 to i64
  %t541 = getelementptr ptr, ptr %t539, i64 %t542
  %t543 = sext i32 0 to i64
  store i64 %t543, ptr %t541
  br label %L122
L121:
  %t544 = load ptr, ptr %t6
  %t545 = load i64, ptr %t544
  %t547 = sext i32 60 to i64
  %t546 = icmp eq i64 %t545, %t547
  %t548 = zext i1 %t546 to i64
  %t549 = icmp ne i64 %t548, 0
  br i1 %t549, label %L127, label %L128
L127:
  %t550 = load ptr, ptr %t6
  %t552 = ptrtoint ptr %t550 to i64
  %t551 = add i64 %t552, 1
  store i64 %t551, ptr %t6
  %t553 = alloca ptr
  %t554 = load ptr, ptr %t6
  %t555 = call ptr @strchr(ptr %t554, i64 62)
  store ptr %t555, ptr %t553
  %t556 = load ptr, ptr %t553
  %t558 = ptrtoint ptr %t556 to i64
  %t559 = icmp eq i64 %t558, 0
  %t557 = zext i1 %t559 to i64
  %t560 = icmp ne i64 %t557, 0
  br i1 %t560, label %L130, label %L132
L130:
  ret void
L133:
  br label %L132
L132:
  %t561 = alloca i64
  %t562 = load ptr, ptr %t553
  %t563 = load ptr, ptr %t6
  %t565 = ptrtoint ptr %t562 to i64
  %t566 = ptrtoint ptr %t563 to i64
  %t564 = sub i64 %t565, %t566
  %t567 = add i64 %t564, 0
  store i64 %t567, ptr %t561
  %t568 = load ptr, ptr %t510
  %t569 = load ptr, ptr %t6
  %t570 = load i64, ptr %t561
  %t571 = call ptr @memcpy(ptr %t568, ptr %t569, i64 %t570)
  %t572 = load ptr, ptr %t510
  %t573 = load i64, ptr %t561
  %t575 = sext i32 %t573 to i64
  %t574 = getelementptr ptr, ptr %t572, i64 %t575
  %t576 = sext i32 0 to i64
  store i64 %t576, ptr %t574
  %t577 = sext i32 1 to i64
  store i64 %t577, ptr %t508
  br label %L129
L128:
  ret void
L134:
  br label %L129
L129:
  br label %L122
L122:
  %t578 = alloca ptr
  %t579 = load ptr, ptr %t510
  %t580 = load i64, ptr %t508
  %t581 = call ptr @find_include(ptr %t579, i64 %t580)
  store ptr %t581, ptr %t578
  %t582 = load ptr, ptr %t578
  %t584 = ptrtoint ptr %t582 to i64
  %t585 = icmp eq i64 %t584, 0
  %t583 = zext i1 %t585 to i64
  %t586 = icmp ne i64 %t583, 0
  br i1 %t586, label %L135, label %L137
L135:
  %t587 = getelementptr [2 x i8], ptr @.str21, i64 0, i64 0
  call void @buf_append(ptr %t2, ptr %t587, i64 1)
  ret void
L138:
  br label %L137
L137:
  %t589 = load i64, ptr %t508
  %t591 = sext i32 %t589 to i64
  %t590 = icmp ne i64 %t591, 0
  br i1 %t590, label %L139, label %L141
L139:
  %t592 = load ptr, ptr %t578
  call void @free(ptr %t592)
  %t594 = getelementptr [2 x i8], ptr @.str22, i64 0, i64 0
  call void @buf_append(ptr %t2, ptr %t594, i64 1)
  ret void
L142:
  br label %L141
L141:
  %t596 = alloca ptr
  %t597 = load ptr, ptr %t578
  %t598 = load ptr, ptr %t510
  %t600 = sext i32 1 to i64
  %t599 = add i64 %t3, %t600
  %t601 = call ptr @macro_preprocess(ptr %t597, ptr %t598, i64 %t599)
  store ptr %t601, ptr %t596
  %t602 = load ptr, ptr %t578
  call void @free(ptr %t602)
  %t604 = load ptr, ptr %t596
  %t605 = load ptr, ptr %t596
  %t606 = call i64 @strlen(ptr %t605)
  call void @buf_append(ptr %t2, ptr %t604, i64 %t606)
  call void @buf_putc(ptr %t2, i64 10)
  %t609 = load ptr, ptr %t596
  call void @free(ptr %t609)
  ret void
L143:
  br label %L115
L115:
  ret void
}

define internal void @preprocess_into(ptr %t0, ptr %t1, ptr %t2, i64 %t3, ptr %t4, ptr %t5) {
entry:
  %t6 = alloca ptr
  store ptr %t0, ptr %t6
  br label %L0
L0:
  %t7 = load ptr, ptr %t6
  %t8 = load i64, ptr %t7
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L1, label %L2
L1:
  %t10 = load ptr, ptr %t6
  %t11 = load i64, ptr %t10
  %t13 = sext i32 92 to i64
  %t12 = icmp eq i64 %t11, %t13
  %t14 = zext i1 %t12 to i64
  %t15 = icmp ne i64 %t14, 0
  br i1 %t15, label %L3, label %L4
L3:
  %t16 = load ptr, ptr %t6
  %t18 = ptrtoint ptr %t16 to i64
  %t19 = sext i32 1 to i64
  %t20 = inttoptr i64 %t18 to ptr
  %t17 = getelementptr i8, ptr %t20, i64 %t19
  %t21 = load i64, ptr %t17
  %t23 = sext i32 10 to i64
  %t22 = icmp eq i64 %t21, %t23
  %t24 = zext i1 %t22 to i64
  %t25 = icmp ne i64 %t24, 0
  %t26 = zext i1 %t25 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t27 = phi i64 [ %t26, %L3 ], [ 0, %L4 ]
  %t28 = icmp ne i64 %t27, 0
  br i1 %t28, label %L6, label %L8
L6:
  %t29 = load ptr, ptr %t6
  %t31 = ptrtoint ptr %t29 to i64
  %t32 = sext i32 2 to i64
  %t30 = add i64 %t31, %t32
  store i64 %t30, ptr %t6
  br label %L0
L9:
  br label %L8
L8:
  %t33 = alloca i64
  %t34 = call i64 @buf_new()
  store i64 %t34, ptr %t33
  br label %L10
L10:
  %t35 = load ptr, ptr %t6
  %t36 = load i64, ptr %t35
  %t37 = icmp ne i64 %t36, 0
  br i1 %t37, label %L13, label %L14
L13:
  %t38 = load ptr, ptr %t6
  %t39 = load i64, ptr %t38
  %t41 = sext i32 10 to i64
  %t40 = icmp ne i64 %t39, %t41
  %t42 = zext i1 %t40 to i64
  %t43 = icmp ne i64 %t42, 0
  %t44 = zext i1 %t43 to i64
  br label %L15
L14:
  br label %L15
L15:
  %t45 = phi i64 [ %t44, %L13 ], [ 0, %L14 ]
  %t46 = icmp ne i64 %t45, 0
  br i1 %t46, label %L11, label %L12
L11:
  %t47 = load ptr, ptr %t6
  %t48 = load i64, ptr %t47
  %t50 = sext i32 92 to i64
  %t49 = icmp eq i64 %t48, %t50
  %t51 = zext i1 %t49 to i64
  %t52 = icmp ne i64 %t51, 0
  br i1 %t52, label %L16, label %L17
L16:
  %t53 = load ptr, ptr %t6
  %t55 = ptrtoint ptr %t53 to i64
  %t56 = sext i32 1 to i64
  %t57 = inttoptr i64 %t55 to ptr
  %t54 = getelementptr i8, ptr %t57, i64 %t56
  %t58 = load i64, ptr %t54
  %t60 = sext i32 10 to i64
  %t59 = icmp eq i64 %t58, %t60
  %t61 = zext i1 %t59 to i64
  %t62 = icmp ne i64 %t61, 0
  %t63 = zext i1 %t62 to i64
  br label %L18
L17:
  br label %L18
L18:
  %t64 = phi i64 [ %t63, %L16 ], [ 0, %L17 ]
  %t65 = icmp ne i64 %t64, 0
  br i1 %t65, label %L19, label %L21
L19:
  %t66 = load ptr, ptr %t6
  %t68 = ptrtoint ptr %t66 to i64
  %t69 = sext i32 2 to i64
  %t67 = add i64 %t68, %t69
  store i64 %t67, ptr %t6
  br label %L10
L22:
  br label %L21
L21:
  %t70 = load ptr, ptr %t6
  %t71 = load i64, ptr %t70
  %t73 = sext i32 39 to i64
  %t72 = icmp eq i64 %t71, %t73
  %t74 = zext i1 %t72 to i64
  %t75 = icmp ne i64 %t74, 0
  br i1 %t75, label %L23, label %L25
L23:
  %t76 = load i64, ptr %t33
  %t77 = load ptr, ptr %t6
  %t79 = ptrtoint ptr %t77 to i64
  %t78 = add i64 %t79, 1
  store i64 %t78, ptr %t6
  %t80 = load i64, ptr %t77
  call void @buf_putc(i64 %t76, i64 %t80)
  %t82 = load ptr, ptr %t6
  %t83 = load i64, ptr %t82
  %t85 = sext i32 92 to i64
  %t84 = icmp eq i64 %t83, %t85
  %t86 = zext i1 %t84 to i64
  %t87 = icmp ne i64 %t86, 0
  br i1 %t87, label %L26, label %L27
L26:
  %t88 = load ptr, ptr %t6
  %t90 = ptrtoint ptr %t88 to i64
  %t91 = sext i32 1 to i64
  %t92 = inttoptr i64 %t90 to ptr
  %t89 = getelementptr i8, ptr %t92, i64 %t91
  %t93 = load i64, ptr %t89
  %t94 = icmp ne i64 %t93, 0
  %t95 = zext i1 %t94 to i64
  br label %L28
L27:
  br label %L28
L28:
  %t96 = phi i64 [ %t95, %L26 ], [ 0, %L27 ]
  %t97 = icmp ne i64 %t96, 0
  br i1 %t97, label %L29, label %L30
L29:
  %t98 = load ptr, ptr %t6
  %t100 = ptrtoint ptr %t98 to i64
  %t101 = sext i32 1 to i64
  %t102 = inttoptr i64 %t100 to ptr
  %t99 = getelementptr i8, ptr %t102, i64 %t101
  %t103 = load i64, ptr %t99
  %t105 = sext i32 10 to i64
  %t104 = icmp ne i64 %t103, %t105
  %t106 = zext i1 %t104 to i64
  %t107 = icmp ne i64 %t106, 0
  %t108 = zext i1 %t107 to i64
  br label %L31
L30:
  br label %L31
L31:
  %t109 = phi i64 [ %t108, %L29 ], [ 0, %L30 ]
  %t110 = icmp ne i64 %t109, 0
  br i1 %t110, label %L32, label %L34
L32:
  %t111 = load i64, ptr %t33
  %t112 = load ptr, ptr %t6
  %t114 = ptrtoint ptr %t112 to i64
  %t113 = add i64 %t114, 1
  store i64 %t113, ptr %t6
  %t115 = load i64, ptr %t112
  call void @buf_putc(i64 %t111, i64 %t115)
  br label %L34
L34:
  %t117 = load ptr, ptr %t6
  %t118 = load i64, ptr %t117
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L35, label %L36
L35:
  %t120 = load ptr, ptr %t6
  %t121 = load i64, ptr %t120
  %t123 = sext i32 10 to i64
  %t122 = icmp ne i64 %t121, %t123
  %t124 = zext i1 %t122 to i64
  %t125 = icmp ne i64 %t124, 0
  %t126 = zext i1 %t125 to i64
  br label %L37
L36:
  br label %L37
L37:
  %t127 = phi i64 [ %t126, %L35 ], [ 0, %L36 ]
  %t128 = icmp ne i64 %t127, 0
  br i1 %t128, label %L38, label %L40
L38:
  %t129 = load i64, ptr %t33
  %t130 = load ptr, ptr %t6
  %t132 = ptrtoint ptr %t130 to i64
  %t131 = add i64 %t132, 1
  store i64 %t131, ptr %t6
  %t133 = load i64, ptr %t130
  call void @buf_putc(i64 %t129, i64 %t133)
  br label %L40
L40:
  %t135 = load ptr, ptr %t6
  %t136 = load i64, ptr %t135
  %t138 = sext i32 39 to i64
  %t137 = icmp eq i64 %t136, %t138
  %t139 = zext i1 %t137 to i64
  %t140 = icmp ne i64 %t139, 0
  br i1 %t140, label %L41, label %L43
L41:
  %t141 = load i64, ptr %t33
  %t142 = load ptr, ptr %t6
  %t144 = ptrtoint ptr %t142 to i64
  %t143 = add i64 %t144, 1
  store i64 %t143, ptr %t6
  %t145 = load i64, ptr %t142
  call void @buf_putc(i64 %t141, i64 %t145)
  br label %L43
L43:
  br label %L10
L44:
  br label %L25
L25:
  %t147 = load ptr, ptr %t6
  %t148 = load i64, ptr %t147
  %t150 = sext i32 34 to i64
  %t149 = icmp eq i64 %t148, %t150
  %t151 = zext i1 %t149 to i64
  %t152 = icmp ne i64 %t151, 0
  br i1 %t152, label %L45, label %L47
L45:
  %t153 = load i64, ptr %t33
  %t154 = load ptr, ptr %t6
  %t156 = ptrtoint ptr %t154 to i64
  %t155 = add i64 %t156, 1
  store i64 %t155, ptr %t6
  %t157 = load i64, ptr %t154
  call void @buf_putc(i64 %t153, i64 %t157)
  br label %L48
L48:
  %t159 = load ptr, ptr %t6
  %t160 = load i64, ptr %t159
  %t161 = icmp ne i64 %t160, 0
  br i1 %t161, label %L51, label %L52
L51:
  %t162 = load ptr, ptr %t6
  %t163 = load i64, ptr %t162
  %t165 = sext i32 34 to i64
  %t164 = icmp ne i64 %t163, %t165
  %t166 = zext i1 %t164 to i64
  %t167 = icmp ne i64 %t166, 0
  %t168 = zext i1 %t167 to i64
  br label %L53
L52:
  br label %L53
L53:
  %t169 = phi i64 [ %t168, %L51 ], [ 0, %L52 ]
  %t170 = icmp ne i64 %t169, 0
  br i1 %t170, label %L54, label %L55
L54:
  %t171 = load ptr, ptr %t6
  %t172 = load i64, ptr %t171
  %t174 = sext i32 10 to i64
  %t173 = icmp ne i64 %t172, %t174
  %t175 = zext i1 %t173 to i64
  %t176 = icmp ne i64 %t175, 0
  %t177 = zext i1 %t176 to i64
  br label %L56
L55:
  br label %L56
L56:
  %t178 = phi i64 [ %t177, %L54 ], [ 0, %L55 ]
  %t179 = icmp ne i64 %t178, 0
  br i1 %t179, label %L49, label %L50
L49:
  %t180 = load ptr, ptr %t6
  %t181 = load i64, ptr %t180
  %t183 = sext i32 92 to i64
  %t182 = icmp eq i64 %t181, %t183
  %t184 = zext i1 %t182 to i64
  %t185 = icmp ne i64 %t184, 0
  br i1 %t185, label %L57, label %L58
L57:
  %t186 = load ptr, ptr %t6
  %t188 = ptrtoint ptr %t186 to i64
  %t189 = sext i32 1 to i64
  %t190 = inttoptr i64 %t188 to ptr
  %t187 = getelementptr i8, ptr %t190, i64 %t189
  %t191 = load i64, ptr %t187
  %t192 = icmp ne i64 %t191, 0
  %t193 = zext i1 %t192 to i64
  br label %L59
L58:
  br label %L59
L59:
  %t194 = phi i64 [ %t193, %L57 ], [ 0, %L58 ]
  %t195 = icmp ne i64 %t194, 0
  br i1 %t195, label %L60, label %L62
L60:
  %t196 = load i64, ptr %t33
  %t197 = load ptr, ptr %t6
  %t199 = ptrtoint ptr %t197 to i64
  %t198 = add i64 %t199, 1
  store i64 %t198, ptr %t6
  %t200 = load i64, ptr %t197
  call void @buf_putc(i64 %t196, i64 %t200)
  br label %L62
L62:
  %t202 = load i64, ptr %t33
  %t203 = load ptr, ptr %t6
  %t205 = ptrtoint ptr %t203 to i64
  %t204 = add i64 %t205, 1
  store i64 %t204, ptr %t6
  %t206 = load i64, ptr %t203
  call void @buf_putc(i64 %t202, i64 %t206)
  br label %L48
L50:
  %t208 = load ptr, ptr %t6
  %t209 = load i64, ptr %t208
  %t211 = sext i32 34 to i64
  %t210 = icmp eq i64 %t209, %t211
  %t212 = zext i1 %t210 to i64
  %t213 = icmp ne i64 %t212, 0
  br i1 %t213, label %L63, label %L65
L63:
  %t214 = load i64, ptr %t33
  %t215 = load ptr, ptr %t6
  %t217 = ptrtoint ptr %t215 to i64
  %t216 = add i64 %t217, 1
  store i64 %t216, ptr %t6
  %t218 = load i64, ptr %t215
  call void @buf_putc(i64 %t214, i64 %t218)
  br label %L65
L65:
  br label %L10
L66:
  br label %L47
L47:
  %t220 = load ptr, ptr %t6
  %t221 = load i64, ptr %t220
  %t223 = sext i32 47 to i64
  %t222 = icmp eq i64 %t221, %t223
  %t224 = zext i1 %t222 to i64
  %t225 = icmp ne i64 %t224, 0
  br i1 %t225, label %L67, label %L68
L67:
  %t226 = load ptr, ptr %t6
  %t228 = ptrtoint ptr %t226 to i64
  %t229 = sext i32 1 to i64
  %t230 = inttoptr i64 %t228 to ptr
  %t227 = getelementptr i8, ptr %t230, i64 %t229
  %t231 = load i64, ptr %t227
  %t233 = sext i32 47 to i64
  %t232 = icmp eq i64 %t231, %t233
  %t234 = zext i1 %t232 to i64
  %t235 = icmp ne i64 %t234, 0
  %t236 = zext i1 %t235 to i64
  br label %L69
L68:
  br label %L69
L69:
  %t237 = phi i64 [ %t236, %L67 ], [ 0, %L68 ]
  %t238 = icmp ne i64 %t237, 0
  br i1 %t238, label %L70, label %L72
L70:
  br label %L73
L73:
  %t239 = load ptr, ptr %t6
  %t240 = load i64, ptr %t239
  %t241 = icmp ne i64 %t240, 0
  br i1 %t241, label %L76, label %L77
L76:
  %t242 = load ptr, ptr %t6
  %t243 = load i64, ptr %t242
  %t245 = sext i32 10 to i64
  %t244 = icmp ne i64 %t243, %t245
  %t246 = zext i1 %t244 to i64
  %t247 = icmp ne i64 %t246, 0
  %t248 = zext i1 %t247 to i64
  br label %L78
L77:
  br label %L78
L78:
  %t249 = phi i64 [ %t248, %L76 ], [ 0, %L77 ]
  %t250 = icmp ne i64 %t249, 0
  br i1 %t250, label %L74, label %L75
L74:
  %t251 = load ptr, ptr %t6
  %t253 = ptrtoint ptr %t251 to i64
  %t252 = add i64 %t253, 1
  store i64 %t252, ptr %t6
  br label %L73
L75:
  br label %L12
L79:
  br label %L72
L72:
  %t254 = load ptr, ptr %t6
  %t255 = load i64, ptr %t254
  %t257 = sext i32 47 to i64
  %t256 = icmp eq i64 %t255, %t257
  %t258 = zext i1 %t256 to i64
  %t259 = icmp ne i64 %t258, 0
  br i1 %t259, label %L80, label %L81
L80:
  %t260 = load ptr, ptr %t6
  %t262 = ptrtoint ptr %t260 to i64
  %t263 = sext i32 1 to i64
  %t264 = inttoptr i64 %t262 to ptr
  %t261 = getelementptr i8, ptr %t264, i64 %t263
  %t265 = load i64, ptr %t261
  %t267 = sext i32 42 to i64
  %t266 = icmp eq i64 %t265, %t267
  %t268 = zext i1 %t266 to i64
  %t269 = icmp ne i64 %t268, 0
  %t270 = zext i1 %t269 to i64
  br label %L82
L81:
  br label %L82
L82:
  %t271 = phi i64 [ %t270, %L80 ], [ 0, %L81 ]
  %t272 = icmp ne i64 %t271, 0
  br i1 %t272, label %L83, label %L85
L83:
  %t273 = load ptr, ptr %t6
  %t275 = ptrtoint ptr %t273 to i64
  %t276 = sext i32 2 to i64
  %t274 = add i64 %t275, %t276
  store i64 %t274, ptr %t6
  br label %L86
L86:
  %t277 = load ptr, ptr %t6
  %t278 = load i64, ptr %t277
  %t279 = icmp ne i64 %t278, 0
  br i1 %t279, label %L89, label %L90
L89:
  %t280 = load ptr, ptr %t6
  %t281 = load i64, ptr %t280
  %t283 = sext i32 42 to i64
  %t282 = icmp eq i64 %t281, %t283
  %t284 = zext i1 %t282 to i64
  %t285 = icmp ne i64 %t284, 0
  br i1 %t285, label %L92, label %L93
L92:
  %t286 = load ptr, ptr %t6
  %t288 = ptrtoint ptr %t286 to i64
  %t289 = sext i32 1 to i64
  %t290 = inttoptr i64 %t288 to ptr
  %t287 = getelementptr i8, ptr %t290, i64 %t289
  %t291 = load i64, ptr %t287
  %t293 = sext i32 47 to i64
  %t292 = icmp eq i64 %t291, %t293
  %t294 = zext i1 %t292 to i64
  %t295 = icmp ne i64 %t294, 0
  %t296 = zext i1 %t295 to i64
  br label %L94
L93:
  br label %L94
L94:
  %t297 = phi i64 [ %t296, %L92 ], [ 0, %L93 ]
  %t299 = icmp eq i64 %t297, 0
  %t298 = zext i1 %t299 to i64
  %t300 = icmp ne i64 %t298, 0
  %t301 = zext i1 %t300 to i64
  br label %L91
L90:
  br label %L91
L91:
  %t302 = phi i64 [ %t301, %L89 ], [ 0, %L90 ]
  %t303 = icmp ne i64 %t302, 0
  br i1 %t303, label %L87, label %L88
L87:
  %t304 = load ptr, ptr %t6
  %t305 = load i64, ptr %t304
  %t307 = sext i32 10 to i64
  %t306 = icmp eq i64 %t305, %t307
  %t308 = zext i1 %t306 to i64
  %t309 = icmp ne i64 %t308, 0
  br i1 %t309, label %L95, label %L97
L95:
  %t310 = load i64, ptr %t33
  call void @buf_putc(i64 %t310, i64 32)
  br label %L97
L97:
  %t312 = load ptr, ptr %t6
  %t314 = ptrtoint ptr %t312 to i64
  %t313 = add i64 %t314, 1
  store i64 %t313, ptr %t6
  br label %L86
L88:
  %t315 = load ptr, ptr %t6
  %t316 = load i64, ptr %t315
  %t317 = icmp ne i64 %t316, 0
  br i1 %t317, label %L98, label %L100
L98:
  %t318 = load ptr, ptr %t6
  %t320 = ptrtoint ptr %t318 to i64
  %t321 = sext i32 2 to i64
  %t319 = add i64 %t320, %t321
  store i64 %t319, ptr %t6
  br label %L100
L100:
  br label %L10
L101:
  br label %L85
L85:
  %t322 = load i64, ptr %t33
  %t323 = load ptr, ptr %t6
  %t325 = ptrtoint ptr %t323 to i64
  %t324 = add i64 %t325, 1
  store i64 %t324, ptr %t6
  %t326 = load i64, ptr %t323
  call void @buf_putc(i64 %t322, i64 %t326)
  br label %L10
L12:
  %t328 = alloca ptr
  %t329 = load i64, ptr %t33
  %t330 = inttoptr i64 %t329 to ptr
  %t331 = sext i32 0 to i64
  %t332 = getelementptr ptr, ptr %t330, i64 %t331
  %t333 = load ptr, ptr %t332
  store ptr %t333, ptr %t328
  %t334 = load ptr, ptr %t6
  %t335 = load i64, ptr %t334
  %t337 = sext i32 10 to i64
  %t336 = icmp eq i64 %t335, %t337
  %t338 = zext i1 %t336 to i64
  %t339 = icmp ne i64 %t338, 0
  br i1 %t339, label %L102, label %L104
L102:
  %t340 = load ptr, ptr %t6
  %t342 = ptrtoint ptr %t340 to i64
  %t341 = add i64 %t342, 1
  store i64 %t341, ptr %t6
  br label %L104
L104:
  %t343 = alloca ptr
  %t344 = load ptr, ptr %t328
  %t345 = call ptr @skip_ws(ptr %t344)
  store ptr %t345, ptr %t343
  %t346 = load ptr, ptr %t343
  %t347 = load i64, ptr %t346
  %t349 = sext i32 35 to i64
  %t348 = icmp eq i64 %t347, %t349
  %t350 = zext i1 %t348 to i64
  %t351 = icmp ne i64 %t350, 0
  br i1 %t351, label %L105, label %L106
L105:
  %t352 = load ptr, ptr %t343
  call void @process_directive(ptr %t352, ptr %t1, ptr %t2, i64 %t3, ptr %t4, ptr %t5)
  br label %L107
L106:
  %t354 = alloca i64
  %t355 = sext i32 1 to i64
  store i64 %t355, ptr %t354
  %t356 = alloca i64
  %t357 = sext i32 0 to i64
  store i64 %t357, ptr %t356
  br label %L108
L108:
  %t358 = load i64, ptr %t356
  %t359 = load i64, ptr %t5
  %t361 = sext i32 %t358 to i64
  %t360 = icmp slt i64 %t361, %t359
  %t362 = zext i1 %t360 to i64
  %t363 = icmp ne i64 %t362, 0
  br i1 %t363, label %L109, label %L111
L109:
  %t364 = load i64, ptr %t356
  %t365 = sext i32 %t364 to i64
  %t366 = getelementptr ptr, ptr %t4, i64 %t365
  %t367 = load ptr, ptr %t366
  %t369 = ptrtoint ptr %t367 to i64
  %t370 = icmp eq i64 %t369, 0
  %t368 = zext i1 %t370 to i64
  %t371 = icmp ne i64 %t368, 0
  br i1 %t371, label %L112, label %L114
L112:
  %t372 = sext i32 0 to i64
  store i64 %t372, ptr %t354
  br label %L111
L115:
  br label %L114
L114:
  br label %L110
L110:
  %t373 = load i64, ptr %t356
  %t375 = sext i32 %t373 to i64
  %t374 = add i64 %t375, 1
  store i64 %t374, ptr %t356
  br label %L108
L111:
  %t376 = load i64, ptr %t354
  %t378 = sext i32 %t376 to i64
  %t377 = icmp ne i64 %t378, 0
  br i1 %t377, label %L116, label %L117
L116:
  %t379 = load ptr, ptr %t328
  call void @expand_text(ptr %t379, ptr %t2, i64 0)
  call void @buf_putc(ptr %t2, i64 10)
  br label %L118
L117:
  call void @buf_putc(ptr %t2, i64 10)
  br label %L118
L118:
  br label %L107
L107:
  %t383 = load ptr, ptr %t328
  call void @free(ptr %t383)
  br label %L0
L2:
  ret void
}

define dso_local ptr @macro_preprocess(ptr %t0, ptr %t1, i64 %t2) {
entry:
  call void @macros_init()
  %t4 = alloca i64
  %t5 = call i64 @buf_new()
  store i64 %t5, ptr %t4
  %t6 = alloca ptr
  %t7 = sext i32 0 to i64
  store i64 %t7, ptr %t6
  %t8 = alloca i64
  %t9 = sext i32 0 to i64
  store i64 %t9, ptr %t8
  %t11 = sext i32 0 to i64
  %t10 = icmp eq i64 %t2, %t11
  %t12 = zext i1 %t10 to i64
  %t13 = icmp ne i64 %t12, 0
  br i1 %t13, label %L0, label %L2
L0:
  %t14 = getelementptr [8 x i8], ptr @.str23, i64 0, i64 0
  %t15 = getelementptr [2 x i8], ptr @.str24, i64 0, i64 0
  %t17 = sext i32 0 to i64
  %t16 = inttoptr i64 %t17 to ptr
  call void @macro_define(ptr %t14, ptr %t15, ptr %t16, i64 0, i64 0)
  %t19 = getelementptr [9 x i8], ptr @.str25, i64 0, i64 0
  %t20 = getelementptr [2 x i8], ptr @.str26, i64 0, i64 0
  %t22 = sext i32 0 to i64
  %t21 = inttoptr i64 %t22 to ptr
  call void @macro_define(ptr %t19, ptr %t20, ptr %t21, i64 0, i64 0)
  %t24 = getelementptr [5 x i8], ptr @.str27, i64 0, i64 0
  %t25 = getelementptr [11 x i8], ptr @.str28, i64 0, i64 0
  %t27 = sext i32 0 to i64
  %t26 = inttoptr i64 %t27 to ptr
  call void @macro_define(ptr %t24, ptr %t25, ptr %t26, i64 0, i64 0)
  %t29 = getelementptr [9 x i8], ptr @.str29, i64 0, i64 0
  %t30 = getelementptr [2 x i8], ptr @.str30, i64 0, i64 0
  %t32 = sext i32 0 to i64
  %t31 = inttoptr i64 %t32 to ptr
  call void @macro_define(ptr %t29, ptr %t30, ptr %t31, i64 0, i64 0)
  %t34 = getelementptr [9 x i8], ptr @.str31, i64 0, i64 0
  %t35 = getelementptr [2 x i8], ptr @.str32, i64 0, i64 0
  %t37 = sext i32 0 to i64
  %t36 = inttoptr i64 %t37 to ptr
  call void @macro_define(ptr %t34, ptr %t35, ptr %t36, i64 0, i64 0)
  %t39 = getelementptr [9 x i8], ptr @.str33, i64 0, i64 0
  %t40 = getelementptr [2 x i8], ptr @.str34, i64 0, i64 0
  %t42 = sext i32 0 to i64
  %t41 = inttoptr i64 %t42 to ptr
  call void @macro_define(ptr %t39, ptr %t40, ptr %t41, i64 0, i64 0)
  %t44 = getelementptr [9 x i8], ptr @.str35, i64 0, i64 0
  %t45 = getelementptr [2 x i8], ptr @.str36, i64 0, i64 0
  %t47 = sext i32 0 to i64
  %t46 = inttoptr i64 %t47 to ptr
  call void @macro_define(ptr %t44, ptr %t45, ptr %t46, i64 0, i64 0)
  %t49 = getelementptr [4 x i8], ptr @.str37, i64 0, i64 0
  %t50 = getelementptr [5 x i8], ptr @.str38, i64 0, i64 0
  %t52 = sext i32 0 to i64
  %t51 = inttoptr i64 %t52 to ptr
  call void @macro_define(ptr %t49, ptr %t50, ptr %t51, i64 0, i64 0)
  %t54 = getelementptr [13 x i8], ptr @.str39, i64 0, i64 0
  %t55 = getelementptr [2 x i8], ptr @.str40, i64 0, i64 0
  %t57 = sext i32 0 to i64
  %t56 = inttoptr i64 %t57 to ptr
  call void @macro_define(ptr %t54, ptr %t55, ptr %t56, i64 0, i64 0)
  %t59 = getelementptr [13 x i8], ptr @.str41, i64 0, i64 0
  %t60 = getelementptr [2 x i8], ptr @.str42, i64 0, i64 0
  %t62 = sext i32 0 to i64
  %t61 = inttoptr i64 %t62 to ptr
  call void @macro_define(ptr %t59, ptr %t60, ptr %t61, i64 0, i64 0)
  %t64 = getelementptr [7 x i8], ptr @.str43, i64 0, i64 0
  %t65 = getelementptr [10 x i8], ptr @.str44, i64 0, i64 0
  %t67 = sext i32 0 to i64
  %t66 = inttoptr i64 %t67 to ptr
  call void @macro_define(ptr %t64, ptr %t65, ptr %t66, i64 0, i64 0)
  %t69 = getelementptr [9 x i8], ptr @.str45, i64 0, i64 0
  %t70 = getelementptr [15 x i8], ptr @.str46, i64 0, i64 0
  %t72 = sext i32 0 to i64
  %t71 = inttoptr i64 %t72 to ptr
  call void @macro_define(ptr %t69, ptr %t70, ptr %t71, i64 0, i64 0)
  %t74 = getelementptr [7 x i8], ptr @.str47, i64 0, i64 0
  %t75 = getelementptr [13 x i8], ptr @.str48, i64 0, i64 0
  %t77 = sext i32 0 to i64
  %t76 = inttoptr i64 %t77 to ptr
  call void @macro_define(ptr %t74, ptr %t75, ptr %t76, i64 0, i64 0)
  %t79 = getelementptr [8 x i8], ptr @.str49, i64 0, i64 0
  %t80 = getelementptr [14 x i8], ptr @.str50, i64 0, i64 0
  %t82 = sext i32 0 to i64
  %t81 = inttoptr i64 %t82 to ptr
  call void @macro_define(ptr %t79, ptr %t80, ptr %t81, i64 0, i64 0)
  %t84 = getelementptr [7 x i8], ptr @.str51, i64 0, i64 0
  %t85 = getelementptr [15 x i8], ptr @.str52, i64 0, i64 0
  %t87 = sext i32 0 to i64
  %t86 = inttoptr i64 %t87 to ptr
  call void @macro_define(ptr %t84, ptr %t85, ptr %t86, i64 0, i64 0)
  %t89 = getelementptr [7 x i8], ptr @.str53, i64 0, i64 0
  %t90 = getelementptr [15 x i8], ptr @.str54, i64 0, i64 0
  %t92 = sext i32 0 to i64
  %t91 = inttoptr i64 %t92 to ptr
  call void @macro_define(ptr %t89, ptr %t90, ptr %t91, i64 0, i64 0)
  %t94 = getelementptr [6 x i8], ptr @.str55, i64 0, i64 0
  %t95 = getelementptr [14 x i8], ptr @.str56, i64 0, i64 0
  %t97 = sext i32 0 to i64
  %t96 = inttoptr i64 %t97 to ptr
  call void @macro_define(ptr %t94, ptr %t95, ptr %t96, i64 0, i64 0)
  br label %L2
L2:
  %t99 = load i64, ptr %t4
  %t100 = load ptr, ptr %t6
  call void @preprocess_into(ptr %t0, ptr %t1, i64 %t99, i64 %t2, ptr %t100, ptr %t8)
  %t102 = load i64, ptr %t4
  %t103 = inttoptr i64 %t102 to ptr
  %t104 = sext i32 0 to i64
  %t105 = getelementptr ptr, ptr %t103, i64 %t104
  %t106 = load ptr, ptr %t105
  ret ptr %t106
L3:
  ret ptr null
}

@.str0 = private unnamed_addr constant [7 x i8] c"malloc\00"
@.str1 = private unnamed_addr constant [8 x i8] c"realloc\00"
@.str2 = private unnamed_addr constant [18 x i8] c"macro table full\0A\00"
@.str3 = private unnamed_addr constant [4 x i8] c"...\00"
@.str4 = private unnamed_addr constant [12 x i8] c"__VA_ARGS__\00"
@.str5 = private unnamed_addr constant [2 x i8] c"r\00"
@.str6 = private unnamed_addr constant [13 x i8] c"/usr/include\00"
@.str7 = private unnamed_addr constant [19 x i8] c"/usr/local/include\00"
@.str8 = private unnamed_addr constant [2 x i8] c".\00"
@.str9 = private unnamed_addr constant [6 x i8] c"%s/%s\00"
@.str10 = private unnamed_addr constant [6 x i8] c"ifdef\00"
@.str11 = private unnamed_addr constant [7 x i8] c"ifndef\00"
@.str12 = private unnamed_addr constant [3 x i8] c"if\00"
@.str13 = private unnamed_addr constant [8 x i8] c"defined\00"
@.str14 = private unnamed_addr constant [5 x i8] c"elif\00"
@.str15 = private unnamed_addr constant [5 x i8] c"else\00"
@.str16 = private unnamed_addr constant [6 x i8] c"endif\00"
@.str17 = private unnamed_addr constant [7 x i8] c"define\00"
@.str18 = private unnamed_addr constant [6 x i8] c"undef\00"
@.str19 = private unnamed_addr constant [8 x i8] c"include\00"
@.str20 = private unnamed_addr constant [36 x i8] c"warning: max include depth reached\0A\00"
@.str21 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str22 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str23 = private unnamed_addr constant [8 x i8] c"__C0C__\00"
@.str24 = private unnamed_addr constant [2 x i8] c"1\00"
@.str25 = private unnamed_addr constant [9 x i8] c"__STDC__\00"
@.str26 = private unnamed_addr constant [2 x i8] c"1\00"
@.str27 = private unnamed_addr constant [5 x i8] c"NULL\00"
@.str28 = private unnamed_addr constant [11 x i8] c"((void*)0)\00"
@.str29 = private unnamed_addr constant [9 x i8] c"__LP64__\00"
@.str30 = private unnamed_addr constant [2 x i8] c"1\00"
@.str31 = private unnamed_addr constant [9 x i8] c"SEEK_SET\00"
@.str32 = private unnamed_addr constant [2 x i8] c"0\00"
@.str33 = private unnamed_addr constant [9 x i8] c"SEEK_CUR\00"
@.str34 = private unnamed_addr constant [2 x i8] c"1\00"
@.str35 = private unnamed_addr constant [9 x i8] c"SEEK_END\00"
@.str36 = private unnamed_addr constant [2 x i8] c"2\00"
@.str37 = private unnamed_addr constant [4 x i8] c"EOF\00"
@.str38 = private unnamed_addr constant [5 x i8] c"(-1)\00"
@.str39 = private unnamed_addr constant [13 x i8] c"EXIT_SUCCESS\00"
@.str40 = private unnamed_addr constant [2 x i8] c"0\00"
@.str41 = private unnamed_addr constant [13 x i8] c"EXIT_FAILURE\00"
@.str42 = private unnamed_addr constant [2 x i8] c"1\00"
@.str43 = private unnamed_addr constant [7 x i8] c"assert\00"
@.str44 = private unnamed_addr constant [10 x i8] c"((void)0)\00"
@.str45 = private unnamed_addr constant [9 x i8] c"va_start\00"
@.str46 = private unnamed_addr constant [15 x i8] c"__c0c_va_start\00"
@.str47 = private unnamed_addr constant [7 x i8] c"va_end\00"
@.str48 = private unnamed_addr constant [13 x i8] c"__c0c_va_end\00"
@.str49 = private unnamed_addr constant [8 x i8] c"va_copy\00"
@.str50 = private unnamed_addr constant [14 x i8] c"__c0c_va_copy\00"
@.str51 = private unnamed_addr constant [7 x i8] c"stderr\00"
@.str52 = private unnamed_addr constant [15 x i8] c"__c0c_stderr()\00"
@.str53 = private unnamed_addr constant [7 x i8] c"stdout\00"
@.str54 = private unnamed_addr constant [15 x i8] c"__c0c_stdout()\00"
@.str55 = private unnamed_addr constant [6 x i8] c"stdin\00"
@.str56 = private unnamed_addr constant [14 x i8] c"__c0c_stdin()\00"
