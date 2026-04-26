; ModuleID = 'parser.c'
source_filename = "parser.c"
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

declare ptr @lexer_new(ptr, ptr)
declare void @lexer_free(ptr)
declare i64 @lexer_next(ptr)
declare i64 @lexer_peek(ptr)
declare void @token_free(ptr)
declare ptr @token_type_name(ptr)
declare ptr @node_new(ptr, i64)
declare void @node_free(ptr)
declare void @node_add_child(ptr, ptr)
declare ptr @type_new(ptr)
declare ptr @type_ptr(ptr)
declare ptr @type_array(ptr, i64)
declare void @type_free(ptr)
declare i32 @type_is_integer(ptr)
declare i32 @type_is_float(ptr)
declare i32 @type_is_pointer(ptr)
declare i32 @type_size(ptr)

define internal void @register_enum_const(ptr %t0, ptr %t1, i64 %t2) {
entry:
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 1024 to i64
  %t5 = icmp sge i64 %t4, %t6
  %t7 = zext i1 %t5 to i64
  %t8 = icmp ne i64 %t7, 0
  br i1 %t8, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t9 = alloca ptr
  %t10 = call ptr @calloc(i64 1, i64 0)
  store ptr %t10, ptr %t9
  %t11 = call ptr @strdup(ptr %t1)
  %t12 = load ptr, ptr %t9
  %t13 = getelementptr i8, ptr %t12, i64 0
  store ptr %t11, ptr %t13
  %t14 = load ptr, ptr %t9
  %t15 = getelementptr i8, ptr %t14, i64 0
  store i64 %t2, ptr %t15
  %t16 = load ptr, ptr %t9
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = load i64, ptr %t19
  %t21 = add i64 %t20, 1
  %t22 = getelementptr i8, ptr %t0, i64 0
  store i64 %t21, ptr %t22
  %t24 = inttoptr i64 %t18 to ptr
  %t23 = getelementptr ptr, ptr %t24, i64 %t20
  store ptr %t16, ptr %t23
  ret void
}

define internal i32 @lookup_enum_const(ptr %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = alloca i64
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  %t5 = load i64, ptr %t3
  %t6 = getelementptr i8, ptr %t0, i64 0
  %t7 = load i64, ptr %t6
  %t9 = sext i32 %t5 to i64
  %t8 = icmp slt i64 %t9, %t7
  %t10 = zext i1 %t8 to i64
  %t11 = icmp ne i64 %t10, 0
  br i1 %t11, label %L1, label %L3
L1:
  %t12 = alloca ptr
  %t13 = getelementptr i8, ptr %t0, i64 0
  %t14 = load i64, ptr %t13
  %t15 = load i64, ptr %t3
  %t16 = inttoptr i64 %t14 to ptr
  %t17 = sext i32 %t15 to i64
  %t18 = getelementptr ptr, ptr %t16, i64 %t17
  %t19 = load ptr, ptr %t18
  store ptr %t19, ptr %t12
  %t20 = load ptr, ptr %t12
  %t21 = ptrtoint ptr %t20 to i64
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L4, label %L5
L4:
  %t23 = load ptr, ptr %t12
  %t24 = getelementptr i8, ptr %t23, i64 0
  %t25 = load i64, ptr %t24
  %t26 = call i32 @strcmp(i64 %t25, ptr %t1)
  %t27 = sext i32 %t26 to i64
  %t29 = sext i32 0 to i64
  %t28 = icmp eq i64 %t27, %t29
  %t30 = zext i1 %t28 to i64
  %t31 = icmp ne i64 %t30, 0
  %t32 = zext i1 %t31 to i64
  br label %L6
L5:
  br label %L6
L6:
  %t33 = phi i64 [ %t32, %L4 ], [ 0, %L5 ]
  %t34 = icmp ne i64 %t33, 0
  br i1 %t34, label %L7, label %L9
L7:
  %t35 = load ptr, ptr %t12
  %t36 = getelementptr i8, ptr %t35, i64 0
  %t37 = load i64, ptr %t36
  store i64 %t37, ptr %t2
  %t38 = sext i32 1 to i64
  %t39 = trunc i64 %t38 to i32
  ret i32 %t39
L10:
  br label %L9
L9:
  br label %L2
L2:
  %t40 = load i64, ptr %t3
  %t42 = sext i32 %t40 to i64
  %t41 = add i64 %t42, 1
  store i64 %t41, ptr %t3
  br label %L0
L3:
  %t43 = sext i32 0 to i64
  %t44 = trunc i64 %t43 to i32
  ret i32 %t44
L11:
  ret i32 0
}

define internal void @register_struct(ptr %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 256 to i64
  %t5 = icmp sge i64 %t4, %t6
  %t7 = zext i1 %t5 to i64
  %t8 = icmp ne i64 %t7, 0
  br i1 %t8, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t9 = alloca ptr
  %t10 = call ptr @calloc(i64 1, i64 0)
  store ptr %t10, ptr %t9
  %t11 = call ptr @strdup(ptr %t1)
  %t12 = load ptr, ptr %t9
  %t13 = getelementptr i8, ptr %t12, i64 0
  store ptr %t11, ptr %t13
  %t14 = load ptr, ptr %t9
  %t15 = getelementptr i8, ptr %t14, i64 0
  store ptr %t2, ptr %t15
  %t16 = load ptr, ptr %t9
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = load i64, ptr %t19
  %t21 = add i64 %t20, 1
  %t22 = getelementptr i8, ptr %t0, i64 0
  store i64 %t21, ptr %t22
  %t24 = inttoptr i64 %t18 to ptr
  %t23 = getelementptr ptr, ptr %t24, i64 %t20
  store ptr %t16, ptr %t23
  ret void
}

define internal ptr @lookup_struct(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 1 to i64
  %t5 = sub i64 %t4, %t6
  store i64 %t5, ptr %t2
  br label %L0
L0:
  %t7 = load i64, ptr %t2
  %t9 = sext i32 %t7 to i64
  %t10 = sext i32 0 to i64
  %t8 = icmp sge i64 %t9, %t10
  %t11 = zext i1 %t8 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L1, label %L3
L1:
  %t13 = alloca ptr
  %t14 = getelementptr i8, ptr %t0, i64 0
  %t15 = load i64, ptr %t14
  %t16 = load i64, ptr %t2
  %t17 = inttoptr i64 %t15 to ptr
  %t18 = sext i32 %t16 to i64
  %t19 = getelementptr ptr, ptr %t17, i64 %t18
  %t20 = load ptr, ptr %t19
  store ptr %t20, ptr %t13
  %t21 = load ptr, ptr %t13
  %t22 = ptrtoint ptr %t21 to i64
  %t23 = icmp ne i64 %t22, 0
  br i1 %t23, label %L4, label %L5
L4:
  %t24 = load ptr, ptr %t13
  %t25 = getelementptr i8, ptr %t24, i64 0
  %t26 = load i64, ptr %t25
  %t27 = call i32 @strcmp(i64 %t26, ptr %t1)
  %t28 = sext i32 %t27 to i64
  %t30 = sext i32 0 to i64
  %t29 = icmp eq i64 %t28, %t30
  %t31 = zext i1 %t29 to i64
  %t32 = icmp ne i64 %t31, 0
  %t33 = zext i1 %t32 to i64
  br label %L6
L5:
  br label %L6
L6:
  %t34 = phi i64 [ %t33, %L4 ], [ 0, %L5 ]
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L7, label %L9
L7:
  %t36 = load ptr, ptr %t13
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  %t39 = inttoptr i64 %t38 to ptr
  ret ptr %t39
L10:
  br label %L9
L9:
  br label %L2
L2:
  %t40 = load i64, ptr %t2
  %t42 = sext i32 %t40 to i64
  %t41 = sub i64 %t42, 1
  store i64 %t41, ptr %t2
  br label %L0
L3:
  %t44 = sext i32 0 to i64
  %t43 = inttoptr i64 %t44 to ptr
  ret ptr %t43
L11:
  ret ptr null
}

define internal void @p_error(ptr %t0, ptr %t1) {
entry:
  %t2 = call ptr @__c0c_stderr()
  %t3 = getelementptr [38 x i8], ptr @.str0, i64 0, i64 0
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = getelementptr i8, ptr %t4, i64 0
  %t6 = load i64, ptr %t5
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = getelementptr i8, ptr %t7, i64 0
  %t9 = load i64, ptr %t8
  %t10 = icmp ne i64 %t9, 0
  br i1 %t10, label %L0, label %L1
L0:
  %t11 = getelementptr i8, ptr %t0, i64 0
  %t12 = getelementptr i8, ptr %t11, i64 0
  %t13 = load i64, ptr %t12
  br label %L2
L1:
  %t14 = getelementptr [2 x i8], ptr @.str1, i64 0, i64 0
  %t15 = ptrtoint ptr %t14 to i64
  br label %L2
L2:
  %t16 = phi i64 [ %t13, %L0 ], [ %t15, %L1 ]
  %t17 = call i32 (ptr, ...) @fprintf(ptr %t2, ptr %t3, i64 %t6, ptr %t1, i64 %t16)
  %t18 = sext i32 %t17 to i64
  call void @exit(i64 1)
  ret void
}

define internal void @advance(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  call void @token_free(i64 %t2)
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t6 = call i64 @lexer_next(i64 %t5)
  %t7 = getelementptr i8, ptr %t0, i64 0
  store i64 %t6, ptr %t7
  ret void
}

define internal i64 @peek(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = call i64 @lexer_peek(i64 %t2)
  ret i64 %t3
L0:
  ret i64 0
}

define internal i32 @check(ptr %t0, ptr %t1) {
entry:
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  %t6 = ptrtoint ptr %t1 to i64
  %t5 = icmp eq i64 %t4, %t6
  %t7 = zext i1 %t5 to i64
  %t8 = trunc i64 %t7 to i32
  ret i32 %t8
L0:
  ret i32 0
}

define internal i32 @match(ptr %t0, ptr %t1) {
entry:
  %t2 = call i32 @check(ptr %t0, ptr %t1)
  %t3 = sext i32 %t2 to i64
  %t4 = icmp ne i64 %t3, 0
  br i1 %t4, label %L0, label %L2
L0:
  call void @advance(ptr %t0)
  %t6 = sext i32 1 to i64
  %t7 = trunc i64 %t6 to i32
  ret i32 %t7
L3:
  br label %L2
L2:
  %t8 = sext i32 0 to i64
  %t9 = trunc i64 %t8 to i32
  ret i32 %t9
L4:
  ret i32 0
}

define internal void @expect(ptr %t0, ptr %t1) {
entry:
  %t2 = call i32 @check(ptr %t0, ptr %t1)
  %t3 = sext i32 %t2 to i64
  %t5 = icmp eq i64 %t3, 0
  %t4 = zext i1 %t5 to i64
  %t6 = icmp ne i64 %t4, 0
  br i1 %t6, label %L0, label %L2
L0:
  %t7 = alloca ptr
  %t8 = load ptr, ptr %t7
  %t9 = getelementptr [12 x i8], ptr @.str2, i64 0, i64 0
  %t10 = call ptr @token_type_name(ptr %t1)
  %t11 = call i32 (ptr, ...) @snprintf(ptr %t8, i64 8, ptr %t9, ptr %t10)
  %t12 = sext i32 %t11 to i64
  %t13 = load ptr, ptr %t7
  call void @p_error(ptr %t0, ptr %t13)
  br label %L2
L2:
  call void @advance(ptr %t0)
  ret void
}

define internal ptr @expect_ident(ptr %t0) {
entry:
  %t1 = call i32 @check(ptr %t0, i64 4)
  %t2 = sext i32 %t1 to i64
  %t4 = icmp eq i64 %t2, 0
  %t3 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t3, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = getelementptr [20 x i8], ptr @.str3, i64 0, i64 0
  call void @p_error(ptr %t0, ptr %t6)
  br label %L2
L2:
  %t8 = alloca ptr
  %t9 = getelementptr i8, ptr %t0, i64 0
  %t10 = getelementptr i8, ptr %t9, i64 0
  %t11 = load i64, ptr %t10
  %t12 = call ptr @strdup(i64 %t11)
  store ptr %t12, ptr %t8
  call void @advance(ptr %t0)
  %t14 = load ptr, ptr %t8
  ret ptr %t14
L3:
  ret ptr null
}

define internal void @register_typedef(ptr %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 512 to i64
  %t5 = icmp sge i64 %t4, %t6
  %t7 = zext i1 %t5 to i64
  %t8 = icmp ne i64 %t7, 0
  br i1 %t8, label %L0, label %L2
L0:
  %t9 = getelementptr [18 x i8], ptr @.str4, i64 0, i64 0
  call void @p_error(ptr %t0, ptr %t9)
  br label %L2
L2:
  %t11 = alloca ptr
  %t12 = call ptr @calloc(i64 1, i64 0)
  store ptr %t12, ptr %t11
  %t13 = call ptr @strdup(ptr %t1)
  %t14 = load ptr, ptr %t11
  %t15 = getelementptr i8, ptr %t14, i64 0
  store ptr %t13, ptr %t15
  %t16 = load ptr, ptr %t11
  %t17 = getelementptr i8, ptr %t16, i64 0
  store ptr %t2, ptr %t17
  %t18 = load ptr, ptr %t11
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = load i64, ptr %t19
  %t21 = getelementptr i8, ptr %t0, i64 0
  %t22 = load i64, ptr %t21
  %t23 = add i64 %t22, 1
  %t24 = getelementptr i8, ptr %t0, i64 0
  store i64 %t23, ptr %t24
  %t26 = inttoptr i64 %t20 to ptr
  %t25 = getelementptr ptr, ptr %t26, i64 %t22
  store ptr %t18, ptr %t25
  ret void
}

define internal ptr @lookup_typedef(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 1 to i64
  %t5 = sub i64 %t4, %t6
  store i64 %t5, ptr %t2
  br label %L0
L0:
  %t7 = load i64, ptr %t2
  %t9 = sext i32 %t7 to i64
  %t10 = sext i32 0 to i64
  %t8 = icmp sge i64 %t9, %t10
  %t11 = zext i1 %t8 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L1, label %L3
L1:
  %t13 = alloca ptr
  %t14 = getelementptr i8, ptr %t0, i64 0
  %t15 = load i64, ptr %t14
  %t16 = load i64, ptr %t2
  %t17 = inttoptr i64 %t15 to ptr
  %t18 = sext i32 %t16 to i64
  %t19 = getelementptr ptr, ptr %t17, i64 %t18
  %t20 = load ptr, ptr %t19
  store ptr %t20, ptr %t13
  %t21 = load ptr, ptr %t13
  %t22 = ptrtoint ptr %t21 to i64
  %t23 = icmp ne i64 %t22, 0
  br i1 %t23, label %L4, label %L5
L4:
  %t24 = load ptr, ptr %t13
  %t25 = getelementptr i8, ptr %t24, i64 0
  %t26 = load i64, ptr %t25
  %t27 = call i32 @strcmp(i64 %t26, ptr %t1)
  %t28 = sext i32 %t27 to i64
  %t30 = sext i32 0 to i64
  %t29 = icmp eq i64 %t28, %t30
  %t31 = zext i1 %t29 to i64
  %t32 = icmp ne i64 %t31, 0
  %t33 = zext i1 %t32 to i64
  br label %L6
L5:
  br label %L6
L6:
  %t34 = phi i64 [ %t33, %L4 ], [ 0, %L5 ]
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L7, label %L9
L7:
  %t36 = load ptr, ptr %t13
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  %t39 = inttoptr i64 %t38 to ptr
  ret ptr %t39
L10:
  br label %L9
L9:
  br label %L2
L2:
  %t40 = load i64, ptr %t2
  %t42 = sext i32 %t40 to i64
  %t41 = sub i64 %t42, 1
  store i64 %t41, ptr %t2
  br label %L0
L3:
  %t44 = sext i32 0 to i64
  %t43 = inttoptr i64 %t44 to ptr
  ret ptr %t43
L11:
  ret ptr null
}

define internal i32 @is_gcc_extension(ptr %t0) {
entry:
  %t1 = getelementptr [14 x i8], ptr @.str5, i64 0, i64 0
  %t2 = call i32 @strcmp(ptr %t0, ptr %t1)
  %t3 = sext i32 %t2 to i64
  %t5 = sext i32 0 to i64
  %t4 = icmp eq i64 %t3, %t5
  %t6 = zext i1 %t4 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L1
L0:
  br label %L2
L1:
  %t8 = getelementptr [14 x i8], ptr @.str6, i64 0, i64 0
  %t9 = call i32 @strcmp(ptr %t0, ptr %t8)
  %t10 = sext i32 %t9 to i64
  %t12 = sext i32 0 to i64
  %t11 = icmp eq i64 %t10, %t12
  %t13 = zext i1 %t11 to i64
  %t14 = icmp ne i64 %t13, 0
  %t15 = zext i1 %t14 to i64
  br label %L2
L2:
  %t16 = phi i64 [ 1, %L0 ], [ %t15, %L1 ]
  %t17 = icmp ne i64 %t16, 0
  br i1 %t17, label %L3, label %L4
L3:
  br label %L5
L4:
  %t18 = getelementptr [8 x i8], ptr @.str7, i64 0, i64 0
  %t19 = call i32 @strcmp(ptr %t0, ptr %t18)
  %t20 = sext i32 %t19 to i64
  %t22 = sext i32 0 to i64
  %t21 = icmp eq i64 %t20, %t22
  %t23 = zext i1 %t21 to i64
  %t24 = icmp ne i64 %t23, 0
  %t25 = zext i1 %t24 to i64
  br label %L5
L5:
  %t26 = phi i64 [ 1, %L3 ], [ %t25, %L4 ]
  %t27 = icmp ne i64 %t26, 0
  br i1 %t27, label %L6, label %L7
L6:
  br label %L8
L7:
  %t28 = getelementptr [6 x i8], ptr @.str8, i64 0, i64 0
  %t29 = call i32 @strcmp(ptr %t0, ptr %t28)
  %t30 = sext i32 %t29 to i64
  %t32 = sext i32 0 to i64
  %t31 = icmp eq i64 %t30, %t32
  %t33 = zext i1 %t31 to i64
  %t34 = icmp ne i64 %t33, 0
  %t35 = zext i1 %t34 to i64
  br label %L8
L8:
  %t36 = phi i64 [ 1, %L6 ], [ %t35, %L7 ]
  %t37 = icmp ne i64 %t36, 0
  br i1 %t37, label %L9, label %L10
L9:
  br label %L11
L10:
  %t38 = getelementptr [11 x i8], ptr @.str9, i64 0, i64 0
  %t39 = call i32 @strcmp(ptr %t0, ptr %t38)
  %t40 = sext i32 %t39 to i64
  %t42 = sext i32 0 to i64
  %t41 = icmp eq i64 %t40, %t42
  %t43 = zext i1 %t41 to i64
  %t44 = icmp ne i64 %t43, 0
  %t45 = zext i1 %t44 to i64
  br label %L11
L11:
  %t46 = phi i64 [ 1, %L9 ], [ %t45, %L10 ]
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L12, label %L13
L12:
  br label %L14
L13:
  %t48 = getelementptr [9 x i8], ptr @.str10, i64 0, i64 0
  %t49 = call i32 @strcmp(ptr %t0, ptr %t48)
  %t50 = sext i32 %t49 to i64
  %t52 = sext i32 0 to i64
  %t51 = icmp eq i64 %t50, %t52
  %t53 = zext i1 %t51 to i64
  %t54 = icmp ne i64 %t53, 0
  %t55 = zext i1 %t54 to i64
  br label %L14
L14:
  %t56 = phi i64 [ 1, %L12 ], [ %t55, %L13 ]
  %t57 = icmp ne i64 %t56, 0
  br i1 %t57, label %L15, label %L16
L15:
  br label %L17
L16:
  %t58 = getelementptr [13 x i8], ptr @.str11, i64 0, i64 0
  %t59 = call i32 @strcmp(ptr %t0, ptr %t58)
  %t60 = sext i32 %t59 to i64
  %t62 = sext i32 0 to i64
  %t61 = icmp eq i64 %t60, %t62
  %t63 = zext i1 %t61 to i64
  %t64 = icmp ne i64 %t63, 0
  %t65 = zext i1 %t64 to i64
  br label %L17
L17:
  %t66 = phi i64 [ 1, %L15 ], [ %t65, %L16 ]
  %t67 = icmp ne i64 %t66, 0
  br i1 %t67, label %L18, label %L19
L18:
  br label %L20
L19:
  %t68 = getelementptr [11 x i8], ptr @.str12, i64 0, i64 0
  %t69 = call i32 @strcmp(ptr %t0, ptr %t68)
  %t70 = sext i32 %t69 to i64
  %t72 = sext i32 0 to i64
  %t71 = icmp eq i64 %t70, %t72
  %t73 = zext i1 %t71 to i64
  %t74 = icmp ne i64 %t73, 0
  %t75 = zext i1 %t74 to i64
  br label %L20
L20:
  %t76 = phi i64 [ 1, %L18 ], [ %t75, %L19 ]
  %t77 = icmp ne i64 %t76, 0
  br i1 %t77, label %L21, label %L22
L21:
  br label %L23
L22:
  %t78 = getelementptr [11 x i8], ptr @.str13, i64 0, i64 0
  %t79 = call i32 @strcmp(ptr %t0, ptr %t78)
  %t80 = sext i32 %t79 to i64
  %t82 = sext i32 0 to i64
  %t81 = icmp eq i64 %t80, %t82
  %t83 = zext i1 %t81 to i64
  %t84 = icmp ne i64 %t83, 0
  %t85 = zext i1 %t84 to i64
  br label %L23
L23:
  %t86 = phi i64 [ 1, %L21 ], [ %t85, %L22 ]
  %t87 = icmp ne i64 %t86, 0
  br i1 %t87, label %L24, label %L25
L24:
  br label %L26
L25:
  %t88 = getelementptr [13 x i8], ptr @.str14, i64 0, i64 0
  %t89 = call i32 @strcmp(ptr %t0, ptr %t88)
  %t90 = sext i32 %t89 to i64
  %t92 = sext i32 0 to i64
  %t91 = icmp eq i64 %t90, %t92
  %t93 = zext i1 %t91 to i64
  %t94 = icmp ne i64 %t93, 0
  %t95 = zext i1 %t94 to i64
  br label %L26
L26:
  %t96 = phi i64 [ 1, %L24 ], [ %t95, %L25 ]
  %t97 = icmp ne i64 %t96, 0
  br i1 %t97, label %L27, label %L28
L27:
  br label %L29
L28:
  %t98 = getelementptr [8 x i8], ptr @.str15, i64 0, i64 0
  %t99 = call i32 @strcmp(ptr %t0, ptr %t98)
  %t100 = sext i32 %t99 to i64
  %t102 = sext i32 0 to i64
  %t101 = icmp eq i64 %t100, %t102
  %t103 = zext i1 %t101 to i64
  %t104 = icmp ne i64 %t103, 0
  %t105 = zext i1 %t104 to i64
  br label %L29
L29:
  %t106 = phi i64 [ 1, %L27 ], [ %t105, %L28 ]
  %t107 = icmp ne i64 %t106, 0
  br i1 %t107, label %L30, label %L31
L30:
  br label %L32
L31:
  %t108 = getelementptr [10 x i8], ptr @.str16, i64 0, i64 0
  %t109 = call i32 @strcmp(ptr %t0, ptr %t108)
  %t110 = sext i32 %t109 to i64
  %t112 = sext i32 0 to i64
  %t111 = icmp eq i64 %t110, %t112
  %t113 = zext i1 %t111 to i64
  %t114 = icmp ne i64 %t113, 0
  %t115 = zext i1 %t114 to i64
  br label %L32
L32:
  %t116 = phi i64 [ 1, %L30 ], [ %t115, %L31 ]
  %t117 = icmp ne i64 %t116, 0
  br i1 %t117, label %L33, label %L34
L33:
  br label %L35
L34:
  %t118 = getelementptr [11 x i8], ptr @.str17, i64 0, i64 0
  %t119 = call i32 @strcmp(ptr %t0, ptr %t118)
  %t120 = sext i32 %t119 to i64
  %t122 = sext i32 0 to i64
  %t121 = icmp eq i64 %t120, %t122
  %t123 = zext i1 %t121 to i64
  %t124 = icmp ne i64 %t123, 0
  %t125 = zext i1 %t124 to i64
  br label %L35
L35:
  %t126 = phi i64 [ 1, %L33 ], [ %t125, %L34 ]
  %t127 = icmp ne i64 %t126, 0
  br i1 %t127, label %L36, label %L37
L36:
  br label %L38
L37:
  %t128 = getelementptr [9 x i8], ptr @.str18, i64 0, i64 0
  %t129 = call i32 @strcmp(ptr %t0, ptr %t128)
  %t130 = sext i32 %t129 to i64
  %t132 = sext i32 0 to i64
  %t131 = icmp eq i64 %t130, %t132
  %t133 = zext i1 %t131 to i64
  %t134 = icmp ne i64 %t133, 0
  %t135 = zext i1 %t134 to i64
  br label %L38
L38:
  %t136 = phi i64 [ 1, %L36 ], [ %t135, %L37 ]
  %t137 = icmp ne i64 %t136, 0
  br i1 %t137, label %L39, label %L40
L39:
  br label %L41
L40:
  %t138 = getelementptr [11 x i8], ptr @.str19, i64 0, i64 0
  %t139 = call i32 @strcmp(ptr %t0, ptr %t138)
  %t140 = sext i32 %t139 to i64
  %t142 = sext i32 0 to i64
  %t141 = icmp eq i64 %t140, %t142
  %t143 = zext i1 %t141 to i64
  %t144 = icmp ne i64 %t143, 0
  %t145 = zext i1 %t144 to i64
  br label %L41
L41:
  %t146 = phi i64 [ 1, %L39 ], [ %t145, %L40 ]
  %t147 = icmp ne i64 %t146, 0
  br i1 %t147, label %L42, label %L43
L42:
  br label %L44
L43:
  %t148 = getelementptr [9 x i8], ptr @.str20, i64 0, i64 0
  %t149 = call i32 @strcmp(ptr %t0, ptr %t148)
  %t150 = sext i32 %t149 to i64
  %t152 = sext i32 0 to i64
  %t151 = icmp eq i64 %t150, %t152
  %t153 = zext i1 %t151 to i64
  %t154 = icmp ne i64 %t153, 0
  %t155 = zext i1 %t154 to i64
  br label %L44
L44:
  %t156 = phi i64 [ 1, %L42 ], [ %t155, %L43 ]
  %t157 = icmp ne i64 %t156, 0
  br i1 %t157, label %L45, label %L46
L45:
  br label %L47
L46:
  %t158 = getelementptr [8 x i8], ptr @.str21, i64 0, i64 0
  %t159 = call i32 @strcmp(ptr %t0, ptr %t158)
  %t160 = sext i32 %t159 to i64
  %t162 = sext i32 0 to i64
  %t161 = icmp eq i64 %t160, %t162
  %t163 = zext i1 %t161 to i64
  %t164 = icmp ne i64 %t163, 0
  %t165 = zext i1 %t164 to i64
  br label %L47
L47:
  %t166 = phi i64 [ 1, %L45 ], [ %t165, %L46 ]
  %t167 = icmp ne i64 %t166, 0
  br i1 %t167, label %L48, label %L49
L48:
  br label %L50
L49:
  %t168 = getelementptr [11 x i8], ptr @.str22, i64 0, i64 0
  %t169 = call i32 @strcmp(ptr %t0, ptr %t168)
  %t170 = sext i32 %t169 to i64
  %t172 = sext i32 0 to i64
  %t171 = icmp eq i64 %t170, %t172
  %t173 = zext i1 %t171 to i64
  %t174 = icmp ne i64 %t173, 0
  %t175 = zext i1 %t174 to i64
  br label %L50
L50:
  %t176 = phi i64 [ 1, %L48 ], [ %t175, %L49 ]
  %t177 = icmp ne i64 %t176, 0
  br i1 %t177, label %L51, label %L52
L51:
  br label %L53
L52:
  %t178 = getelementptr [14 x i8], ptr @.str23, i64 0, i64 0
  %t179 = call i32 @strcmp(ptr %t0, ptr %t178)
  %t180 = sext i32 %t179 to i64
  %t182 = sext i32 0 to i64
  %t181 = icmp eq i64 %t180, %t182
  %t183 = zext i1 %t181 to i64
  %t184 = icmp ne i64 %t183, 0
  %t185 = zext i1 %t184 to i64
  br label %L53
L53:
  %t186 = phi i64 [ 1, %L51 ], [ %t185, %L52 ]
  %t187 = icmp ne i64 %t186, 0
  br i1 %t187, label %L54, label %L55
L54:
  br label %L56
L55:
  %t188 = getelementptr [10 x i8], ptr @.str24, i64 0, i64 0
  %t189 = call i32 @strcmp(ptr %t0, ptr %t188)
  %t190 = sext i32 %t189 to i64
  %t192 = sext i32 0 to i64
  %t191 = icmp eq i64 %t190, %t192
  %t193 = zext i1 %t191 to i64
  %t194 = icmp ne i64 %t193, 0
  %t195 = zext i1 %t194 to i64
  br label %L56
L56:
  %t196 = phi i64 [ 1, %L54 ], [ %t195, %L55 ]
  %t197 = trunc i64 %t196 to i32
  ret i32 %t197
L57:
  ret i32 0
}

define internal void @skip_gcc_extension(ptr %t0) {
entry:
  br label %L0
L0:
  br label %L1
L1:
  %t1 = call i32 @check(ptr %t0, i64 4)
  %t2 = sext i32 %t1 to i64
  %t4 = icmp eq i64 %t2, 0
  %t3 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t3, 0
  br i1 %t5, label %L4, label %L6
L4:
  br label %L3
L7:
  br label %L6
L6:
  %t6 = getelementptr i8, ptr %t0, i64 0
  %t7 = getelementptr i8, ptr %t6, i64 0
  %t8 = load i64, ptr %t7
  %t9 = call i32 @is_gcc_extension(i64 %t8)
  %t10 = sext i32 %t9 to i64
  %t12 = icmp eq i64 %t10, 0
  %t11 = zext i1 %t12 to i64
  %t13 = icmp ne i64 %t11, 0
  br i1 %t13, label %L8, label %L10
L8:
  br label %L3
L11:
  br label %L10
L10:
  %t14 = alloca ptr
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = getelementptr i8, ptr %t15, i64 0
  %t17 = load i64, ptr %t16
  store i64 %t17, ptr %t14
  %t18 = alloca i64
  %t19 = load ptr, ptr %t14
  %t20 = getelementptr [14 x i8], ptr @.str25, i64 0, i64 0
  %t21 = call i32 @strcmp(ptr %t19, ptr %t20)
  %t22 = sext i32 %t21 to i64
  %t24 = sext i32 0 to i64
  %t23 = icmp eq i64 %t22, %t24
  %t25 = zext i1 %t23 to i64
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L12, label %L13
L12:
  br label %L14
L13:
  %t27 = load ptr, ptr %t14
  %t28 = getelementptr [8 x i8], ptr @.str26, i64 0, i64 0
  %t29 = call i32 @strcmp(ptr %t27, ptr %t28)
  %t30 = sext i32 %t29 to i64
  %t32 = sext i32 0 to i64
  %t31 = icmp eq i64 %t30, %t32
  %t33 = zext i1 %t31 to i64
  %t34 = icmp ne i64 %t33, 0
  %t35 = zext i1 %t34 to i64
  br label %L14
L14:
  %t36 = phi i64 [ 1, %L12 ], [ %t35, %L13 ]
  %t37 = icmp ne i64 %t36, 0
  br i1 %t37, label %L15, label %L16
L15:
  br label %L17
L16:
  %t38 = load ptr, ptr %t14
  %t39 = getelementptr [6 x i8], ptr @.str27, i64 0, i64 0
  %t40 = call i32 @strcmp(ptr %t38, ptr %t39)
  %t41 = sext i32 %t40 to i64
  %t43 = sext i32 0 to i64
  %t42 = icmp eq i64 %t41, %t43
  %t44 = zext i1 %t42 to i64
  %t45 = icmp ne i64 %t44, 0
  %t46 = zext i1 %t45 to i64
  br label %L17
L17:
  %t47 = phi i64 [ 1, %L15 ], [ %t46, %L16 ]
  %t48 = icmp ne i64 %t47, 0
  br i1 %t48, label %L18, label %L19
L18:
  br label %L20
L19:
  %t49 = load ptr, ptr %t14
  %t50 = getelementptr [11 x i8], ptr @.str28, i64 0, i64 0
  %t51 = call i32 @strcmp(ptr %t49, ptr %t50)
  %t52 = sext i32 %t51 to i64
  %t54 = sext i32 0 to i64
  %t53 = icmp eq i64 %t52, %t54
  %t55 = zext i1 %t53 to i64
  %t56 = icmp ne i64 %t55, 0
  %t57 = zext i1 %t56 to i64
  br label %L20
L20:
  %t58 = phi i64 [ 1, %L18 ], [ %t57, %L19 ]
  %t59 = icmp ne i64 %t58, 0
  br i1 %t59, label %L21, label %L22
L21:
  br label %L23
L22:
  %t60 = load ptr, ptr %t14
  %t61 = getelementptr [9 x i8], ptr @.str29, i64 0, i64 0
  %t62 = call i32 @strcmp(ptr %t60, ptr %t61)
  %t63 = sext i32 %t62 to i64
  %t65 = sext i32 0 to i64
  %t64 = icmp eq i64 %t63, %t65
  %t66 = zext i1 %t64 to i64
  %t67 = icmp ne i64 %t66, 0
  %t68 = zext i1 %t67 to i64
  br label %L23
L23:
  %t69 = phi i64 [ 1, %L21 ], [ %t68, %L22 ]
  %t70 = icmp ne i64 %t69, 0
  br i1 %t70, label %L24, label %L25
L24:
  br label %L26
L25:
  %t71 = load ptr, ptr %t14
  %t72 = getelementptr [11 x i8], ptr @.str30, i64 0, i64 0
  %t73 = call i32 @strcmp(ptr %t71, ptr %t72)
  %t74 = sext i32 %t73 to i64
  %t76 = sext i32 0 to i64
  %t75 = icmp eq i64 %t74, %t76
  %t77 = zext i1 %t75 to i64
  %t78 = icmp ne i64 %t77, 0
  %t79 = zext i1 %t78 to i64
  br label %L26
L26:
  %t80 = phi i64 [ 1, %L24 ], [ %t79, %L25 ]
  store i64 %t80, ptr %t18
  call void @advance(ptr %t0)
  %t82 = load i64, ptr %t18
  %t83 = sext i32 %t82 to i64
  %t84 = icmp ne i64 %t83, 0
  br i1 %t84, label %L27, label %L28
L27:
  %t85 = call i32 @check(ptr %t0, i64 72)
  %t86 = sext i32 %t85 to i64
  %t87 = icmp ne i64 %t86, 0
  %t88 = zext i1 %t87 to i64
  br label %L29
L28:
  br label %L29
L29:
  %t89 = phi i64 [ %t88, %L27 ], [ 0, %L28 ]
  %t90 = icmp ne i64 %t89, 0
  br i1 %t90, label %L30, label %L32
L30:
  %t91 = alloca i64
  %t92 = sext i32 1 to i64
  store i64 %t92, ptr %t91
  call void @advance(ptr %t0)
  br label %L33
L33:
  %t94 = call i32 @check(ptr %t0, i64 81)
  %t95 = sext i32 %t94 to i64
  %t97 = icmp eq i64 %t95, 0
  %t96 = zext i1 %t97 to i64
  %t98 = icmp ne i64 %t96, 0
  br i1 %t98, label %L36, label %L37
L36:
  %t99 = load i64, ptr %t91
  %t101 = sext i32 %t99 to i64
  %t102 = sext i32 0 to i64
  %t100 = icmp sgt i64 %t101, %t102
  %t103 = zext i1 %t100 to i64
  %t104 = icmp ne i64 %t103, 0
  %t105 = zext i1 %t104 to i64
  br label %L38
L37:
  br label %L38
L38:
  %t106 = phi i64 [ %t105, %L36 ], [ 0, %L37 ]
  %t107 = icmp ne i64 %t106, 0
  br i1 %t107, label %L34, label %L35
L34:
  %t108 = call i32 @check(ptr %t0, i64 72)
  %t109 = sext i32 %t108 to i64
  %t110 = icmp ne i64 %t109, 0
  br i1 %t110, label %L39, label %L40
L39:
  %t111 = load i64, ptr %t91
  %t113 = sext i32 %t111 to i64
  %t112 = add i64 %t113, 1
  store i64 %t112, ptr %t91
  br label %L41
L40:
  %t114 = call i32 @check(ptr %t0, i64 73)
  %t115 = sext i32 %t114 to i64
  %t116 = icmp ne i64 %t115, 0
  br i1 %t116, label %L42, label %L44
L42:
  %t117 = load i64, ptr %t91
  %t119 = sext i32 %t117 to i64
  %t118 = sub i64 %t119, 1
  store i64 %t118, ptr %t91
  br label %L44
L44:
  br label %L41
L41:
  call void @advance(ptr %t0)
  br label %L33
L35:
  br label %L32
L32:
  br label %L2
L2:
  br label %L0
L3:
  ret void
}

define internal i32 @is_type_start(ptr %t0) {
entry:
  %t1 = call i32 @check(ptr %t0, i64 4)
  %t2 = sext i32 %t1 to i64
  %t3 = icmp ne i64 %t2, 0
  br i1 %t3, label %L0, label %L1
L0:
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = getelementptr i8, ptr %t4, i64 0
  %t6 = load i64, ptr %t5
  %t7 = call i32 @is_gcc_extension(i64 %t6)
  %t8 = sext i32 %t7 to i64
  %t9 = icmp ne i64 %t8, 0
  %t10 = zext i1 %t9 to i64
  br label %L2
L1:
  br label %L2
L2:
  %t11 = phi i64 [ %t10, %L0 ], [ 0, %L1 ]
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L3, label %L5
L3:
  %t13 = sext i32 0 to i64
  %t14 = trunc i64 %t13 to i32
  ret i32 %t14
L6:
  br label %L5
L5:
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = getelementptr i8, ptr %t15, i64 0
  %t17 = load i64, ptr %t16
  %t18 = add i64 %t17, 0
  switch i64 %t18, label %L26 [
    i64 5, label %L8
    i64 6, label %L9
    i64 7, label %L10
    i64 8, label %L11
    i64 9, label %L12
    i64 10, label %L13
    i64 11, label %L14
    i64 12, label %L15
    i64 13, label %L16
    i64 26, label %L17
    i64 27, label %L18
    i64 28, label %L19
    i64 32, label %L20
    i64 33, label %L21
    i64 30, label %L22
    i64 31, label %L23
    i64 29, label %L24
    i64 4, label %L25
  ]
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
  br label %L14
L14:
  br label %L15
L15:
  br label %L16
L16:
  br label %L17
L17:
  br label %L18
L18:
  br label %L19
L19:
  br label %L20
L20:
  br label %L21
L21:
  br label %L22
L22:
  br label %L23
L23:
  br label %L24
L24:
  %t19 = sext i32 1 to i64
  %t20 = trunc i64 %t19 to i32
  ret i32 %t20
L27:
  br label %L25
L25:
  %t21 = getelementptr i8, ptr %t0, i64 0
  %t22 = getelementptr i8, ptr %t21, i64 0
  %t23 = load i64, ptr %t22
  %t24 = call ptr @lookup_typedef(ptr %t0, i64 %t23)
  %t26 = sext i32 0 to i64
  %t25 = inttoptr i64 %t26 to ptr
  %t28 = ptrtoint ptr %t24 to i64
  %t29 = ptrtoint ptr %t25 to i64
  %t27 = icmp ne i64 %t28, %t29
  %t30 = zext i1 %t27 to i64
  %t31 = trunc i64 %t30 to i32
  ret i32 %t31
L28:
  br label %L7
L26:
  %t32 = sext i32 0 to i64
  %t33 = trunc i64 %t32 to i32
  ret i32 %t33
L29:
  br label %L7
L7:
  ret i32 0
}

define internal ptr @parse_struct_union(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 27 to i64
  %t5 = icmp eq i64 %t4, %t6
  %t7 = zext i1 %t5 to i64
  store i64 %t7, ptr %t1
  call void @advance(ptr %t0)
  %t9 = alloca ptr
  %t11 = sext i32 0 to i64
  %t10 = inttoptr i64 %t11 to ptr
  store ptr %t10, ptr %t9
  %t12 = call i32 @check(ptr %t0, i64 4)
  %t13 = sext i32 %t12 to i64
  %t14 = icmp ne i64 %t13, 0
  br i1 %t14, label %L0, label %L2
L0:
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = getelementptr i8, ptr %t15, i64 0
  %t17 = load i64, ptr %t16
  %t18 = call ptr @strdup(i64 %t17)
  store ptr %t18, ptr %t9
  call void @advance(ptr %t0)
  br label %L2
L2:
  %t20 = alloca ptr
  %t21 = load i64, ptr %t1
  %t23 = sext i32 %t21 to i64
  %t22 = icmp ne i64 %t23, 0
  br i1 %t22, label %L3, label %L4
L3:
  %t24 = sext i32 19 to i64
  br label %L5
L4:
  %t25 = sext i32 18 to i64
  br label %L5
L5:
  %t26 = phi i64 [ %t24, %L3 ], [ %t25, %L4 ]
  %t27 = call ptr @type_new(i64 %t26)
  store ptr %t27, ptr %t20
  %t28 = load ptr, ptr %t9
  %t29 = load ptr, ptr %t20
  %t30 = getelementptr i8, ptr %t29, i64 0
  store ptr %t28, ptr %t30
  %t31 = call i32 @check(ptr %t0, i64 74)
  %t32 = sext i32 %t31 to i64
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L6, label %L8
L6:
  call void @advance(ptr %t0)
  br label %L9
L9:
  %t35 = call i32 @check(ptr %t0, i64 75)
  %t36 = sext i32 %t35 to i64
  %t38 = icmp eq i64 %t36, 0
  %t37 = zext i1 %t38 to i64
  %t39 = icmp ne i64 %t37, 0
  br i1 %t39, label %L12, label %L13
L12:
  %t40 = call i32 @check(ptr %t0, i64 81)
  %t41 = sext i32 %t40 to i64
  %t43 = icmp eq i64 %t41, 0
  %t42 = zext i1 %t43 to i64
  %t44 = icmp ne i64 %t42, 0
  %t45 = zext i1 %t44 to i64
  br label %L14
L13:
  br label %L14
L14:
  %t46 = phi i64 [ %t45, %L12 ], [ 0, %L13 ]
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L10, label %L11
L10:
  %t48 = alloca i64
  %t49 = sext i32 0 to i64
  store i64 %t49, ptr %t48
  %t50 = alloca i64
  %t51 = sext i32 0 to i64
  store i64 %t51, ptr %t50
  %t52 = alloca i64
  %t53 = sext i32 0 to i64
  store i64 %t53, ptr %t52
  %t54 = alloca ptr
  %t55 = call ptr @parse_type_specifier(ptr %t0, ptr %t48, ptr %t50, ptr %t52)
  store ptr %t55, ptr %t54
  %t56 = load ptr, ptr %t54
  %t58 = ptrtoint ptr %t56 to i64
  %t59 = icmp eq i64 %t58, 0
  %t57 = zext i1 %t59 to i64
  %t60 = icmp ne i64 %t57, 0
  br i1 %t60, label %L15, label %L17
L15:
  call void @advance(ptr %t0)
  br label %L9
L18:
  br label %L17
L17:
  %t62 = alloca ptr
  %t64 = sext i32 0 to i64
  %t63 = inttoptr i64 %t64 to ptr
  store ptr %t63, ptr %t62
  %t65 = load ptr, ptr %t54
  %t66 = call ptr @parse_declarator(ptr %t0, ptr %t65, ptr %t62)
  store ptr %t66, ptr %t54
  %t67 = load ptr, ptr %t62
  %t68 = icmp ne ptr %t67, null
  br i1 %t68, label %L19, label %L21
L19:
  %t69 = load ptr, ptr %t20
  %t70 = getelementptr i8, ptr %t69, i64 0
  %t71 = load i64, ptr %t70
  %t72 = load ptr, ptr %t20
  %t73 = getelementptr i8, ptr %t72, i64 0
  %t74 = load i64, ptr %t73
  %t76 = sext i32 1 to i64
  %t75 = add i64 %t74, %t76
  %t78 = sext i32 8 to i64
  %t77 = mul i64 %t75, %t78
  %t79 = call ptr @realloc(i64 %t71, i64 %t77)
  %t80 = load ptr, ptr %t20
  %t81 = getelementptr i8, ptr %t80, i64 0
  store ptr %t79, ptr %t81
  %t82 = load ptr, ptr %t20
  %t83 = getelementptr i8, ptr %t82, i64 0
  %t84 = load i64, ptr %t83
  %t85 = load ptr, ptr %t20
  %t86 = getelementptr i8, ptr %t85, i64 0
  %t87 = load i64, ptr %t86
  %t89 = sext i32 1 to i64
  %t88 = add i64 %t87, %t89
  %t91 = sext i32 0 to i64
  %t90 = mul i64 %t88, %t91
  %t92 = call ptr @realloc(i64 %t84, i64 %t90)
  %t93 = load ptr, ptr %t20
  %t94 = getelementptr i8, ptr %t93, i64 0
  store ptr %t92, ptr %t94
  %t95 = load ptr, ptr %t54
  %t96 = load ptr, ptr %t20
  %t97 = getelementptr i8, ptr %t96, i64 0
  %t98 = load i64, ptr %t97
  %t99 = load ptr, ptr %t20
  %t100 = getelementptr i8, ptr %t99, i64 0
  %t101 = load i64, ptr %t100
  %t103 = inttoptr i64 %t98 to ptr
  %t102 = getelementptr ptr, ptr %t103, i64 %t101
  store ptr %t95, ptr %t102
  %t104 = load ptr, ptr %t62
  %t105 = load ptr, ptr %t20
  %t106 = getelementptr i8, ptr %t105, i64 0
  %t107 = load i64, ptr %t106
  %t108 = load ptr, ptr %t20
  %t109 = getelementptr i8, ptr %t108, i64 0
  %t110 = load i64, ptr %t109
  %t112 = inttoptr i64 %t107 to ptr
  %t111 = getelementptr ptr, ptr %t112, i64 %t110
  %t113 = getelementptr i8, ptr %t111, i64 0
  store ptr %t104, ptr %t113
  %t114 = load ptr, ptr %t54
  %t115 = load ptr, ptr %t20
  %t116 = getelementptr i8, ptr %t115, i64 0
  %t117 = load i64, ptr %t116
  %t118 = load ptr, ptr %t20
  %t119 = getelementptr i8, ptr %t118, i64 0
  %t120 = load i64, ptr %t119
  %t122 = inttoptr i64 %t117 to ptr
  %t121 = getelementptr ptr, ptr %t122, i64 %t120
  %t123 = getelementptr i8, ptr %t121, i64 0
  store ptr %t114, ptr %t123
  %t124 = load ptr, ptr %t20
  %t125 = getelementptr i8, ptr %t124, i64 0
  %t126 = load i64, ptr %t125
  %t127 = add i64 %t126, 1
  %t128 = load ptr, ptr %t20
  %t129 = getelementptr i8, ptr %t128, i64 0
  store i64 %t127, ptr %t129
  br label %L21
L21:
  call void @expect(ptr %t0, i64 78)
  br label %L9
L11:
  call void @expect(ptr %t0, i64 75)
  br label %L8
L8:
  %t132 = load ptr, ptr %t9
  %t133 = icmp ne ptr %t132, null
  br i1 %t133, label %L22, label %L24
L22:
  %t134 = load ptr, ptr %t9
  %t135 = load ptr, ptr %t20
  call void @register_struct(ptr %t0, ptr %t134, ptr %t135)
  br label %L24
L24:
  %t137 = load ptr, ptr %t20
  ret ptr %t137
L25:
  ret ptr null
}

define internal ptr @parse_enum_specifier(ptr %t0) {
entry:
  call void @advance(ptr %t0)
  %t2 = alloca ptr
  %t3 = call ptr @type_new(i64 20)
  store ptr %t3, ptr %t2
  %t4 = call i32 @check(ptr %t0, i64 4)
  %t5 = sext i32 %t4 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L2
L0:
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = getelementptr i8, ptr %t7, i64 0
  %t9 = load i64, ptr %t8
  %t10 = call ptr @strdup(i64 %t9)
  %t11 = load ptr, ptr %t2
  %t12 = getelementptr i8, ptr %t11, i64 0
  store ptr %t10, ptr %t12
  call void @advance(ptr %t0)
  br label %L2
L2:
  %t14 = call i32 @check(ptr %t0, i64 74)
  %t15 = sext i32 %t14 to i64
  %t16 = icmp ne i64 %t15, 0
  br i1 %t16, label %L3, label %L5
L3:
  call void @advance(ptr %t0)
  %t18 = alloca i64
  %t19 = sext i32 0 to i64
  store i64 %t19, ptr %t18
  br label %L6
L6:
  %t20 = call i32 @check(ptr %t0, i64 75)
  %t21 = sext i32 %t20 to i64
  %t23 = icmp eq i64 %t21, 0
  %t22 = zext i1 %t23 to i64
  %t24 = icmp ne i64 %t22, 0
  br i1 %t24, label %L9, label %L10
L9:
  %t25 = call i32 @check(ptr %t0, i64 81)
  %t26 = sext i32 %t25 to i64
  %t28 = icmp eq i64 %t26, 0
  %t27 = zext i1 %t28 to i64
  %t29 = icmp ne i64 %t27, 0
  %t30 = zext i1 %t29 to i64
  br label %L11
L10:
  br label %L11
L11:
  %t31 = phi i64 [ %t30, %L9 ], [ 0, %L10 ]
  %t32 = icmp ne i64 %t31, 0
  br i1 %t32, label %L7, label %L8
L7:
  %t33 = call i32 @check(ptr %t0, i64 4)
  %t34 = sext i32 %t33 to i64
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L12, label %L13
L12:
  %t36 = alloca ptr
  %t37 = getelementptr i8, ptr %t0, i64 0
  %t38 = getelementptr i8, ptr %t37, i64 0
  %t39 = load i64, ptr %t38
  %t40 = call ptr @strdup(i64 %t39)
  store ptr %t40, ptr %t36
  call void @advance(ptr %t0)
  %t42 = call i32 @match(ptr %t0, i64 55)
  %t43 = sext i32 %t42 to i64
  %t44 = icmp ne i64 %t43, 0
  br i1 %t44, label %L15, label %L17
L15:
  %t45 = call i32 @check(ptr %t0, i64 0)
  %t46 = sext i32 %t45 to i64
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L18, label %L19
L18:
  %t48 = getelementptr i8, ptr %t0, i64 0
  %t49 = getelementptr i8, ptr %t48, i64 0
  %t50 = load i64, ptr %t49
  %t52 = sext i32 0 to i64
  %t51 = inttoptr i64 %t52 to ptr
  %t53 = call i64 @strtoll(i64 %t50, ptr %t51, i64 0)
  %t54 = add i64 %t53, 0
  store i64 %t54, ptr %t18
  call void @advance(ptr %t0)
  br label %L20
L19:
  %t56 = call i32 @check(ptr %t0, i64 36)
  %t57 = sext i32 %t56 to i64
  %t58 = icmp ne i64 %t57, 0
  br i1 %t58, label %L21, label %L22
L21:
  call void @advance(ptr %t0)
  %t60 = call i32 @check(ptr %t0, i64 0)
  %t61 = sext i32 %t60 to i64
  %t62 = icmp ne i64 %t61, 0
  br i1 %t62, label %L24, label %L26
L24:
  %t63 = getelementptr i8, ptr %t0, i64 0
  %t64 = getelementptr i8, ptr %t63, i64 0
  %t65 = load i64, ptr %t64
  %t67 = sext i32 0 to i64
  %t66 = inttoptr i64 %t67 to ptr
  %t68 = call i64 @strtoll(i64 %t65, ptr %t66, i64 0)
  %t69 = add i64 %t68, 0
  %t70 = sub i64 0, %t69
  store i64 %t70, ptr %t18
  call void @advance(ptr %t0)
  br label %L26
L26:
  br label %L23
L22:
  %t72 = call i32 @check(ptr %t0, i64 4)
  %t73 = sext i32 %t72 to i64
  %t74 = icmp ne i64 %t73, 0
  br i1 %t74, label %L27, label %L29
L27:
  %t75 = alloca i64
  %t76 = getelementptr i8, ptr %t0, i64 0
  %t77 = getelementptr i8, ptr %t76, i64 0
  %t78 = load i64, ptr %t77
  %t79 = call i32 @lookup_enum_const(ptr %t0, i64 %t78, ptr %t75)
  %t80 = sext i32 %t79 to i64
  %t81 = icmp ne i64 %t80, 0
  br i1 %t81, label %L30, label %L32
L30:
  %t82 = load i64, ptr %t75
  store i64 %t82, ptr %t18
  br label %L32
L32:
  call void @advance(ptr %t0)
  %t84 = call i32 @check(ptr %t0, i64 35)
  %t85 = sext i32 %t84 to i64
  %t86 = icmp ne i64 %t85, 0
  br i1 %t86, label %L33, label %L34
L33:
  br label %L35
L34:
  %t87 = call i32 @check(ptr %t0, i64 36)
  %t88 = sext i32 %t87 to i64
  %t89 = icmp ne i64 %t88, 0
  %t90 = zext i1 %t89 to i64
  br label %L35
L35:
  %t91 = phi i64 [ 1, %L33 ], [ %t90, %L34 ]
  %t92 = icmp ne i64 %t91, 0
  br i1 %t92, label %L36, label %L38
L36:
  %t93 = alloca i64
  %t94 = getelementptr i8, ptr %t0, i64 0
  %t95 = getelementptr i8, ptr %t94, i64 0
  %t96 = load i64, ptr %t95
  %t98 = sext i32 36 to i64
  %t97 = icmp eq i64 %t96, %t98
  %t99 = zext i1 %t97 to i64
  store i64 %t99, ptr %t93
  call void @advance(ptr %t0)
  %t101 = call i32 @check(ptr %t0, i64 0)
  %t102 = sext i32 %t101 to i64
  %t103 = icmp ne i64 %t102, 0
  br i1 %t103, label %L39, label %L41
L39:
  %t104 = alloca i64
  %t105 = getelementptr i8, ptr %t0, i64 0
  %t106 = getelementptr i8, ptr %t105, i64 0
  %t107 = load i64, ptr %t106
  %t109 = sext i32 0 to i64
  %t108 = inttoptr i64 %t109 to ptr
  %t110 = call i64 @strtoll(i64 %t107, ptr %t108, i64 0)
  store i64 %t110, ptr %t104
  %t111 = load i64, ptr %t93
  %t113 = sext i32 %t111 to i64
  %t112 = icmp ne i64 %t113, 0
  br i1 %t112, label %L42, label %L43
L42:
  %t114 = load i64, ptr %t18
  %t115 = load i64, ptr %t104
  %t116 = sub i64 %t114, %t115
  br label %L44
L43:
  %t117 = load i64, ptr %t18
  %t118 = load i64, ptr %t104
  %t119 = add i64 %t117, %t118
  br label %L44
L44:
  %t120 = phi i64 [ %t116, %L42 ], [ %t119, %L43 ]
  store i64 %t120, ptr %t18
  call void @advance(ptr %t0)
  br label %L41
L41:
  br label %L38
L38:
  br label %L29
L29:
  br label %L23
L23:
  br label %L20
L20:
  br label %L17
L17:
  %t122 = load ptr, ptr %t36
  %t123 = load i64, ptr %t18
  %t124 = add i64 %t123, 1
  store i64 %t124, ptr %t18
  call void @register_enum_const(ptr %t0, ptr %t122, i64 %t123)
  %t126 = load ptr, ptr %t36
  call void @free(ptr %t126)
  br label %L14
L13:
  call void @advance(ptr %t0)
  br label %L14
L14:
  %t129 = call i32 @match(ptr %t0, i64 79)
  %t130 = sext i32 %t129 to i64
  %t132 = icmp eq i64 %t130, 0
  %t131 = zext i1 %t132 to i64
  %t133 = icmp ne i64 %t131, 0
  br i1 %t133, label %L45, label %L47
L45:
  br label %L8
L48:
  br label %L47
L47:
  br label %L6
L8:
  call void @expect(ptr %t0, i64 75)
  br label %L5
L5:
  %t135 = load ptr, ptr %t2
  ret ptr %t135
L49:
  ret ptr null
}

define internal ptr @parse_type_specifier(ptr %t0, ptr %t1, ptr %t2, ptr %t3) {
entry:
  %t4 = alloca i64
  %t5 = sext i32 0 to i64
  store i64 %t5, ptr %t4
  %t6 = alloca i64
  %t7 = sext i32 0 to i64
  store i64 %t7, ptr %t6
  %t8 = alloca i64
  %t9 = sext i32 0 to i64
  store i64 %t9, ptr %t8
  %t10 = alloca i64
  %t11 = sext i32 0 to i64
  store i64 %t11, ptr %t10
  %t12 = alloca i64
  %t13 = sext i32 0 to i64
  store i64 %t13, ptr %t12
  %t14 = alloca i64
  %t15 = sext i32 0 to i64
  store i64 %t15, ptr %t14
  %t16 = alloca i64
  %t17 = sext i32 0 to i64
  store i64 %t17, ptr %t16
  %t18 = alloca i64
  %t19 = sext i32 0 to i64
  store i64 %t19, ptr %t18
  %t20 = alloca i64
  %t21 = sext i32 0 to i64
  store i64 %t21, ptr %t20
  %t22 = alloca i64
  %t23 = sext i32 0 to i64
  store i64 %t23, ptr %t22
  %t24 = alloca i64
  %t25 = sext i32 7 to i64
  store i64 %t25, ptr %t24
  %t26 = alloca i64
  %t27 = sext i32 0 to i64
  store i64 %t27, ptr %t26
  %t28 = alloca ptr
  %t30 = sext i32 0 to i64
  %t29 = inttoptr i64 %t30 to ptr
  store ptr %t29, ptr %t28
  br label %L0
L0:
  br label %L1
L1:
  %t31 = call i32 @check(ptr %t0, i64 4)
  %t32 = sext i32 %t31 to i64
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L4, label %L5
L4:
  %t34 = getelementptr i8, ptr %t0, i64 0
  %t35 = getelementptr i8, ptr %t34, i64 0
  %t36 = load i64, ptr %t35
  %t37 = call i32 @is_gcc_extension(i64 %t36)
  %t38 = sext i32 %t37 to i64
  %t39 = icmp ne i64 %t38, 0
  %t40 = zext i1 %t39 to i64
  br label %L6
L5:
  br label %L6
L6:
  %t41 = phi i64 [ %t40, %L4 ], [ 0, %L5 ]
  %t42 = icmp ne i64 %t41, 0
  br i1 %t42, label %L7, label %L9
L7:
  call void @skip_gcc_extension(ptr %t0)
  br label %L2
L10:
  br label %L9
L9:
  %t44 = getelementptr i8, ptr %t0, i64 0
  %t45 = getelementptr i8, ptr %t44, i64 0
  %t46 = load i64, ptr %t45
  %t47 = add i64 %t46, 0
  switch i64 %t47, label %L30 [
    i64 29, label %L12
    i64 30, label %L13
    i64 31, label %L14
    i64 32, label %L15
    i64 33, label %L16
    i64 12, label %L17
    i64 13, label %L18
    i64 11, label %L19
    i64 10, label %L20
    i64 9, label %L21
    i64 6, label %L22
    i64 5, label %L23
    i64 7, label %L24
    i64 8, label %L25
    i64 26, label %L26
    i64 27, label %L27
    i64 28, label %L28
    i64 4, label %L29
  ]
L12:
  %t48 = sext i32 1 to i64
  store i64 %t48, ptr %t4
  call void @advance(ptr %t0)
  br label %L11
L31:
  br label %L13
L13:
  %t50 = sext i32 1 to i64
  store i64 %t50, ptr %t6
  call void @advance(ptr %t0)
  br label %L11
L32:
  br label %L14
L14:
  %t52 = sext i32 1 to i64
  store i64 %t52, ptr %t8
  call void @advance(ptr %t0)
  br label %L11
L33:
  br label %L15
L15:
  %t54 = sext i32 1 to i64
  store i64 %t54, ptr %t10
  call void @advance(ptr %t0)
  br label %L11
L34:
  br label %L16
L16:
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t12
  call void @advance(ptr %t0)
  br label %L11
L35:
  br label %L17
L17:
  %t58 = sext i32 1 to i64
  store i64 %t58, ptr %t14
  call void @advance(ptr %t0)
  br label %L11
L36:
  br label %L18
L18:
  %t60 = sext i32 1 to i64
  store i64 %t60, ptr %t16
  call void @advance(ptr %t0)
  br label %L11
L37:
  br label %L19
L19:
  %t62 = sext i32 1 to i64
  store i64 %t62, ptr %t22
  call void @advance(ptr %t0)
  br label %L11
L38:
  br label %L20
L20:
  %t64 = load i64, ptr %t18
  %t66 = sext i32 %t64 to i64
  %t65 = icmp ne i64 %t66, 0
  br i1 %t65, label %L39, label %L40
L39:
  %t67 = sext i32 1 to i64
  store i64 %t67, ptr %t20
  br label %L41
L40:
  %t68 = sext i32 1 to i64
  store i64 %t68, ptr %t18
  br label %L41
L41:
  call void @advance(ptr %t0)
  br label %L11
L42:
  br label %L21
L21:
  %t70 = sext i32 0 to i64
  store i64 %t70, ptr %t24
  %t71 = sext i32 1 to i64
  store i64 %t71, ptr %t26
  call void @advance(ptr %t0)
  br label %L11
L43:
  br label %L22
L22:
  %t73 = sext i32 2 to i64
  store i64 %t73, ptr %t24
  %t74 = sext i32 1 to i64
  store i64 %t74, ptr %t26
  call void @advance(ptr %t0)
  br label %L11
L44:
  br label %L23
L23:
  %t76 = sext i32 7 to i64
  store i64 %t76, ptr %t24
  %t77 = sext i32 1 to i64
  store i64 %t77, ptr %t26
  call void @advance(ptr %t0)
  br label %L11
L45:
  br label %L24
L24:
  %t79 = sext i32 13 to i64
  store i64 %t79, ptr %t24
  %t80 = sext i32 1 to i64
  store i64 %t80, ptr %t26
  call void @advance(ptr %t0)
  br label %L11
L46:
  br label %L25
L25:
  %t82 = sext i32 14 to i64
  store i64 %t82, ptr %t24
  %t83 = sext i32 1 to i64
  store i64 %t83, ptr %t26
  call void @advance(ptr %t0)
  br label %L11
L47:
  br label %L26
L26:
  br label %L27
L27:
  %t85 = call ptr @parse_struct_union(ptr %t0)
  store ptr %t85, ptr %t28
  %t86 = sext i32 1 to i64
  store i64 %t86, ptr %t26
  br label %parse_type_done
L48:
  br label %L28
L28:
  %t87 = call ptr @parse_enum_specifier(ptr %t0)
  store ptr %t87, ptr %t28
  %t88 = sext i32 1 to i64
  store i64 %t88, ptr %t26
  br label %parse_type_done
L49:
  br label %L29
L29:
  %t89 = alloca ptr
  %t90 = getelementptr i8, ptr %t0, i64 0
  %t91 = getelementptr i8, ptr %t90, i64 0
  %t92 = load i64, ptr %t91
  %t93 = call ptr @lookup_typedef(ptr %t0, i64 %t92)
  store ptr %t93, ptr %t89
  %t94 = load ptr, ptr %t89
  %t95 = icmp ne ptr %t94, null
  br i1 %t95, label %L50, label %L52
L50:
  %t96 = call ptr @type_new(i64 21)
  store ptr %t96, ptr %t28
  %t97 = getelementptr i8, ptr %t0, i64 0
  %t98 = getelementptr i8, ptr %t97, i64 0
  %t99 = load i64, ptr %t98
  %t100 = call ptr @strdup(i64 %t99)
  %t101 = load ptr, ptr %t28
  %t102 = getelementptr i8, ptr %t101, i64 0
  store ptr %t100, ptr %t102
  %t103 = sext i32 1 to i64
  store i64 %t103, ptr %t26
  call void @advance(ptr %t0)
  br label %parse_type_done
L53:
  br label %L52
L52:
  %t105 = alloca ptr
  %t106 = getelementptr i8, ptr %t0, i64 0
  %t107 = getelementptr i8, ptr %t106, i64 0
  %t108 = load i64, ptr %t107
  %t109 = call ptr @lookup_struct(ptr %t0, i64 %t108)
  store ptr %t109, ptr %t105
  %t110 = load ptr, ptr %t105
  %t111 = icmp ne ptr %t110, null
  br i1 %t111, label %L54, label %L56
L54:
  %t112 = load ptr, ptr %t105
  store ptr %t112, ptr %t28
  %t113 = sext i32 1 to i64
  store i64 %t113, ptr %t26
  call void @advance(ptr %t0)
  br label %parse_type_done
L57:
  br label %L56
L56:
  br label %parse_type_done
L58:
  br label %L11
L30:
  br label %parse_type_done
L59:
  br label %L11
L11:
  br label %L2
L2:
  br label %L0
L3:
  br label %parse_type_done
parse_type_done:
  %t115 = icmp ne ptr %t1, null
  br i1 %t115, label %L60, label %L62
L60:
  %t116 = load i64, ptr %t4
  %t117 = sext i32 %t116 to i64
  store i64 %t117, ptr %t1
  br label %L62
L62:
  %t118 = icmp ne ptr %t2, null
  br i1 %t118, label %L63, label %L65
L63:
  %t119 = load i64, ptr %t6
  %t120 = sext i32 %t119 to i64
  store i64 %t120, ptr %t2
  br label %L65
L65:
  %t121 = icmp ne ptr %t3, null
  br i1 %t121, label %L66, label %L68
L66:
  %t122 = load i64, ptr %t8
  %t123 = sext i32 %t122 to i64
  store i64 %t123, ptr %t3
  br label %L68
L68:
  %t124 = load ptr, ptr %t28
  %t125 = icmp ne ptr %t124, null
  br i1 %t125, label %L69, label %L71
L69:
  %t126 = load i64, ptr %t10
  %t127 = load ptr, ptr %t28
  %t128 = getelementptr i8, ptr %t127, i64 0
  %t129 = sext i32 %t126 to i64
  store i64 %t129, ptr %t128
  %t130 = load i64, ptr %t12
  %t131 = load ptr, ptr %t28
  %t132 = getelementptr i8, ptr %t131, i64 0
  %t133 = sext i32 %t130 to i64
  store i64 %t133, ptr %t132
  %t134 = load ptr, ptr %t28
  ret ptr %t134
L72:
  br label %L71
L71:
  %t135 = load i64, ptr %t26
  %t137 = sext i32 %t135 to i64
  %t138 = icmp eq i64 %t137, 0
  %t136 = zext i1 %t138 to i64
  %t139 = icmp ne i64 %t136, 0
  br i1 %t139, label %L73, label %L74
L73:
  %t140 = load i64, ptr %t18
  %t142 = sext i32 %t140 to i64
  %t143 = icmp eq i64 %t142, 0
  %t141 = zext i1 %t143 to i64
  %t144 = icmp ne i64 %t141, 0
  %t145 = zext i1 %t144 to i64
  br label %L75
L74:
  br label %L75
L75:
  %t146 = phi i64 [ %t145, %L73 ], [ 0, %L74 ]
  %t147 = icmp ne i64 %t146, 0
  br i1 %t147, label %L76, label %L77
L76:
  %t148 = load i64, ptr %t22
  %t150 = sext i32 %t148 to i64
  %t151 = icmp eq i64 %t150, 0
  %t149 = zext i1 %t151 to i64
  %t152 = icmp ne i64 %t149, 0
  %t153 = zext i1 %t152 to i64
  br label %L78
L77:
  br label %L78
L78:
  %t154 = phi i64 [ %t153, %L76 ], [ 0, %L77 ]
  %t155 = icmp ne i64 %t154, 0
  br i1 %t155, label %L79, label %L80
L79:
  %t156 = load i64, ptr %t14
  %t158 = sext i32 %t156 to i64
  %t159 = icmp eq i64 %t158, 0
  %t157 = zext i1 %t159 to i64
  %t160 = icmp ne i64 %t157, 0
  %t161 = zext i1 %t160 to i64
  br label %L81
L80:
  br label %L81
L81:
  %t162 = phi i64 [ %t161, %L79 ], [ 0, %L80 ]
  %t163 = icmp ne i64 %t162, 0
  br i1 %t163, label %L82, label %L83
L82:
  %t164 = load i64, ptr %t16
  %t166 = sext i32 %t164 to i64
  %t167 = icmp eq i64 %t166, 0
  %t165 = zext i1 %t167 to i64
  %t168 = icmp ne i64 %t165, 0
  %t169 = zext i1 %t168 to i64
  br label %L84
L83:
  br label %L84
L84:
  %t170 = phi i64 [ %t169, %L82 ], [ 0, %L83 ]
  %t171 = icmp ne i64 %t170, 0
  br i1 %t171, label %L85, label %L87
L85:
  %t173 = sext i32 0 to i64
  %t172 = inttoptr i64 %t173 to ptr
  ret ptr %t172
L88:
  br label %L87
L87:
  %t174 = load i64, ptr %t24
  %t176 = sext i32 %t174 to i64
  %t177 = sext i32 2 to i64
  %t175 = icmp eq i64 %t176, %t177
  %t178 = zext i1 %t175 to i64
  %t179 = icmp ne i64 %t178, 0
  br i1 %t179, label %L89, label %L90
L89:
  %t180 = load i64, ptr %t14
  %t182 = sext i32 %t180 to i64
  %t181 = icmp ne i64 %t182, 0
  br i1 %t181, label %L92, label %L93
L92:
  %t183 = sext i32 4 to i64
  store i64 %t183, ptr %t24
  br label %L94
L93:
  %t184 = load i64, ptr %t16
  %t186 = sext i32 %t184 to i64
  %t185 = icmp ne i64 %t186, 0
  br i1 %t185, label %L95, label %L97
L95:
  %t187 = sext i32 3 to i64
  store i64 %t187, ptr %t24
  br label %L97
L97:
  br label %L94
L94:
  br label %L91
L90:
  %t188 = load i64, ptr %t20
  %t190 = sext i32 %t188 to i64
  %t189 = icmp ne i64 %t190, 0
  br i1 %t189, label %L98, label %L99
L98:
  %t191 = load i64, ptr %t14
  %t193 = sext i32 %t191 to i64
  %t192 = icmp ne i64 %t193, 0
  br i1 %t192, label %L101, label %L102
L101:
  %t194 = sext i32 12 to i64
  br label %L103
L102:
  %t195 = sext i32 11 to i64
  br label %L103
L103:
  %t196 = phi i64 [ %t194, %L101 ], [ %t195, %L102 ]
  store i64 %t196, ptr %t24
  br label %L100
L99:
  %t197 = load i64, ptr %t18
  %t199 = sext i32 %t197 to i64
  %t198 = icmp ne i64 %t199, 0
  br i1 %t198, label %L104, label %L105
L104:
  %t200 = load i64, ptr %t14
  %t202 = sext i32 %t200 to i64
  %t201 = icmp ne i64 %t202, 0
  br i1 %t201, label %L107, label %L108
L107:
  %t203 = sext i32 10 to i64
  br label %L109
L108:
  %t204 = sext i32 9 to i64
  br label %L109
L109:
  %t205 = phi i64 [ %t203, %L107 ], [ %t204, %L108 ]
  store i64 %t205, ptr %t24
  br label %L106
L105:
  %t206 = load i64, ptr %t22
  %t208 = sext i32 %t206 to i64
  %t207 = icmp ne i64 %t208, 0
  br i1 %t207, label %L110, label %L111
L110:
  %t209 = load i64, ptr %t14
  %t211 = sext i32 %t209 to i64
  %t210 = icmp ne i64 %t211, 0
  br i1 %t210, label %L113, label %L114
L113:
  %t212 = sext i32 6 to i64
  br label %L115
L114:
  %t213 = sext i32 5 to i64
  br label %L115
L115:
  %t214 = phi i64 [ %t212, %L113 ], [ %t213, %L114 ]
  store i64 %t214, ptr %t24
  br label %L112
L111:
  %t215 = load i64, ptr %t24
  %t217 = sext i32 %t215 to i64
  %t218 = sext i32 7 to i64
  %t216 = icmp eq i64 %t217, %t218
  %t219 = zext i1 %t216 to i64
  %t220 = icmp ne i64 %t219, 0
  br i1 %t220, label %L116, label %L117
L116:
  br label %L118
L117:
  %t221 = load i64, ptr %t26
  %t223 = sext i32 %t221 to i64
  %t224 = icmp eq i64 %t223, 0
  %t222 = zext i1 %t224 to i64
  %t225 = icmp ne i64 %t222, 0
  %t226 = zext i1 %t225 to i64
  br label %L118
L118:
  %t227 = phi i64 [ 1, %L116 ], [ %t226, %L117 ]
  %t228 = icmp ne i64 %t227, 0
  br i1 %t228, label %L119, label %L121
L119:
  %t229 = load i64, ptr %t14
  %t231 = sext i32 %t229 to i64
  %t230 = icmp ne i64 %t231, 0
  br i1 %t230, label %L122, label %L124
L122:
  %t232 = sext i32 8 to i64
  store i64 %t232, ptr %t24
  br label %L124
L124:
  br label %L121
L121:
  br label %L112
L112:
  br label %L106
L106:
  br label %L100
L100:
  br label %L91
L91:
  %t233 = alloca ptr
  %t234 = load i64, ptr %t24
  %t235 = call ptr @type_new(i64 %t234)
  store ptr %t235, ptr %t233
  %t236 = load i64, ptr %t10
  %t237 = load ptr, ptr %t233
  %t238 = getelementptr i8, ptr %t237, i64 0
  %t239 = sext i32 %t236 to i64
  store i64 %t239, ptr %t238
  %t240 = load i64, ptr %t12
  %t241 = load ptr, ptr %t233
  %t242 = getelementptr i8, ptr %t241, i64 0
  %t243 = sext i32 %t240 to i64
  store i64 %t243, ptr %t242
  %t244 = load ptr, ptr %t233
  ret ptr %t244
L125:
  ret ptr null
}

define internal ptr @parse_declarator(ptr %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = alloca i64
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  %t5 = alloca ptr
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  br label %L0
L0:
  %t7 = call i32 @check(ptr %t0, i64 37)
  %t8 = sext i32 %t7 to i64
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L3, label %L4
L3:
  %t10 = load i64, ptr %t3
  %t12 = sext i32 %t10 to i64
  %t13 = sext i32 16 to i64
  %t11 = icmp slt i64 %t12, %t13
  %t14 = zext i1 %t11 to i64
  %t15 = icmp ne i64 %t14, 0
  %t16 = zext i1 %t15 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t17 = phi i64 [ %t16, %L3 ], [ 0, %L4 ]
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L1, label %L2
L1:
  call void @advance(ptr %t0)
  %t20 = load ptr, ptr %t5
  %t21 = load i64, ptr %t3
  %t23 = sext i32 %t21 to i64
  %t22 = getelementptr ptr, ptr %t20, i64 %t23
  %t24 = sext i32 0 to i64
  store i64 %t24, ptr %t22
  br label %L6
L6:
  %t25 = call i32 @check(ptr %t0, i64 32)
  %t26 = sext i32 %t25 to i64
  %t27 = icmp ne i64 %t26, 0
  br i1 %t27, label %L9, label %L10
L9:
  br label %L11
L10:
  %t28 = call i32 @check(ptr %t0, i64 33)
  %t29 = sext i32 %t28 to i64
  %t30 = icmp ne i64 %t29, 0
  %t31 = zext i1 %t30 to i64
  br label %L11
L11:
  %t32 = phi i64 [ 1, %L9 ], [ %t31, %L10 ]
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L7, label %L8
L7:
  %t34 = call i32 @check(ptr %t0, i64 32)
  %t35 = sext i32 %t34 to i64
  %t36 = icmp ne i64 %t35, 0
  br i1 %t36, label %L12, label %L14
L12:
  %t37 = load ptr, ptr %t5
  %t38 = load i64, ptr %t3
  %t40 = sext i32 %t38 to i64
  %t39 = getelementptr ptr, ptr %t37, i64 %t40
  %t41 = sext i32 1 to i64
  store i64 %t41, ptr %t39
  br label %L14
L14:
  call void @advance(ptr %t0)
  br label %L6
L8:
  %t43 = load i64, ptr %t3
  %t45 = sext i32 %t43 to i64
  %t44 = add i64 %t45, 1
  store i64 %t44, ptr %t3
  br label %L0
L2:
  %t46 = alloca i64
  %t47 = load i64, ptr %t3
  %t49 = sext i32 %t47 to i64
  %t50 = sext i32 1 to i64
  %t48 = sub i64 %t49, %t50
  store i64 %t48, ptr %t46
  br label %L15
L15:
  %t51 = load i64, ptr %t46
  %t53 = sext i32 %t51 to i64
  %t54 = sext i32 0 to i64
  %t52 = icmp sge i64 %t53, %t54
  %t55 = zext i1 %t52 to i64
  %t56 = icmp ne i64 %t55, 0
  br i1 %t56, label %L16, label %L18
L16:
  %t57 = alloca ptr
  %t58 = call ptr @type_ptr(ptr %t1)
  store ptr %t58, ptr %t57
  %t59 = load ptr, ptr %t5
  %t60 = load i64, ptr %t46
  %t61 = sext i32 %t60 to i64
  %t62 = getelementptr ptr, ptr %t59, i64 %t61
  %t63 = load ptr, ptr %t62
  %t64 = load ptr, ptr %t57
  %t65 = getelementptr i8, ptr %t64, i64 0
  store ptr %t63, ptr %t65
  %t66 = load ptr, ptr %t57
  store ptr %t66, ptr %t1
  br label %L17
L17:
  %t67 = load i64, ptr %t46
  %t69 = sext i32 %t67 to i64
  %t68 = sub i64 %t69, 1
  store i64 %t68, ptr %t46
  br label %L15
L18:
  %t70 = icmp ne ptr %t2, null
  br i1 %t70, label %L19, label %L21
L19:
  %t72 = sext i32 0 to i64
  %t71 = inttoptr i64 %t72 to ptr
  store ptr %t71, ptr %t2
  br label %L21
L21:
  call void @skip_gcc_extension(ptr %t0)
  %t74 = call i32 @check(ptr %t0, i64 4)
  %t75 = sext i32 %t74 to i64
  %t76 = icmp ne i64 %t75, 0
  br i1 %t76, label %L22, label %L23
L22:
  %t77 = getelementptr i8, ptr %t0, i64 0
  %t78 = getelementptr i8, ptr %t77, i64 0
  %t79 = load i64, ptr %t78
  %t80 = call i32 @is_gcc_extension(i64 %t79)
  %t81 = sext i32 %t80 to i64
  %t83 = icmp eq i64 %t81, 0
  %t82 = zext i1 %t83 to i64
  %t84 = icmp ne i64 %t82, 0
  %t85 = zext i1 %t84 to i64
  br label %L24
L23:
  br label %L24
L24:
  %t86 = phi i64 [ %t85, %L22 ], [ 0, %L23 ]
  %t87 = icmp ne i64 %t86, 0
  br i1 %t87, label %L25, label %L26
L25:
  %t88 = icmp ne ptr %t2, null
  br i1 %t88, label %L28, label %L30
L28:
  %t89 = getelementptr i8, ptr %t0, i64 0
  %t90 = getelementptr i8, ptr %t89, i64 0
  %t91 = load i64, ptr %t90
  %t92 = call ptr @strdup(i64 %t91)
  store ptr %t92, ptr %t2
  br label %L30
L30:
  call void @advance(ptr %t0)
  br label %L27
L26:
  %t94 = call i32 @check(ptr %t0, i64 72)
  %t95 = sext i32 %t94 to i64
  %t96 = icmp ne i64 %t95, 0
  br i1 %t96, label %L31, label %L33
L31:
  call void @advance(ptr %t0)
  %t98 = call ptr @parse_declarator(ptr %t0, ptr %t1, ptr %t2)
  store ptr %t98, ptr %t1
  call void @expect(ptr %t0, i64 73)
  br label %L33
L33:
  br label %L27
L27:
  call void @skip_gcc_extension(ptr %t0)
  br label %L34
L34:
  br label %L35
L35:
  %t101 = call i32 @check(ptr %t0, i64 4)
  %t102 = sext i32 %t101 to i64
  %t103 = icmp ne i64 %t102, 0
  br i1 %t103, label %L38, label %L39
L38:
  %t104 = getelementptr i8, ptr %t0, i64 0
  %t105 = getelementptr i8, ptr %t104, i64 0
  %t106 = load i64, ptr %t105
  %t107 = call i32 @is_gcc_extension(i64 %t106)
  %t108 = sext i32 %t107 to i64
  %t109 = icmp ne i64 %t108, 0
  %t110 = zext i1 %t109 to i64
  br label %L40
L39:
  br label %L40
L40:
  %t111 = phi i64 [ %t110, %L38 ], [ 0, %L39 ]
  %t112 = icmp ne i64 %t111, 0
  br i1 %t112, label %L41, label %L43
L41:
  call void @skip_gcc_extension(ptr %t0)
  br label %L36
L44:
  br label %L43
L43:
  %t114 = call i32 @check(ptr %t0, i64 76)
  %t115 = sext i32 %t114 to i64
  %t116 = icmp ne i64 %t115, 0
  br i1 %t116, label %L45, label %L46
L45:
  call void @advance(ptr %t0)
  %t118 = alloca i64
  %t120 = sext i32 1 to i64
  %t119 = sub i64 0, %t120
  store i64 %t119, ptr %t118
  %t121 = call i32 @check(ptr %t0, i64 77)
  %t122 = sext i32 %t121 to i64
  %t124 = icmp eq i64 %t122, 0
  %t123 = zext i1 %t124 to i64
  %t125 = icmp ne i64 %t123, 0
  br i1 %t125, label %L48, label %L50
L48:
  %t126 = call i32 @check(ptr %t0, i64 0)
  %t127 = sext i32 %t126 to i64
  %t128 = icmp ne i64 %t127, 0
  br i1 %t128, label %L51, label %L52
L51:
  %t129 = getelementptr i8, ptr %t0, i64 0
  %t130 = getelementptr i8, ptr %t129, i64 0
  %t131 = load i64, ptr %t130
  %t132 = call i64 @atol(i64 %t131)
  %t133 = add i64 %t132, 0
  store i64 %t133, ptr %t118
  call void @advance(ptr %t0)
  br label %L53
L52:
  %t135 = alloca i64
  %t136 = sext i32 0 to i64
  store i64 %t136, ptr %t135
  br label %L54
L54:
  %t137 = call i32 @check(ptr %t0, i64 81)
  %t138 = sext i32 %t137 to i64
  %t140 = icmp eq i64 %t138, 0
  %t139 = zext i1 %t140 to i64
  %t141 = icmp ne i64 %t139, 0
  br i1 %t141, label %L55, label %L56
L55:
  %t142 = call i32 @check(ptr %t0, i64 76)
  %t143 = sext i32 %t142 to i64
  %t144 = icmp ne i64 %t143, 0
  br i1 %t144, label %L57, label %L59
L57:
  %t145 = load i64, ptr %t135
  %t147 = sext i32 %t145 to i64
  %t146 = add i64 %t147, 1
  store i64 %t146, ptr %t135
  br label %L59
L59:
  %t148 = call i32 @check(ptr %t0, i64 77)
  %t149 = sext i32 %t148 to i64
  %t150 = icmp ne i64 %t149, 0
  br i1 %t150, label %L60, label %L62
L60:
  %t151 = load i64, ptr %t135
  %t153 = sext i32 %t151 to i64
  %t154 = sext i32 0 to i64
  %t152 = icmp eq i64 %t153, %t154
  %t155 = zext i1 %t152 to i64
  %t156 = icmp ne i64 %t155, 0
  br i1 %t156, label %L63, label %L65
L63:
  br label %L56
L66:
  br label %L65
L65:
  %t157 = load i64, ptr %t135
  %t159 = sext i32 %t157 to i64
  %t158 = sub i64 %t159, 1
  store i64 %t158, ptr %t135
  br label %L62
L62:
  call void @advance(ptr %t0)
  br label %L54
L56:
  br label %L53
L53:
  br label %L50
L50:
  call void @expect(ptr %t0, i64 77)
  %t162 = load i64, ptr %t118
  %t163 = call ptr @type_array(ptr %t1, i64 %t162)
  store ptr %t163, ptr %t1
  br label %L47
L46:
  %t164 = call i32 @check(ptr %t0, i64 72)
  %t165 = sext i32 %t164 to i64
  %t166 = icmp ne i64 %t165, 0
  br i1 %t166, label %L67, label %L68
L67:
  call void @advance(ptr %t0)
  %t168 = alloca ptr
  %t169 = call ptr @type_new(i64 17)
  store ptr %t169, ptr %t168
  %t170 = load ptr, ptr %t168
  %t171 = getelementptr i8, ptr %t170, i64 0
  store ptr %t1, ptr %t171
  %t172 = alloca ptr
  %t174 = sext i32 0 to i64
  %t173 = inttoptr i64 %t174 to ptr
  store ptr %t173, ptr %t172
  %t175 = alloca i64
  %t176 = sext i32 0 to i64
  store i64 %t176, ptr %t175
  %t177 = alloca i64
  %t178 = sext i32 0 to i64
  store i64 %t178, ptr %t177
  br label %L70
L70:
  %t179 = call i32 @check(ptr %t0, i64 73)
  %t180 = sext i32 %t179 to i64
  %t182 = icmp eq i64 %t180, 0
  %t181 = zext i1 %t182 to i64
  %t183 = icmp ne i64 %t181, 0
  br i1 %t183, label %L73, label %L74
L73:
  %t184 = call i32 @check(ptr %t0, i64 81)
  %t185 = sext i32 %t184 to i64
  %t187 = icmp eq i64 %t185, 0
  %t186 = zext i1 %t187 to i64
  %t188 = icmp ne i64 %t186, 0
  %t189 = zext i1 %t188 to i64
  br label %L75
L74:
  br label %L75
L75:
  %t190 = phi i64 [ %t189, %L73 ], [ 0, %L74 ]
  %t191 = icmp ne i64 %t190, 0
  br i1 %t191, label %L71, label %L72
L71:
  %t192 = call i32 @check(ptr %t0, i64 80)
  %t193 = sext i32 %t192 to i64
  %t194 = icmp ne i64 %t193, 0
  br i1 %t194, label %L76, label %L78
L76:
  %t195 = sext i32 1 to i64
  store i64 %t195, ptr %t177
  call void @advance(ptr %t0)
  br label %L72
L79:
  br label %L78
L78:
  %t197 = alloca i64
  %t198 = sext i32 0 to i64
  store i64 %t198, ptr %t197
  %t199 = alloca i64
  %t200 = sext i32 0 to i64
  store i64 %t200, ptr %t199
  %t201 = alloca i64
  %t202 = sext i32 0 to i64
  store i64 %t202, ptr %t201
  %t203 = alloca ptr
  %t204 = call ptr @parse_type_specifier(ptr %t0, ptr %t197, ptr %t199, ptr %t201)
  store ptr %t204, ptr %t203
  %t205 = load ptr, ptr %t203
  %t207 = ptrtoint ptr %t205 to i64
  %t208 = icmp eq i64 %t207, 0
  %t206 = zext i1 %t208 to i64
  %t209 = icmp ne i64 %t206, 0
  br i1 %t209, label %L80, label %L82
L80:
  br label %L72
L83:
  br label %L82
L82:
  %t210 = alloca ptr
  %t212 = sext i32 0 to i64
  %t211 = inttoptr i64 %t212 to ptr
  store ptr %t211, ptr %t210
  %t213 = load ptr, ptr %t203
  %t214 = call ptr @parse_declarator(ptr %t0, ptr %t213, ptr %t210)
  store ptr %t214, ptr %t203
  %t215 = load ptr, ptr %t172
  %t216 = load i64, ptr %t175
  %t218 = sext i32 %t216 to i64
  %t219 = sext i32 1 to i64
  %t217 = add i64 %t218, %t219
  %t221 = sext i32 0 to i64
  %t220 = mul i64 %t217, %t221
  %t222 = call ptr @realloc(ptr %t215, i64 %t220)
  store ptr %t222, ptr %t172
  %t223 = load ptr, ptr %t172
  %t225 = ptrtoint ptr %t223 to i64
  %t226 = icmp eq i64 %t225, 0
  %t224 = zext i1 %t226 to i64
  %t227 = icmp ne i64 %t224, 0
  br i1 %t227, label %L84, label %L86
L84:
  %t228 = getelementptr [8 x i8], ptr @.str31, i64 0, i64 0
  call void @perror(ptr %t228)
  call void @exit(i64 1)
  br label %L86
L86:
  %t231 = load ptr, ptr %t210
  %t232 = load ptr, ptr %t172
  %t233 = load i64, ptr %t175
  %t235 = sext i32 %t233 to i64
  %t234 = getelementptr ptr, ptr %t232, i64 %t235
  %t236 = getelementptr i8, ptr %t234, i64 0
  store ptr %t231, ptr %t236
  %t237 = load ptr, ptr %t203
  %t238 = load ptr, ptr %t172
  %t239 = load i64, ptr %t175
  %t241 = sext i32 %t239 to i64
  %t240 = getelementptr ptr, ptr %t238, i64 %t241
  %t242 = getelementptr i8, ptr %t240, i64 0
  store ptr %t237, ptr %t242
  %t243 = load i64, ptr %t175
  %t245 = sext i32 %t243 to i64
  %t244 = add i64 %t245, 1
  store i64 %t244, ptr %t175
  %t246 = call i32 @match(ptr %t0, i64 79)
  %t247 = sext i32 %t246 to i64
  %t249 = icmp eq i64 %t247, 0
  %t248 = zext i1 %t249 to i64
  %t250 = icmp ne i64 %t248, 0
  br i1 %t250, label %L87, label %L89
L87:
  br label %L72
L90:
  br label %L89
L89:
  br label %L70
L72:
  call void @expect(ptr %t0, i64 73)
  %t252 = load ptr, ptr %t172
  %t253 = load ptr, ptr %t168
  %t254 = getelementptr i8, ptr %t253, i64 0
  store ptr %t252, ptr %t254
  %t255 = load i64, ptr %t175
  %t256 = load ptr, ptr %t168
  %t257 = getelementptr i8, ptr %t256, i64 0
  %t258 = sext i32 %t255 to i64
  store i64 %t258, ptr %t257
  %t259 = load i64, ptr %t177
  %t260 = load ptr, ptr %t168
  %t261 = getelementptr i8, ptr %t260, i64 0
  %t262 = sext i32 %t259 to i64
  store i64 %t262, ptr %t261
  %t263 = load ptr, ptr %t168
  store ptr %t263, ptr %t1
  br label %L69
L68:
  br label %L37
L91:
  br label %L69
L69:
  br label %L47
L47:
  br label %L36
L36:
  br label %L34
L37:
  ret ptr %t1
L92:
  ret ptr null
}

define internal ptr @parse_primary(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  %t5 = call i32 @check(ptr %t0, i64 0)
  %t6 = sext i32 %t5 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = alloca ptr
  %t9 = load i64, ptr %t1
  %t10 = call ptr @node_new(i64 19, i64 %t9)
  store ptr %t10, ptr %t8
  %t11 = getelementptr i8, ptr %t0, i64 0
  %t12 = getelementptr i8, ptr %t11, i64 0
  %t13 = load i64, ptr %t12
  %t15 = sext i32 0 to i64
  %t14 = inttoptr i64 %t15 to ptr
  %t16 = call i64 @strtoll(i64 %t13, ptr %t14, i64 0)
  %t17 = add i64 %t16, 0
  %t18 = load ptr, ptr %t8
  %t19 = getelementptr i8, ptr %t18, i64 0
  store i64 %t17, ptr %t19
  call void @advance(ptr %t0)
  %t21 = load ptr, ptr %t8
  ret ptr %t21
L3:
  br label %L2
L2:
  %t22 = call i32 @check(ptr %t0, i64 1)
  %t23 = sext i32 %t22 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L4, label %L6
L4:
  %t25 = alloca ptr
  %t26 = load i64, ptr %t1
  %t27 = call ptr @node_new(i64 20, i64 %t26)
  store ptr %t27, ptr %t25
  %t28 = getelementptr i8, ptr %t0, i64 0
  %t29 = getelementptr i8, ptr %t28, i64 0
  %t30 = load i64, ptr %t29
  %t31 = call i32 @atof(i64 %t30)
  %t32 = sext i32 %t31 to i64
  %t33 = load ptr, ptr %t25
  %t34 = getelementptr i8, ptr %t33, i64 0
  store i64 %t32, ptr %t34
  call void @advance(ptr %t0)
  %t36 = load ptr, ptr %t25
  ret ptr %t36
L7:
  br label %L6
L6:
  %t37 = call i32 @check(ptr %t0, i64 2)
  %t38 = sext i32 %t37 to i64
  %t39 = icmp ne i64 %t38, 0
  br i1 %t39, label %L8, label %L10
L8:
  %t40 = alloca ptr
  %t41 = load i64, ptr %t1
  %t42 = call ptr @node_new(i64 21, i64 %t41)
  store ptr %t42, ptr %t40
  %t43 = alloca ptr
  %t44 = getelementptr i8, ptr %t0, i64 0
  %t45 = getelementptr i8, ptr %t44, i64 0
  %t46 = load i64, ptr %t45
  store i64 %t46, ptr %t43
  %t47 = load ptr, ptr %t43
  %t48 = sext i32 0 to i64
  %t49 = getelementptr ptr, ptr %t47, i64 %t48
  %t50 = load ptr, ptr %t49
  %t52 = ptrtoint ptr %t50 to i64
  %t53 = sext i32 39 to i64
  %t51 = icmp eq i64 %t52, %t53
  %t54 = zext i1 %t51 to i64
  %t55 = icmp ne i64 %t54, 0
  br i1 %t55, label %L11, label %L12
L11:
  %t56 = load ptr, ptr %t43
  %t57 = sext i32 1 to i64
  %t58 = getelementptr ptr, ptr %t56, i64 %t57
  %t59 = load ptr, ptr %t58
  %t61 = ptrtoint ptr %t59 to i64
  %t62 = sext i32 92 to i64
  %t60 = icmp eq i64 %t61, %t62
  %t63 = zext i1 %t60 to i64
  %t64 = icmp ne i64 %t63, 0
  %t65 = zext i1 %t64 to i64
  br label %L13
L12:
  br label %L13
L13:
  %t66 = phi i64 [ %t65, %L11 ], [ 0, %L12 ]
  %t67 = icmp ne i64 %t66, 0
  br i1 %t67, label %L14, label %L15
L14:
  %t68 = load ptr, ptr %t43
  %t69 = sext i32 2 to i64
  %t70 = getelementptr ptr, ptr %t68, i64 %t69
  %t71 = load ptr, ptr %t70
  %t72 = ptrtoint ptr %t71 to i64
  %t73 = add i64 %t72, 0
  switch i64 %t73, label %L22 [
    i64 110, label %L18
    i64 116, label %L19
    i64 114, label %L20
    i64 48, label %L21
  ]
L18:
  %t74 = load ptr, ptr %t40
  %t75 = getelementptr i8, ptr %t74, i64 0
  %t76 = sext i32 10 to i64
  store i64 %t76, ptr %t75
  br label %L17
L23:
  br label %L19
L19:
  %t77 = load ptr, ptr %t40
  %t78 = getelementptr i8, ptr %t77, i64 0
  %t79 = sext i32 9 to i64
  store i64 %t79, ptr %t78
  br label %L17
L24:
  br label %L20
L20:
  %t80 = load ptr, ptr %t40
  %t81 = getelementptr i8, ptr %t80, i64 0
  %t82 = sext i32 13 to i64
  store i64 %t82, ptr %t81
  br label %L17
L25:
  br label %L21
L21:
  %t83 = load ptr, ptr %t40
  %t84 = getelementptr i8, ptr %t83, i64 0
  %t85 = sext i32 0 to i64
  store i64 %t85, ptr %t84
  br label %L17
L26:
  br label %L17
L22:
  %t86 = load ptr, ptr %t43
  %t87 = sext i32 2 to i64
  %t88 = getelementptr ptr, ptr %t86, i64 %t87
  %t89 = load ptr, ptr %t88
  %t90 = load ptr, ptr %t40
  %t91 = getelementptr i8, ptr %t90, i64 0
  store ptr %t89, ptr %t91
  br label %L17
L27:
  br label %L17
L17:
  br label %L16
L15:
  %t92 = load ptr, ptr %t43
  %t93 = sext i32 1 to i64
  %t94 = getelementptr ptr, ptr %t92, i64 %t93
  %t95 = load ptr, ptr %t94
  %t96 = ptrtoint ptr %t95 to i64
  %t97 = load ptr, ptr %t40
  %t98 = getelementptr i8, ptr %t97, i64 0
  store i64 %t96, ptr %t98
  br label %L16
L16:
  call void @advance(ptr %t0)
  %t100 = load ptr, ptr %t40
  ret ptr %t100
L28:
  br label %L10
L10:
  %t101 = call i32 @check(ptr %t0, i64 3)
  %t102 = sext i32 %t101 to i64
  %t103 = icmp ne i64 %t102, 0
  br i1 %t103, label %L29, label %L31
L29:
  %t104 = alloca ptr
  %t105 = load i64, ptr %t1
  %t106 = call ptr @node_new(i64 22, i64 %t105)
  store ptr %t106, ptr %t104
  %t107 = alloca i64
  %t108 = getelementptr i8, ptr %t0, i64 0
  %t109 = getelementptr i8, ptr %t108, i64 0
  %t110 = load i64, ptr %t109
  %t111 = call i64 @strlen(i64 %t110)
  store i64 %t111, ptr %t107
  %t112 = alloca ptr
  %t113 = load i64, ptr %t107
  %t115 = sext i32 %t113 to i64
  %t116 = sext i32 1 to i64
  %t114 = add i64 %t115, %t116
  %t117 = call ptr @malloc(i64 %t114)
  store ptr %t117, ptr %t112
  %t118 = load ptr, ptr %t112
  %t119 = getelementptr i8, ptr %t0, i64 0
  %t120 = getelementptr i8, ptr %t119, i64 0
  %t121 = load i64, ptr %t120
  %t122 = load i64, ptr %t107
  %t124 = sext i32 %t122 to i64
  %t125 = sext i32 1 to i64
  %t123 = sub i64 %t124, %t125
  %t126 = call ptr @memcpy(ptr %t118, i64 %t121, i64 %t123)
  %t127 = load ptr, ptr %t112
  %t128 = load i64, ptr %t107
  %t130 = sext i32 %t128 to i64
  %t131 = sext i32 1 to i64
  %t129 = sub i64 %t130, %t131
  %t132 = getelementptr ptr, ptr %t127, i64 %t129
  %t133 = sext i32 0 to i64
  store i64 %t133, ptr %t132
  call void @advance(ptr %t0)
  br label %L32
L32:
  %t135 = call i32 @check(ptr %t0, i64 3)
  %t136 = sext i32 %t135 to i64
  %t137 = icmp ne i64 %t136, 0
  br i1 %t137, label %L33, label %L34
L33:
  %t138 = alloca ptr
  %t139 = getelementptr i8, ptr %t0, i64 0
  %t140 = getelementptr i8, ptr %t139, i64 0
  %t141 = load i64, ptr %t140
  %t143 = sext i32 1 to i64
  %t142 = add i64 %t141, %t143
  store i64 %t142, ptr %t138
  %t144 = alloca i64
  %t145 = load ptr, ptr %t138
  %t146 = call i64 @strlen(ptr %t145)
  store i64 %t146, ptr %t144
  %t147 = alloca i64
  %t148 = load ptr, ptr %t112
  %t149 = call i64 @strlen(ptr %t148)
  store i64 %t149, ptr %t147
  %t150 = load ptr, ptr %t112
  %t151 = load i64, ptr %t147
  %t152 = load i64, ptr %t144
  %t154 = sext i32 %t151 to i64
  %t155 = sext i32 %t152 to i64
  %t153 = add i64 %t154, %t155
  %t157 = sext i32 1 to i64
  %t156 = add i64 %t153, %t157
  %t158 = call ptr @realloc(ptr %t150, i64 %t156)
  store ptr %t158, ptr %t112
  %t159 = load ptr, ptr %t112
  %t160 = load i64, ptr %t147
  %t162 = ptrtoint ptr %t159 to i64
  %t163 = sext i32 %t160 to i64
  %t164 = inttoptr i64 %t162 to ptr
  %t161 = getelementptr i8, ptr %t164, i64 %t163
  %t165 = load ptr, ptr %t138
  %t166 = load i64, ptr %t144
  %t167 = call ptr @memcpy(ptr %t161, ptr %t165, i64 %t166)
  %t168 = load ptr, ptr %t112
  %t169 = load i64, ptr %t147
  %t170 = load i64, ptr %t144
  %t172 = sext i32 %t169 to i64
  %t173 = sext i32 %t170 to i64
  %t171 = add i64 %t172, %t173
  %t174 = getelementptr ptr, ptr %t168, i64 %t171
  %t175 = sext i32 0 to i64
  store i64 %t175, ptr %t174
  call void @advance(ptr %t0)
  br label %L32
L34:
  %t177 = alloca i64
  %t178 = load ptr, ptr %t112
  %t179 = call i64 @strlen(ptr %t178)
  store i64 %t179, ptr %t177
  %t180 = load ptr, ptr %t112
  %t181 = load i64, ptr %t177
  %t183 = sext i32 %t181 to i64
  %t184 = sext i32 2 to i64
  %t182 = add i64 %t183, %t184
  %t185 = call ptr @realloc(ptr %t180, i64 %t182)
  store ptr %t185, ptr %t112
  %t186 = load ptr, ptr %t112
  %t187 = load i64, ptr %t177
  %t189 = sext i32 %t187 to i64
  %t188 = getelementptr ptr, ptr %t186, i64 %t189
  %t190 = sext i32 34 to i64
  store i64 %t190, ptr %t188
  %t191 = load ptr, ptr %t112
  %t192 = load i64, ptr %t177
  %t194 = sext i32 %t192 to i64
  %t195 = sext i32 1 to i64
  %t193 = add i64 %t194, %t195
  %t196 = getelementptr ptr, ptr %t191, i64 %t193
  %t197 = sext i32 0 to i64
  store i64 %t197, ptr %t196
  %t198 = load ptr, ptr %t112
  %t199 = load ptr, ptr %t104
  %t200 = getelementptr i8, ptr %t199, i64 0
  store ptr %t198, ptr %t200
  %t201 = load ptr, ptr %t104
  ret ptr %t201
L35:
  br label %L31
L31:
  %t202 = call i32 @check(ptr %t0, i64 4)
  %t203 = sext i32 %t202 to i64
  %t204 = icmp ne i64 %t203, 0
  br i1 %t204, label %L36, label %L38
L36:
  %t205 = alloca i64
  %t206 = getelementptr i8, ptr %t0, i64 0
  %t207 = getelementptr i8, ptr %t206, i64 0
  %t208 = load i64, ptr %t207
  %t209 = call i32 @lookup_enum_const(ptr %t0, i64 %t208, ptr %t205)
  %t210 = sext i32 %t209 to i64
  %t211 = icmp ne i64 %t210, 0
  br i1 %t211, label %L39, label %L41
L39:
  %t212 = alloca ptr
  %t213 = load i64, ptr %t1
  %t214 = call ptr @node_new(i64 19, i64 %t213)
  store ptr %t214, ptr %t212
  %t215 = load i64, ptr %t205
  %t216 = load ptr, ptr %t212
  %t217 = getelementptr i8, ptr %t216, i64 0
  store i64 %t215, ptr %t217
  call void @advance(ptr %t0)
  %t219 = load ptr, ptr %t212
  ret ptr %t219
L42:
  br label %L41
L41:
  %t220 = alloca ptr
  %t221 = load i64, ptr %t1
  %t222 = call ptr @node_new(i64 23, i64 %t221)
  store ptr %t222, ptr %t220
  %t223 = getelementptr i8, ptr %t0, i64 0
  %t224 = getelementptr i8, ptr %t223, i64 0
  %t225 = load i64, ptr %t224
  %t226 = call ptr @strdup(i64 %t225)
  %t227 = load ptr, ptr %t220
  %t228 = getelementptr i8, ptr %t227, i64 0
  store ptr %t226, ptr %t228
  call void @advance(ptr %t0)
  %t230 = load ptr, ptr %t220
  ret ptr %t230
L43:
  br label %L38
L38:
  %t231 = call i32 @match(ptr %t0, i64 72)
  %t232 = sext i32 %t231 to i64
  %t233 = icmp ne i64 %t232, 0
  br i1 %t233, label %L44, label %L46
L44:
  %t234 = call i32 @is_type_start(ptr %t0)
  %t235 = sext i32 %t234 to i64
  %t236 = icmp ne i64 %t235, 0
  br i1 %t236, label %L47, label %L49
L47:
  %t237 = alloca i64
  %t238 = sext i32 0 to i64
  store i64 %t238, ptr %t237
  %t239 = alloca i64
  %t240 = sext i32 0 to i64
  store i64 %t240, ptr %t239
  %t241 = alloca i64
  %t242 = sext i32 0 to i64
  store i64 %t242, ptr %t241
  %t243 = alloca ptr
  %t244 = call ptr @parse_type_specifier(ptr %t0, ptr %t237, ptr %t239, ptr %t241)
  store ptr %t244, ptr %t243
  %t245 = load ptr, ptr %t243
  %t246 = icmp ne ptr %t245, null
  br i1 %t246, label %L50, label %L52
L50:
  %t247 = alloca ptr
  %t249 = sext i32 0 to i64
  %t248 = inttoptr i64 %t249 to ptr
  store ptr %t248, ptr %t247
  %t250 = load ptr, ptr %t243
  %t251 = call ptr @parse_declarator(ptr %t0, ptr %t250, ptr %t247)
  store ptr %t251, ptr %t243
  %t252 = load ptr, ptr %t247
  call void @free(ptr %t252)
  %t254 = call i32 @match(ptr %t0, i64 73)
  %t255 = sext i32 %t254 to i64
  %t256 = icmp ne i64 %t255, 0
  br i1 %t256, label %L53, label %L55
L53:
  %t257 = alloca ptr
  %t258 = load i64, ptr %t1
  %t259 = call ptr @node_new(i64 29, i64 %t258)
  store ptr %t259, ptr %t257
  %t260 = load ptr, ptr %t243
  %t261 = load ptr, ptr %t257
  %t262 = getelementptr i8, ptr %t261, i64 0
  store ptr %t260, ptr %t262
  %t263 = call ptr @parse_cast(ptr %t0)
  %t264 = load ptr, ptr %t257
  %t265 = getelementptr i8, ptr %t264, i64 0
  store ptr %t263, ptr %t265
  %t266 = load ptr, ptr %t257
  ret ptr %t266
L56:
  br label %L55
L55:
  br label %L52
L52:
  br label %L49
L49:
  %t267 = alloca ptr
  %t268 = call ptr @parse_expr(ptr %t0)
  store ptr %t268, ptr %t267
  call void @expect(ptr %t0, i64 73)
  %t270 = load ptr, ptr %t267
  ret ptr %t270
L57:
  br label %L46
L46:
  %t271 = call i32 @check(ptr %t0, i64 34)
  %t272 = sext i32 %t271 to i64
  %t273 = icmp ne i64 %t272, 0
  br i1 %t273, label %L58, label %L60
L58:
  call void @advance(ptr %t0)
  %t275 = call i32 @match(ptr %t0, i64 72)
  %t276 = sext i32 %t275 to i64
  %t277 = icmp ne i64 %t276, 0
  br i1 %t277, label %L61, label %L63
L61:
  %t278 = call i32 @is_type_start(ptr %t0)
  %t279 = sext i32 %t278 to i64
  %t280 = icmp ne i64 %t279, 0
  br i1 %t280, label %L64, label %L66
L64:
  %t281 = alloca i64
  %t282 = sext i32 0 to i64
  store i64 %t282, ptr %t281
  %t283 = alloca i64
  %t284 = sext i32 0 to i64
  store i64 %t284, ptr %t283
  %t285 = alloca i64
  %t286 = sext i32 0 to i64
  store i64 %t286, ptr %t285
  %t287 = alloca ptr
  %t288 = call ptr @parse_type_specifier(ptr %t0, ptr %t281, ptr %t283, ptr %t285)
  store ptr %t288, ptr %t287
  %t289 = alloca ptr
  %t291 = sext i32 0 to i64
  %t290 = inttoptr i64 %t291 to ptr
  store ptr %t290, ptr %t289
  %t292 = load ptr, ptr %t287
  %t293 = call ptr @parse_declarator(ptr %t0, ptr %t292, ptr %t289)
  store ptr %t293, ptr %t287
  %t294 = load ptr, ptr %t289
  call void @free(ptr %t294)
  call void @expect(ptr %t0, i64 73)
  %t297 = alloca ptr
  %t298 = load i64, ptr %t1
  %t299 = call ptr @node_new(i64 31, i64 %t298)
  store ptr %t299, ptr %t297
  %t300 = load ptr, ptr %t287
  %t301 = load ptr, ptr %t297
  %t302 = getelementptr i8, ptr %t301, i64 0
  store ptr %t300, ptr %t302
  %t303 = load ptr, ptr %t297
  ret ptr %t303
L67:
  br label %L66
L66:
  %t304 = alloca ptr
  %t305 = call ptr @parse_expr(ptr %t0)
  store ptr %t305, ptr %t304
  call void @expect(ptr %t0, i64 73)
  %t307 = alloca ptr
  %t308 = load i64, ptr %t1
  %t309 = call ptr @node_new(i64 32, i64 %t308)
  store ptr %t309, ptr %t307
  %t310 = load ptr, ptr %t307
  %t311 = load ptr, ptr %t304
  call void @node_add_child(ptr %t310, ptr %t311)
  %t313 = load ptr, ptr %t307
  ret ptr %t313
L68:
  br label %L63
L63:
  %t314 = alloca ptr
  %t315 = call ptr @parse_unary(ptr %t0)
  store ptr %t315, ptr %t314
  %t316 = alloca ptr
  %t317 = load i64, ptr %t1
  %t318 = call ptr @node_new(i64 32, i64 %t317)
  store ptr %t318, ptr %t316
  %t319 = load ptr, ptr %t316
  %t320 = load ptr, ptr %t314
  call void @node_add_child(ptr %t319, ptr %t320)
  %t322 = load ptr, ptr %t316
  ret ptr %t322
L69:
  br label %L60
L60:
  %t323 = getelementptr [28 x i8], ptr @.str32, i64 0, i64 0
  call void @p_error(ptr %t0, ptr %t323)
  %t326 = sext i32 0 to i64
  %t325 = inttoptr i64 %t326 to ptr
  ret ptr %t325
L70:
  ret ptr null
}

define internal ptr @parse_postfix(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_primary(ptr %t0)
  store ptr %t2, ptr %t1
  br label %L0
L0:
  br label %L1
L1:
  %t3 = alloca i64
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = getelementptr i8, ptr %t4, i64 0
  %t6 = load i64, ptr %t5
  store i64 %t6, ptr %t3
  %t7 = call i32 @match(ptr %t0, i64 72)
  %t8 = sext i32 %t7 to i64
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L4, label %L5
L4:
  %t10 = alloca ptr
  %t11 = load i64, ptr %t3
  %t12 = call ptr @node_new(i64 24, i64 %t11)
  store ptr %t12, ptr %t10
  %t13 = load ptr, ptr %t10
  %t14 = load ptr, ptr %t1
  call void @node_add_child(ptr %t13, ptr %t14)
  br label %L7
L7:
  %t16 = call i32 @check(ptr %t0, i64 73)
  %t17 = sext i32 %t16 to i64
  %t19 = icmp eq i64 %t17, 0
  %t18 = zext i1 %t19 to i64
  %t20 = icmp ne i64 %t18, 0
  br i1 %t20, label %L10, label %L11
L10:
  %t21 = call i32 @check(ptr %t0, i64 81)
  %t22 = sext i32 %t21 to i64
  %t24 = icmp eq i64 %t22, 0
  %t23 = zext i1 %t24 to i64
  %t25 = icmp ne i64 %t23, 0
  %t26 = zext i1 %t25 to i64
  br label %L12
L11:
  br label %L12
L12:
  %t27 = phi i64 [ %t26, %L10 ], [ 0, %L11 ]
  %t28 = icmp ne i64 %t27, 0
  br i1 %t28, label %L8, label %L9
L8:
  %t29 = load ptr, ptr %t10
  %t30 = call ptr @parse_assign(ptr %t0)
  call void @node_add_child(ptr %t29, ptr %t30)
  %t32 = call i32 @match(ptr %t0, i64 79)
  %t33 = sext i32 %t32 to i64
  %t35 = icmp eq i64 %t33, 0
  %t34 = zext i1 %t35 to i64
  %t36 = icmp ne i64 %t34, 0
  br i1 %t36, label %L13, label %L15
L13:
  br label %L9
L16:
  br label %L15
L15:
  br label %L7
L9:
  call void @expect(ptr %t0, i64 73)
  %t38 = load ptr, ptr %t10
  store ptr %t38, ptr %t1
  br label %L6
L5:
  %t39 = call i32 @match(ptr %t0, i64 76)
  %t40 = sext i32 %t39 to i64
  %t41 = icmp ne i64 %t40, 0
  br i1 %t41, label %L17, label %L18
L17:
  %t42 = alloca ptr
  %t43 = load i64, ptr %t3
  %t44 = call ptr @node_new(i64 33, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load ptr, ptr %t42
  %t46 = load ptr, ptr %t1
  call void @node_add_child(ptr %t45, ptr %t46)
  %t48 = load ptr, ptr %t42
  %t49 = call ptr @parse_expr(ptr %t0)
  call void @node_add_child(ptr %t48, ptr %t49)
  call void @expect(ptr %t0, i64 77)
  %t52 = load ptr, ptr %t42
  store ptr %t52, ptr %t1
  br label %L19
L18:
  %t53 = call i32 @match(ptr %t0, i64 69)
  %t54 = sext i32 %t53 to i64
  %t55 = icmp ne i64 %t54, 0
  br i1 %t55, label %L20, label %L21
L20:
  %t56 = alloca ptr
  %t57 = load i64, ptr %t3
  %t58 = call ptr @node_new(i64 34, i64 %t57)
  store ptr %t58, ptr %t56
  %t59 = call ptr @expect_ident(ptr %t0)
  %t60 = load ptr, ptr %t56
  %t61 = getelementptr i8, ptr %t60, i64 0
  store ptr %t59, ptr %t61
  %t62 = load ptr, ptr %t56
  %t63 = load ptr, ptr %t1
  call void @node_add_child(ptr %t62, ptr %t63)
  %t65 = load ptr, ptr %t56
  store ptr %t65, ptr %t1
  br label %L22
L21:
  %t66 = call i32 @match(ptr %t0, i64 68)
  %t67 = sext i32 %t66 to i64
  %t68 = icmp ne i64 %t67, 0
  br i1 %t68, label %L23, label %L24
L23:
  %t69 = alloca ptr
  %t70 = load i64, ptr %t3
  %t71 = call ptr @node_new(i64 35, i64 %t70)
  store ptr %t71, ptr %t69
  %t72 = call ptr @expect_ident(ptr %t0)
  %t73 = load ptr, ptr %t69
  %t74 = getelementptr i8, ptr %t73, i64 0
  store ptr %t72, ptr %t74
  %t75 = load ptr, ptr %t69
  %t76 = load ptr, ptr %t1
  call void @node_add_child(ptr %t75, ptr %t76)
  %t78 = load ptr, ptr %t69
  store ptr %t78, ptr %t1
  br label %L25
L24:
  %t79 = call i32 @check(ptr %t0, i64 66)
  %t80 = sext i32 %t79 to i64
  %t81 = icmp ne i64 %t80, 0
  br i1 %t81, label %L26, label %L27
L26:
  call void @advance(ptr %t0)
  %t83 = alloca ptr
  %t84 = load i64, ptr %t3
  %t85 = call ptr @node_new(i64 40, i64 %t84)
  store ptr %t85, ptr %t83
  %t86 = load ptr, ptr %t83
  %t87 = load ptr, ptr %t1
  call void @node_add_child(ptr %t86, ptr %t87)
  %t89 = load ptr, ptr %t83
  store ptr %t89, ptr %t1
  br label %L28
L27:
  %t90 = call i32 @check(ptr %t0, i64 67)
  %t91 = sext i32 %t90 to i64
  %t92 = icmp ne i64 %t91, 0
  br i1 %t92, label %L29, label %L30
L29:
  call void @advance(ptr %t0)
  %t94 = alloca ptr
  %t95 = load i64, ptr %t3
  %t96 = call ptr @node_new(i64 41, i64 %t95)
  store ptr %t96, ptr %t94
  %t97 = load ptr, ptr %t94
  %t98 = load ptr, ptr %t1
  call void @node_add_child(ptr %t97, ptr %t98)
  %t100 = load ptr, ptr %t94
  store ptr %t100, ptr %t1
  br label %L31
L30:
  br label %L3
L32:
  br label %L31
L31:
  br label %L28
L28:
  br label %L25
L25:
  br label %L22
L22:
  br label %L19
L19:
  br label %L6
L6:
  br label %L2
L2:
  br label %L0
L3:
  %t101 = load ptr, ptr %t1
  ret ptr %t101
L33:
  ret ptr null
}

define internal ptr @parse_unary(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  %t5 = call i32 @check(ptr %t0, i64 66)
  %t6 = sext i32 %t5 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L2
L0:
  call void @advance(ptr %t0)
  %t9 = alloca ptr
  %t10 = load i64, ptr %t1
  %t11 = call ptr @node_new(i64 38, i64 %t10)
  store ptr %t11, ptr %t9
  %t12 = load ptr, ptr %t9
  %t13 = call ptr @parse_unary(ptr %t0)
  call void @node_add_child(ptr %t12, ptr %t13)
  %t15 = load ptr, ptr %t9
  ret ptr %t15
L3:
  br label %L2
L2:
  %t16 = call i32 @check(ptr %t0, i64 67)
  %t17 = sext i32 %t16 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L4, label %L6
L4:
  call void @advance(ptr %t0)
  %t20 = alloca ptr
  %t21 = load i64, ptr %t1
  %t22 = call ptr @node_new(i64 39, i64 %t21)
  store ptr %t22, ptr %t20
  %t23 = load ptr, ptr %t20
  %t24 = call ptr @parse_unary(ptr %t0)
  call void @node_add_child(ptr %t23, ptr %t24)
  %t26 = load ptr, ptr %t20
  ret ptr %t26
L7:
  br label %L6
L6:
  %t27 = call i32 @check(ptr %t0, i64 40)
  %t28 = sext i32 %t27 to i64
  %t29 = icmp ne i64 %t28, 0
  br i1 %t29, label %L8, label %L10
L8:
  call void @advance(ptr %t0)
  %t31 = alloca ptr
  %t32 = load i64, ptr %t1
  %t33 = call ptr @node_new(i64 36, i64 %t32)
  store ptr %t33, ptr %t31
  %t34 = load ptr, ptr %t31
  %t35 = call ptr @parse_cast(ptr %t0)
  call void @node_add_child(ptr %t34, ptr %t35)
  %t37 = load ptr, ptr %t31
  ret ptr %t37
L11:
  br label %L10
L10:
  %t38 = call i32 @check(ptr %t0, i64 37)
  %t39 = sext i32 %t38 to i64
  %t40 = icmp ne i64 %t39, 0
  br i1 %t40, label %L12, label %L14
L12:
  call void @advance(ptr %t0)
  %t42 = alloca ptr
  %t43 = load i64, ptr %t1
  %t44 = call ptr @node_new(i64 37, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load ptr, ptr %t42
  %t46 = call ptr @parse_cast(ptr %t0)
  call void @node_add_child(ptr %t45, ptr %t46)
  %t48 = load ptr, ptr %t42
  ret ptr %t48
L15:
  br label %L14
L14:
  %t49 = call i32 @check(ptr %t0, i64 36)
  %t50 = sext i32 %t49 to i64
  %t51 = icmp ne i64 %t50, 0
  br i1 %t51, label %L16, label %L17
L16:
  br label %L18
L17:
  %t52 = call i32 @check(ptr %t0, i64 35)
  %t53 = sext i32 %t52 to i64
  %t54 = icmp ne i64 %t53, 0
  %t55 = zext i1 %t54 to i64
  br label %L18
L18:
  %t56 = phi i64 [ 1, %L16 ], [ %t55, %L17 ]
  %t57 = icmp ne i64 %t56, 0
  br i1 %t57, label %L19, label %L20
L19:
  br label %L21
L20:
  %t58 = call i32 @check(ptr %t0, i64 54)
  %t59 = sext i32 %t58 to i64
  %t60 = icmp ne i64 %t59, 0
  %t61 = zext i1 %t60 to i64
  br label %L21
L21:
  %t62 = phi i64 [ 1, %L19 ], [ %t61, %L20 ]
  %t63 = icmp ne i64 %t62, 0
  br i1 %t63, label %L22, label %L23
L22:
  br label %L24
L23:
  %t64 = call i32 @check(ptr %t0, i64 43)
  %t65 = sext i32 %t64 to i64
  %t66 = icmp ne i64 %t65, 0
  %t67 = zext i1 %t66 to i64
  br label %L24
L24:
  %t68 = phi i64 [ 1, %L22 ], [ %t67, %L23 ]
  %t69 = icmp ne i64 %t68, 0
  br i1 %t69, label %L25, label %L27
L25:
  %t70 = alloca i64
  %t71 = getelementptr i8, ptr %t0, i64 0
  %t72 = getelementptr i8, ptr %t71, i64 0
  %t73 = load i64, ptr %t72
  store i64 %t73, ptr %t70
  call void @advance(ptr %t0)
  %t75 = alloca ptr
  %t76 = load i64, ptr %t1
  %t77 = call ptr @node_new(i64 26, i64 %t76)
  store ptr %t77, ptr %t75
  %t78 = load i64, ptr %t70
  %t79 = load ptr, ptr %t75
  %t80 = getelementptr i8, ptr %t79, i64 0
  %t81 = sext i32 %t78 to i64
  store i64 %t81, ptr %t80
  %t82 = load ptr, ptr %t75
  %t83 = call ptr @parse_cast(ptr %t0)
  call void @node_add_child(ptr %t82, ptr %t83)
  %t85 = load ptr, ptr %t75
  ret ptr %t85
L28:
  br label %L27
L27:
  %t86 = call ptr @parse_postfix(ptr %t0)
  ret ptr %t86
L29:
  ret ptr null
}

define internal ptr @parse_cast(ptr %t0) {
entry:
  %t1 = call ptr @parse_unary(ptr %t0)
  ret ptr %t1
L0:
  ret ptr null
}

define internal ptr @parse_mul(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_cast(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_cast(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_add(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_mul(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_mul(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_shift(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_add(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_add(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_relational(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_shift(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_shift(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_equality(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_relational(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_relational(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_bitand(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_equality(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_equality(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_bitxor(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_bitand(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_bitand(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_bitor(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_bitxor(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_bitxor(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_logand(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_bitor(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_bitor(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_logor(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_logand(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca ptr
  %t4 = sext i32 0 to i64
  store i64 %t4, ptr %t3
  br label %L0
L0:
  br label %L1
L1:
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  br label %L4
L4:
  %t9 = load ptr, ptr %t3
  %t10 = load i64, ptr %t7
  %t11 = sext i32 %t10 to i64
  %t12 = getelementptr ptr, ptr %t9, i64 %t11
  %t13 = load ptr, ptr %t12
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = sext i32 81 to i64
  %t14 = icmp ne i64 %t15, %t16
  %t17 = zext i1 %t14 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L5, label %L7
L5:
  %t19 = getelementptr i8, ptr %t0, i64 0
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t22 = load ptr, ptr %t3
  %t23 = load i64, ptr %t7
  %t24 = sext i32 %t23 to i64
  %t25 = getelementptr ptr, ptr %t22, i64 %t24
  %t26 = load ptr, ptr %t25
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = icmp eq i64 %t21, %t28
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L8, label %L10
L8:
  %t31 = alloca i64
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = getelementptr i8, ptr %t32, i64 0
  %t34 = load i64, ptr %t33
  store i64 %t34, ptr %t31
  %t35 = alloca i64
  %t36 = getelementptr i8, ptr %t0, i64 0
  %t37 = getelementptr i8, ptr %t36, i64 0
  %t38 = load i64, ptr %t37
  store i64 %t38, ptr %t35
  call void @advance(ptr %t0)
  %t40 = alloca ptr
  %t41 = call ptr @parse_logand(ptr %t0)
  store ptr %t41, ptr %t40
  %t42 = alloca ptr
  %t43 = load i64, ptr %t31
  %t44 = call ptr @node_new(i64 25, i64 %t43)
  store ptr %t44, ptr %t42
  %t45 = load i64, ptr %t35
  %t46 = load ptr, ptr %t42
  %t47 = getelementptr i8, ptr %t46, i64 0
  %t48 = sext i32 %t45 to i64
  store i64 %t48, ptr %t47
  %t49 = load ptr, ptr %t42
  %t50 = load ptr, ptr %t1
  call void @node_add_child(ptr %t49, ptr %t50)
  %t52 = load ptr, ptr %t42
  %t53 = load ptr, ptr %t40
  call void @node_add_child(ptr %t52, ptr %t53)
  %t55 = load ptr, ptr %t42
  store ptr %t55, ptr %t1
  %t56 = sext i32 1 to i64
  store i64 %t56, ptr %t5
  br label %L7
L11:
  br label %L10
L10:
  br label %L6
L6:
  %t57 = load i64, ptr %t7
  %t59 = sext i32 %t57 to i64
  %t58 = add i64 %t59, 1
  store i64 %t58, ptr %t7
  br label %L4
L7:
  %t60 = load i64, ptr %t5
  %t62 = sext i32 %t60 to i64
  %t63 = icmp eq i64 %t62, 0
  %t61 = zext i1 %t63 to i64
  %t64 = icmp ne i64 %t61, 0
  br i1 %t64, label %L12, label %L14
L12:
  br label %L3
L15:
  br label %L14
L14:
  br label %L2
L2:
  br label %L0
L3:
  %t65 = load ptr, ptr %t1
  ret ptr %t65
L16:
  ret ptr null
}

define internal ptr @parse_ternary(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_logor(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = call i32 @check(ptr %t0, i64 70)
  %t4 = sext i32 %t3 to i64
  %t6 = icmp eq i64 %t4, 0
  %t5 = zext i1 %t6 to i64
  %t7 = icmp ne i64 %t5, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = load ptr, ptr %t1
  ret ptr %t8
L3:
  br label %L2
L2:
  %t9 = alloca i64
  %t10 = getelementptr i8, ptr %t0, i64 0
  %t11 = getelementptr i8, ptr %t10, i64 0
  %t12 = load i64, ptr %t11
  store i64 %t12, ptr %t9
  call void @advance(ptr %t0)
  %t14 = alloca ptr
  %t15 = call ptr @parse_expr(ptr %t0)
  store ptr %t15, ptr %t14
  call void @expect(ptr %t0, i64 71)
  %t17 = alloca ptr
  %t18 = call ptr @parse_ternary(ptr %t0)
  store ptr %t18, ptr %t17
  %t19 = alloca ptr
  %t20 = load i64, ptr %t9
  %t21 = call ptr @node_new(i64 30, i64 %t20)
  store ptr %t21, ptr %t19
  %t22 = load ptr, ptr %t1
  %t23 = load ptr, ptr %t19
  %t24 = getelementptr i8, ptr %t23, i64 0
  store ptr %t22, ptr %t24
  %t25 = load ptr, ptr %t19
  %t26 = load ptr, ptr %t14
  call void @node_add_child(ptr %t25, ptr %t26)
  %t28 = load ptr, ptr %t19
  %t29 = load ptr, ptr %t17
  call void @node_add_child(ptr %t28, ptr %t29)
  %t31 = load ptr, ptr %t19
  ret ptr %t31
L4:
  ret ptr null
}

define internal ptr @parse_assign(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_ternary(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = alloca i64
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = getelementptr i8, ptr %t4, i64 0
  %t6 = load i64, ptr %t5
  store i64 %t6, ptr %t3
  %t7 = alloca i64
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = getelementptr i8, ptr %t8, i64 0
  %t10 = load i64, ptr %t9
  store i64 %t10, ptr %t7
  %t11 = load i64, ptr %t7
  %t13 = sext i32 %t11 to i64
  %t14 = sext i32 55 to i64
  %t12 = icmp eq i64 %t13, %t14
  %t15 = zext i1 %t12 to i64
  %t16 = icmp ne i64 %t15, 0
  br i1 %t16, label %L0, label %L1
L0:
  br label %L2
L1:
  %t17 = load i64, ptr %t7
  %t19 = sext i32 %t17 to i64
  %t20 = sext i32 56 to i64
  %t18 = icmp eq i64 %t19, %t20
  %t21 = zext i1 %t18 to i64
  %t22 = icmp ne i64 %t21, 0
  %t23 = zext i1 %t22 to i64
  br label %L2
L2:
  %t24 = phi i64 [ 1, %L0 ], [ %t23, %L1 ]
  %t25 = icmp ne i64 %t24, 0
  br i1 %t25, label %L3, label %L4
L3:
  br label %L5
L4:
  %t26 = load i64, ptr %t7
  %t28 = sext i32 %t26 to i64
  %t29 = sext i32 57 to i64
  %t27 = icmp eq i64 %t28, %t29
  %t30 = zext i1 %t27 to i64
  %t31 = icmp ne i64 %t30, 0
  %t32 = zext i1 %t31 to i64
  br label %L5
L5:
  %t33 = phi i64 [ 1, %L3 ], [ %t32, %L4 ]
  %t34 = icmp ne i64 %t33, 0
  br i1 %t34, label %L6, label %L7
L6:
  br label %L8
L7:
  %t35 = load i64, ptr %t7
  %t37 = sext i32 %t35 to i64
  %t38 = sext i32 58 to i64
  %t36 = icmp eq i64 %t37, %t38
  %t39 = zext i1 %t36 to i64
  %t40 = icmp ne i64 %t39, 0
  %t41 = zext i1 %t40 to i64
  br label %L8
L8:
  %t42 = phi i64 [ 1, %L6 ], [ %t41, %L7 ]
  %t43 = icmp ne i64 %t42, 0
  br i1 %t43, label %L9, label %L10
L9:
  br label %L11
L10:
  %t44 = load i64, ptr %t7
  %t46 = sext i32 %t44 to i64
  %t47 = sext i32 59 to i64
  %t45 = icmp eq i64 %t46, %t47
  %t48 = zext i1 %t45 to i64
  %t49 = icmp ne i64 %t48, 0
  %t50 = zext i1 %t49 to i64
  br label %L11
L11:
  %t51 = phi i64 [ 1, %L9 ], [ %t50, %L10 ]
  %t52 = icmp ne i64 %t51, 0
  br i1 %t52, label %L12, label %L13
L12:
  br label %L14
L13:
  %t53 = load i64, ptr %t7
  %t55 = sext i32 %t53 to i64
  %t56 = sext i32 65 to i64
  %t54 = icmp eq i64 %t55, %t56
  %t57 = zext i1 %t54 to i64
  %t58 = icmp ne i64 %t57, 0
  %t59 = zext i1 %t58 to i64
  br label %L14
L14:
  %t60 = phi i64 [ 1, %L12 ], [ %t59, %L13 ]
  %t61 = icmp ne i64 %t60, 0
  br i1 %t61, label %L15, label %L16
L15:
  br label %L17
L16:
  %t62 = load i64, ptr %t7
  %t64 = sext i32 %t62 to i64
  %t65 = sext i32 60 to i64
  %t63 = icmp eq i64 %t64, %t65
  %t66 = zext i1 %t63 to i64
  %t67 = icmp ne i64 %t66, 0
  %t68 = zext i1 %t67 to i64
  br label %L17
L17:
  %t69 = phi i64 [ 1, %L15 ], [ %t68, %L16 ]
  %t70 = icmp ne i64 %t69, 0
  br i1 %t70, label %L18, label %L19
L18:
  br label %L20
L19:
  %t71 = load i64, ptr %t7
  %t73 = sext i32 %t71 to i64
  %t74 = sext i32 61 to i64
  %t72 = icmp eq i64 %t73, %t74
  %t75 = zext i1 %t72 to i64
  %t76 = icmp ne i64 %t75, 0
  %t77 = zext i1 %t76 to i64
  br label %L20
L20:
  %t78 = phi i64 [ 1, %L18 ], [ %t77, %L19 ]
  %t79 = icmp ne i64 %t78, 0
  br i1 %t79, label %L21, label %L22
L21:
  br label %L23
L22:
  %t80 = load i64, ptr %t7
  %t82 = sext i32 %t80 to i64
  %t83 = sext i32 62 to i64
  %t81 = icmp eq i64 %t82, %t83
  %t84 = zext i1 %t81 to i64
  %t85 = icmp ne i64 %t84, 0
  %t86 = zext i1 %t85 to i64
  br label %L23
L23:
  %t87 = phi i64 [ 1, %L21 ], [ %t86, %L22 ]
  %t88 = icmp ne i64 %t87, 0
  br i1 %t88, label %L24, label %L25
L24:
  br label %L26
L25:
  %t89 = load i64, ptr %t7
  %t91 = sext i32 %t89 to i64
  %t92 = sext i32 63 to i64
  %t90 = icmp eq i64 %t91, %t92
  %t93 = zext i1 %t90 to i64
  %t94 = icmp ne i64 %t93, 0
  %t95 = zext i1 %t94 to i64
  br label %L26
L26:
  %t96 = phi i64 [ 1, %L24 ], [ %t95, %L25 ]
  %t97 = icmp ne i64 %t96, 0
  br i1 %t97, label %L27, label %L28
L27:
  br label %L29
L28:
  %t98 = load i64, ptr %t7
  %t100 = sext i32 %t98 to i64
  %t101 = sext i32 64 to i64
  %t99 = icmp eq i64 %t100, %t101
  %t102 = zext i1 %t99 to i64
  %t103 = icmp ne i64 %t102, 0
  %t104 = zext i1 %t103 to i64
  br label %L29
L29:
  %t105 = phi i64 [ 1, %L27 ], [ %t104, %L28 ]
  %t106 = icmp ne i64 %t105, 0
  br i1 %t106, label %L30, label %L32
L30:
  %t107 = alloca i64
  %t108 = load i64, ptr %t7
  %t109 = sext i32 %t108 to i64
  store i64 %t109, ptr %t107
  call void @advance(ptr %t0)
  %t111 = alloca ptr
  %t112 = call ptr @parse_assign(ptr %t0)
  store ptr %t112, ptr %t111
  %t113 = alloca i64
  %t114 = load i64, ptr %t107
  %t116 = sext i32 %t114 to i64
  %t117 = sext i32 55 to i64
  %t115 = icmp eq i64 %t116, %t117
  %t118 = zext i1 %t115 to i64
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L33, label %L34
L33:
  %t120 = sext i32 27 to i64
  br label %L35
L34:
  %t121 = sext i32 28 to i64
  br label %L35
L35:
  %t122 = phi i64 [ %t120, %L33 ], [ %t121, %L34 ]
  store i64 %t122, ptr %t113
  %t123 = alloca ptr
  %t124 = load i64, ptr %t113
  %t125 = load i64, ptr %t3
  %t126 = call ptr @node_new(i64 %t124, i64 %t125)
  store ptr %t126, ptr %t123
  %t127 = load i64, ptr %t107
  %t128 = load ptr, ptr %t123
  %t129 = getelementptr i8, ptr %t128, i64 0
  %t130 = sext i32 %t127 to i64
  store i64 %t130, ptr %t129
  %t131 = load ptr, ptr %t123
  %t132 = load ptr, ptr %t1
  call void @node_add_child(ptr %t131, ptr %t132)
  %t134 = load ptr, ptr %t123
  %t135 = load ptr, ptr %t111
  call void @node_add_child(ptr %t134, ptr %t135)
  %t137 = load ptr, ptr %t123
  ret ptr %t137
L36:
  br label %L32
L32:
  %t138 = load ptr, ptr %t1
  ret ptr %t138
L37:
  ret ptr null
}

define internal ptr @parse_expr(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @parse_assign(ptr %t0)
  store ptr %t2, ptr %t1
  %t3 = call i32 @check(ptr %t0, i64 79)
  %t4 = sext i32 %t3 to i64
  %t5 = icmp ne i64 %t4, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = alloca i64
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = getelementptr i8, ptr %t7, i64 0
  %t9 = load i64, ptr %t8
  store i64 %t9, ptr %t6
  %t10 = alloca ptr
  %t11 = load i64, ptr %t6
  %t12 = call ptr @node_new(i64 43, i64 %t11)
  store ptr %t12, ptr %t10
  %t13 = load ptr, ptr %t10
  %t14 = load ptr, ptr %t1
  call void @node_add_child(ptr %t13, ptr %t14)
  br label %L3
L3:
  %t16 = call i32 @match(ptr %t0, i64 79)
  %t17 = sext i32 %t16 to i64
  %t18 = icmp ne i64 %t17, 0
  br i1 %t18, label %L4, label %L5
L4:
  %t19 = load ptr, ptr %t10
  %t20 = call ptr @parse_assign(ptr %t0)
  call void @node_add_child(ptr %t19, ptr %t20)
  br label %L3
L5:
  %t22 = load ptr, ptr %t10
  ret ptr %t22
L6:
  br label %L2
L2:
  %t23 = load ptr, ptr %t1
  ret ptr %t23
L7:
  ret ptr null
}

define internal ptr @parse_local_decl(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  %t5 = alloca i64
  %t6 = sext i32 0 to i64
  store i64 %t6, ptr %t5
  %t7 = alloca i64
  %t8 = sext i32 0 to i64
  store i64 %t8, ptr %t7
  %t9 = alloca i64
  %t10 = sext i32 0 to i64
  store i64 %t10, ptr %t9
  %t11 = alloca ptr
  %t12 = call ptr @parse_type_specifier(ptr %t0, ptr %t5, ptr %t7, ptr %t9)
  store ptr %t12, ptr %t11
  %t13 = load ptr, ptr %t11
  %t15 = ptrtoint ptr %t13 to i64
  %t16 = icmp eq i64 %t15, 0
  %t14 = zext i1 %t16 to i64
  %t17 = icmp ne i64 %t14, 0
  br i1 %t17, label %L0, label %L2
L0:
  %t19 = sext i32 0 to i64
  %t18 = inttoptr i64 %t19 to ptr
  ret ptr %t18
L3:
  br label %L2
L2:
  %t20 = alloca ptr
  %t21 = load i64, ptr %t1
  %t22 = call ptr @node_new(i64 5, i64 %t21)
  store ptr %t22, ptr %t20
  br label %L4
L4:
  %t23 = alloca ptr
  %t25 = sext i32 0 to i64
  %t24 = inttoptr i64 %t25 to ptr
  store ptr %t24, ptr %t23
  %t26 = alloca ptr
  %t27 = load ptr, ptr %t11
  %t28 = call ptr @parse_declarator(ptr %t0, ptr %t27, ptr %t23)
  store ptr %t28, ptr %t26
  %t29 = load i64, ptr %t5
  %t30 = sext i32 %t29 to i64
  %t31 = icmp ne i64 %t30, 0
  br i1 %t31, label %L7, label %L8
L7:
  %t32 = load ptr, ptr %t23
  %t33 = ptrtoint ptr %t32 to i64
  %t34 = icmp ne i64 %t33, 0
  %t35 = zext i1 %t34 to i64
  br label %L9
L8:
  br label %L9
L9:
  %t36 = phi i64 [ %t35, %L7 ], [ 0, %L8 ]
  %t37 = icmp ne i64 %t36, 0
  br i1 %t37, label %L10, label %L11
L10:
  %t38 = load ptr, ptr %t23
  %t39 = load ptr, ptr %t26
  call void @register_typedef(ptr %t0, ptr %t38, ptr %t39)
  %t41 = alloca ptr
  %t42 = load i64, ptr %t1
  %t43 = call ptr @node_new(i64 3, i64 %t42)
  store ptr %t43, ptr %t41
  %t44 = load ptr, ptr %t23
  %t45 = load ptr, ptr %t41
  %t46 = getelementptr i8, ptr %t45, i64 0
  store ptr %t44, ptr %t46
  %t47 = load ptr, ptr %t26
  %t48 = load ptr, ptr %t41
  %t49 = getelementptr i8, ptr %t48, i64 0
  store ptr %t47, ptr %t49
  %t50 = load ptr, ptr %t20
  %t51 = load ptr, ptr %t41
  call void @node_add_child(ptr %t50, ptr %t51)
  br label %L12
L11:
  %t53 = alloca ptr
  %t54 = load i64, ptr %t1
  %t55 = call ptr @node_new(i64 2, i64 %t54)
  store ptr %t55, ptr %t53
  %t56 = load ptr, ptr %t23
  %t57 = load ptr, ptr %t53
  %t58 = getelementptr i8, ptr %t57, i64 0
  store ptr %t56, ptr %t58
  %t59 = load ptr, ptr %t26
  %t60 = load ptr, ptr %t53
  %t61 = getelementptr i8, ptr %t60, i64 0
  store ptr %t59, ptr %t61
  %t62 = load i64, ptr %t7
  %t63 = load ptr, ptr %t53
  %t64 = getelementptr i8, ptr %t63, i64 0
  %t65 = sext i32 %t62 to i64
  store i64 %t65, ptr %t64
  %t66 = load i64, ptr %t9
  %t67 = load ptr, ptr %t53
  %t68 = getelementptr i8, ptr %t67, i64 0
  %t69 = sext i32 %t66 to i64
  store i64 %t69, ptr %t68
  %t70 = call i32 @match(ptr %t0, i64 55)
  %t71 = sext i32 %t70 to i64
  %t72 = icmp ne i64 %t71, 0
  br i1 %t72, label %L13, label %L15
L13:
  %t73 = load ptr, ptr %t53
  %t74 = call ptr @parse_initializer(ptr %t0)
  call void @node_add_child(ptr %t73, ptr %t74)
  br label %L15
L15:
  %t76 = load ptr, ptr %t20
  %t77 = load ptr, ptr %t53
  call void @node_add_child(ptr %t76, ptr %t77)
  br label %L12
L12:
  br label %L5
L5:
  %t79 = call i32 @match(ptr %t0, i64 79)
  %t80 = sext i32 %t79 to i64
  %t81 = icmp ne i64 %t80, 0
  br i1 %t81, label %L4, label %L6
L6:
  call void @expect(ptr %t0, i64 78)
  %t83 = load ptr, ptr %t20
  %t84 = getelementptr i8, ptr %t83, i64 0
  %t85 = load i64, ptr %t84
  %t87 = sext i32 1 to i64
  %t86 = icmp eq i64 %t85, %t87
  %t88 = zext i1 %t86 to i64
  %t89 = icmp ne i64 %t88, 0
  br i1 %t89, label %L16, label %L18
L16:
  %t90 = alloca ptr
  %t91 = load ptr, ptr %t20
  %t92 = getelementptr i8, ptr %t91, i64 0
  %t93 = load i64, ptr %t92
  %t94 = inttoptr i64 %t93 to ptr
  %t95 = sext i32 0 to i64
  %t96 = getelementptr ptr, ptr %t94, i64 %t95
  %t97 = load ptr, ptr %t96
  store ptr %t97, ptr %t90
  %t98 = load ptr, ptr %t20
  %t99 = getelementptr i8, ptr %t98, i64 0
  %t100 = sext i32 0 to i64
  store i64 %t100, ptr %t99
  %t101 = load ptr, ptr %t20
  %t102 = getelementptr i8, ptr %t101, i64 0
  %t103 = load i64, ptr %t102
  call void @free(i64 %t103)
  %t105 = load ptr, ptr %t20
  call void @free(ptr %t105)
  %t107 = load ptr, ptr %t90
  ret ptr %t107
L19:
  br label %L18
L18:
  %t108 = load ptr, ptr %t20
  ret ptr %t108
L20:
  ret ptr null
}

define internal ptr @parse_initializer(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  %t5 = call i32 @check(ptr %t0, i64 74)
  %t6 = sext i32 %t5 to i64
  %t8 = icmp eq i64 %t6, 0
  %t7 = zext i1 %t8 to i64
  %t9 = icmp ne i64 %t7, 0
  br i1 %t9, label %L0, label %L2
L0:
  %t10 = call ptr @parse_assign(ptr %t0)
  ret ptr %t10
L3:
  br label %L2
L2:
  call void @advance(ptr %t0)
  %t12 = alloca i64
  %t13 = sext i32 1 to i64
  store i64 %t13, ptr %t12
  br label %L4
L4:
  %t14 = call i32 @check(ptr %t0, i64 81)
  %t15 = sext i32 %t14 to i64
  %t17 = icmp eq i64 %t15, 0
  %t16 = zext i1 %t17 to i64
  %t18 = icmp ne i64 %t16, 0
  br i1 %t18, label %L7, label %L8
L7:
  %t19 = load i64, ptr %t12
  %t21 = sext i32 %t19 to i64
  %t22 = sext i32 0 to i64
  %t20 = icmp sgt i64 %t21, %t22
  %t23 = zext i1 %t20 to i64
  %t24 = icmp ne i64 %t23, 0
  %t25 = zext i1 %t24 to i64
  br label %L9
L8:
  br label %L9
L9:
  %t26 = phi i64 [ %t25, %L7 ], [ 0, %L8 ]
  %t27 = icmp ne i64 %t26, 0
  br i1 %t27, label %L5, label %L6
L5:
  %t28 = call i32 @check(ptr %t0, i64 74)
  %t29 = sext i32 %t28 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L10, label %L11
L10:
  %t31 = load i64, ptr %t12
  %t33 = sext i32 %t31 to i64
  %t32 = add i64 %t33, 1
  store i64 %t32, ptr %t12
  br label %L12
L11:
  %t34 = call i32 @check(ptr %t0, i64 75)
  %t35 = sext i32 %t34 to i64
  %t36 = icmp ne i64 %t35, 0
  br i1 %t36, label %L13, label %L15
L13:
  %t37 = load i64, ptr %t12
  %t39 = sext i32 %t37 to i64
  %t38 = sub i64 %t39, 1
  store i64 %t38, ptr %t12
  %t40 = load i64, ptr %t12
  %t42 = sext i32 %t40 to i64
  %t43 = sext i32 0 to i64
  %t41 = icmp eq i64 %t42, %t43
  %t44 = zext i1 %t41 to i64
  %t45 = icmp ne i64 %t44, 0
  br i1 %t45, label %L16, label %L18
L16:
  br label %L6
L19:
  br label %L18
L18:
  br label %L15
L15:
  br label %L12
L12:
  call void @advance(ptr %t0)
  br label %L4
L6:
  call void @expect(ptr %t0, i64 75)
  %t48 = alloca ptr
  %t49 = load i64, ptr %t1
  %t50 = call ptr @node_new(i64 19, i64 %t49)
  store ptr %t50, ptr %t48
  %t51 = load ptr, ptr %t48
  %t52 = getelementptr i8, ptr %t51, i64 0
  %t53 = sext i32 0 to i64
  store i64 %t53, ptr %t52
  %t54 = getelementptr [7 x i8], ptr @.str33, i64 0, i64 0
  %t55 = call ptr @strdup(ptr %t54)
  %t56 = load ptr, ptr %t48
  %t57 = getelementptr i8, ptr %t56, i64 0
  store ptr %t55, ptr %t57
  %t58 = load ptr, ptr %t48
  ret ptr %t58
L20:
  ret ptr null
}

define internal ptr @parse_stmt(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  %t5 = call i32 @check(ptr %t0, i64 74)
  %t6 = sext i32 %t5 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = call ptr @parse_block(ptr %t0)
  ret ptr %t8
L3:
  br label %L2
L2:
  %t9 = call i32 @check(ptr %t0, i64 14)
  %t10 = sext i32 %t9 to i64
  %t11 = icmp ne i64 %t10, 0
  br i1 %t11, label %L4, label %L6
L4:
  call void @advance(ptr %t0)
  %t13 = alloca ptr
  %t14 = load i64, ptr %t1
  %t15 = call ptr @node_new(i64 6, i64 %t14)
  store ptr %t15, ptr %t13
  call void @expect(ptr %t0, i64 72)
  %t17 = call ptr @parse_expr(ptr %t0)
  %t18 = load ptr, ptr %t13
  %t19 = getelementptr i8, ptr %t18, i64 0
  store ptr %t17, ptr %t19
  call void @expect(ptr %t0, i64 73)
  %t21 = call ptr @parse_stmt(ptr %t0)
  %t22 = load ptr, ptr %t13
  %t23 = getelementptr i8, ptr %t22, i64 0
  store ptr %t21, ptr %t23
  %t24 = call i32 @match(ptr %t0, i64 15)
  %t25 = sext i32 %t24 to i64
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L7, label %L9
L7:
  %t27 = call ptr @parse_stmt(ptr %t0)
  %t28 = load ptr, ptr %t13
  %t29 = getelementptr i8, ptr %t28, i64 0
  store ptr %t27, ptr %t29
  br label %L9
L9:
  %t30 = load ptr, ptr %t13
  ret ptr %t30
L10:
  br label %L6
L6:
  %t31 = call i32 @check(ptr %t0, i64 16)
  %t32 = sext i32 %t31 to i64
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L11, label %L13
L11:
  call void @advance(ptr %t0)
  %t35 = alloca ptr
  %t36 = load i64, ptr %t1
  %t37 = call ptr @node_new(i64 7, i64 %t36)
  store ptr %t37, ptr %t35
  call void @expect(ptr %t0, i64 72)
  %t39 = call ptr @parse_expr(ptr %t0)
  %t40 = load ptr, ptr %t35
  %t41 = getelementptr i8, ptr %t40, i64 0
  store ptr %t39, ptr %t41
  call void @expect(ptr %t0, i64 73)
  %t43 = call ptr @parse_stmt(ptr %t0)
  %t44 = load ptr, ptr %t35
  %t45 = getelementptr i8, ptr %t44, i64 0
  store ptr %t43, ptr %t45
  %t46 = load ptr, ptr %t35
  ret ptr %t46
L14:
  br label %L13
L13:
  %t47 = call i32 @check(ptr %t0, i64 18)
  %t48 = sext i32 %t47 to i64
  %t49 = icmp ne i64 %t48, 0
  br i1 %t49, label %L15, label %L17
L15:
  call void @advance(ptr %t0)
  %t51 = alloca ptr
  %t52 = load i64, ptr %t1
  %t53 = call ptr @node_new(i64 8, i64 %t52)
  store ptr %t53, ptr %t51
  %t54 = call ptr @parse_stmt(ptr %t0)
  %t55 = load ptr, ptr %t51
  %t56 = getelementptr i8, ptr %t55, i64 0
  store ptr %t54, ptr %t56
  call void @expect(ptr %t0, i64 16)
  call void @expect(ptr %t0, i64 72)
  %t59 = call ptr @parse_expr(ptr %t0)
  %t60 = load ptr, ptr %t51
  %t61 = getelementptr i8, ptr %t60, i64 0
  store ptr %t59, ptr %t61
  call void @expect(ptr %t0, i64 73)
  call void @expect(ptr %t0, i64 78)
  %t64 = load ptr, ptr %t51
  ret ptr %t64
L18:
  br label %L17
L17:
  %t65 = call i32 @check(ptr %t0, i64 17)
  %t66 = sext i32 %t65 to i64
  %t67 = icmp ne i64 %t66, 0
  br i1 %t67, label %L19, label %L21
L19:
  call void @advance(ptr %t0)
  %t69 = alloca ptr
  %t70 = load i64, ptr %t1
  %t71 = call ptr @node_new(i64 9, i64 %t70)
  store ptr %t71, ptr %t69
  call void @expect(ptr %t0, i64 72)
  %t73 = call i32 @check(ptr %t0, i64 78)
  %t74 = sext i32 %t73 to i64
  %t76 = icmp eq i64 %t74, 0
  %t75 = zext i1 %t76 to i64
  %t77 = icmp ne i64 %t75, 0
  br i1 %t77, label %L22, label %L23
L22:
  %t78 = call i32 @is_type_start(ptr %t0)
  %t79 = sext i32 %t78 to i64
  %t80 = icmp ne i64 %t79, 0
  br i1 %t80, label %L25, label %L26
L25:
  %t81 = call ptr @parse_local_decl(ptr %t0)
  %t82 = load ptr, ptr %t69
  %t83 = getelementptr i8, ptr %t82, i64 0
  store ptr %t81, ptr %t83
  br label %L27
L26:
  %t84 = load i64, ptr %t1
  %t85 = call ptr @node_new(i64 18, i64 %t84)
  %t86 = load ptr, ptr %t69
  %t87 = getelementptr i8, ptr %t86, i64 0
  store ptr %t85, ptr %t87
  %t88 = load ptr, ptr %t69
  %t89 = getelementptr i8, ptr %t88, i64 0
  %t90 = load i64, ptr %t89
  %t91 = call ptr @parse_expr(ptr %t0)
  call void @node_add_child(i64 %t90, ptr %t91)
  call void @expect(ptr %t0, i64 78)
  br label %L27
L27:
  br label %L24
L23:
  call void @advance(ptr %t0)
  br label %L24
L24:
  %t95 = call i32 @check(ptr %t0, i64 78)
  %t96 = sext i32 %t95 to i64
  %t98 = icmp eq i64 %t96, 0
  %t97 = zext i1 %t98 to i64
  %t99 = icmp ne i64 %t97, 0
  br i1 %t99, label %L28, label %L30
L28:
  %t100 = call ptr @parse_expr(ptr %t0)
  %t101 = load ptr, ptr %t69
  %t102 = getelementptr i8, ptr %t101, i64 0
  store ptr %t100, ptr %t102
  br label %L30
L30:
  call void @expect(ptr %t0, i64 78)
  %t104 = call i32 @check(ptr %t0, i64 73)
  %t105 = sext i32 %t104 to i64
  %t107 = icmp eq i64 %t105, 0
  %t106 = zext i1 %t107 to i64
  %t108 = icmp ne i64 %t106, 0
  br i1 %t108, label %L31, label %L33
L31:
  %t109 = call ptr @parse_expr(ptr %t0)
  %t110 = load ptr, ptr %t69
  %t111 = getelementptr i8, ptr %t110, i64 0
  store ptr %t109, ptr %t111
  br label %L33
L33:
  call void @expect(ptr %t0, i64 73)
  %t113 = call ptr @parse_stmt(ptr %t0)
  %t114 = load ptr, ptr %t69
  %t115 = getelementptr i8, ptr %t114, i64 0
  store ptr %t113, ptr %t115
  %t116 = load ptr, ptr %t69
  ret ptr %t116
L34:
  br label %L21
L21:
  %t117 = call i32 @check(ptr %t0, i64 19)
  %t118 = sext i32 %t117 to i64
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L35, label %L37
L35:
  call void @advance(ptr %t0)
  %t121 = alloca ptr
  %t122 = load i64, ptr %t1
  %t123 = call ptr @node_new(i64 10, i64 %t122)
  store ptr %t123, ptr %t121
  %t124 = call i32 @check(ptr %t0, i64 78)
  %t125 = sext i32 %t124 to i64
  %t127 = icmp eq i64 %t125, 0
  %t126 = zext i1 %t127 to i64
  %t128 = icmp ne i64 %t126, 0
  br i1 %t128, label %L38, label %L40
L38:
  %t129 = call ptr @parse_expr(ptr %t0)
  %t130 = load ptr, ptr %t121
  %t131 = getelementptr i8, ptr %t130, i64 0
  store ptr %t129, ptr %t131
  br label %L40
L40:
  call void @expect(ptr %t0, i64 78)
  %t133 = load ptr, ptr %t121
  ret ptr %t133
L41:
  br label %L37
L37:
  %t134 = call i32 @check(ptr %t0, i64 20)
  %t135 = sext i32 %t134 to i64
  %t136 = icmp ne i64 %t135, 0
  br i1 %t136, label %L42, label %L44
L42:
  call void @advance(ptr %t0)
  call void @expect(ptr %t0, i64 78)
  %t139 = load i64, ptr %t1
  %t140 = call ptr @node_new(i64 11, i64 %t139)
  ret ptr %t140
L45:
  br label %L44
L44:
  %t141 = call i32 @check(ptr %t0, i64 21)
  %t142 = sext i32 %t141 to i64
  %t143 = icmp ne i64 %t142, 0
  br i1 %t143, label %L46, label %L48
L46:
  call void @advance(ptr %t0)
  call void @expect(ptr %t0, i64 78)
  %t146 = load i64, ptr %t1
  %t147 = call ptr @node_new(i64 12, i64 %t146)
  ret ptr %t147
L49:
  br label %L48
L48:
  %t148 = call i32 @check(ptr %t0, i64 22)
  %t149 = sext i32 %t148 to i64
  %t150 = icmp ne i64 %t149, 0
  br i1 %t150, label %L50, label %L52
L50:
  call void @advance(ptr %t0)
  %t152 = alloca ptr
  %t153 = load i64, ptr %t1
  %t154 = call ptr @node_new(i64 13, i64 %t153)
  store ptr %t154, ptr %t152
  call void @expect(ptr %t0, i64 72)
  %t156 = call ptr @parse_expr(ptr %t0)
  %t157 = load ptr, ptr %t152
  %t158 = getelementptr i8, ptr %t157, i64 0
  store ptr %t156, ptr %t158
  call void @expect(ptr %t0, i64 73)
  %t160 = call ptr @parse_stmt(ptr %t0)
  %t161 = load ptr, ptr %t152
  %t162 = getelementptr i8, ptr %t161, i64 0
  store ptr %t160, ptr %t162
  %t163 = load ptr, ptr %t152
  ret ptr %t163
L53:
  br label %L52
L52:
  %t164 = call i32 @check(ptr %t0, i64 23)
  %t165 = sext i32 %t164 to i64
  %t166 = icmp ne i64 %t165, 0
  br i1 %t166, label %L54, label %L56
L54:
  call void @advance(ptr %t0)
  %t168 = alloca ptr
  %t169 = load i64, ptr %t1
  %t170 = call ptr @node_new(i64 14, i64 %t169)
  store ptr %t170, ptr %t168
  %t171 = call ptr @parse_expr(ptr %t0)
  %t172 = load ptr, ptr %t168
  %t173 = getelementptr i8, ptr %t172, i64 0
  store ptr %t171, ptr %t173
  call void @expect(ptr %t0, i64 71)
  %t175 = alloca ptr
  %t176 = load i64, ptr %t1
  %t177 = call ptr @node_new(i64 5, i64 %t176)
  store ptr %t177, ptr %t175
  br label %L57
L57:
  %t178 = call i32 @check(ptr %t0, i64 23)
  %t179 = sext i32 %t178 to i64
  %t181 = icmp eq i64 %t179, 0
  %t180 = zext i1 %t181 to i64
  %t182 = icmp ne i64 %t180, 0
  br i1 %t182, label %L60, label %L61
L60:
  %t183 = call i32 @check(ptr %t0, i64 24)
  %t184 = sext i32 %t183 to i64
  %t186 = icmp eq i64 %t184, 0
  %t185 = zext i1 %t186 to i64
  %t187 = icmp ne i64 %t185, 0
  %t188 = zext i1 %t187 to i64
  br label %L62
L61:
  br label %L62
L62:
  %t189 = phi i64 [ %t188, %L60 ], [ 0, %L61 ]
  %t190 = icmp ne i64 %t189, 0
  br i1 %t190, label %L63, label %L64
L63:
  %t191 = call i32 @check(ptr %t0, i64 75)
  %t192 = sext i32 %t191 to i64
  %t194 = icmp eq i64 %t192, 0
  %t193 = zext i1 %t194 to i64
  %t195 = icmp ne i64 %t193, 0
  %t196 = zext i1 %t195 to i64
  br label %L65
L64:
  br label %L65
L65:
  %t197 = phi i64 [ %t196, %L63 ], [ 0, %L64 ]
  %t198 = icmp ne i64 %t197, 0
  br i1 %t198, label %L66, label %L67
L66:
  %t199 = call i32 @check(ptr %t0, i64 81)
  %t200 = sext i32 %t199 to i64
  %t202 = icmp eq i64 %t200, 0
  %t201 = zext i1 %t202 to i64
  %t203 = icmp ne i64 %t201, 0
  %t204 = zext i1 %t203 to i64
  br label %L68
L67:
  br label %L68
L68:
  %t205 = phi i64 [ %t204, %L66 ], [ 0, %L67 ]
  %t206 = icmp ne i64 %t205, 0
  br i1 %t206, label %L58, label %L59
L58:
  %t207 = load ptr, ptr %t175
  %t208 = call ptr @parse_stmt(ptr %t0)
  call void @node_add_child(ptr %t207, ptr %t208)
  br label %L57
L59:
  %t210 = load ptr, ptr %t168
  %t211 = load ptr, ptr %t175
  call void @node_add_child(ptr %t210, ptr %t211)
  %t213 = load ptr, ptr %t168
  ret ptr %t213
L69:
  br label %L56
L56:
  %t214 = call i32 @check(ptr %t0, i64 24)
  %t215 = sext i32 %t214 to i64
  %t216 = icmp ne i64 %t215, 0
  br i1 %t216, label %L70, label %L72
L70:
  call void @advance(ptr %t0)
  call void @expect(ptr %t0, i64 71)
  %t219 = alloca ptr
  %t220 = load i64, ptr %t1
  %t221 = call ptr @node_new(i64 15, i64 %t220)
  store ptr %t221, ptr %t219
  %t222 = alloca ptr
  %t223 = load i64, ptr %t1
  %t224 = call ptr @node_new(i64 5, i64 %t223)
  store ptr %t224, ptr %t222
  br label %L73
L73:
  %t225 = call i32 @check(ptr %t0, i64 23)
  %t226 = sext i32 %t225 to i64
  %t228 = icmp eq i64 %t226, 0
  %t227 = zext i1 %t228 to i64
  %t229 = icmp ne i64 %t227, 0
  br i1 %t229, label %L76, label %L77
L76:
  %t230 = call i32 @check(ptr %t0, i64 24)
  %t231 = sext i32 %t230 to i64
  %t233 = icmp eq i64 %t231, 0
  %t232 = zext i1 %t233 to i64
  %t234 = icmp ne i64 %t232, 0
  %t235 = zext i1 %t234 to i64
  br label %L78
L77:
  br label %L78
L78:
  %t236 = phi i64 [ %t235, %L76 ], [ 0, %L77 ]
  %t237 = icmp ne i64 %t236, 0
  br i1 %t237, label %L79, label %L80
L79:
  %t238 = call i32 @check(ptr %t0, i64 75)
  %t239 = sext i32 %t238 to i64
  %t241 = icmp eq i64 %t239, 0
  %t240 = zext i1 %t241 to i64
  %t242 = icmp ne i64 %t240, 0
  %t243 = zext i1 %t242 to i64
  br label %L81
L80:
  br label %L81
L81:
  %t244 = phi i64 [ %t243, %L79 ], [ 0, %L80 ]
  %t245 = icmp ne i64 %t244, 0
  br i1 %t245, label %L82, label %L83
L82:
  %t246 = call i32 @check(ptr %t0, i64 81)
  %t247 = sext i32 %t246 to i64
  %t249 = icmp eq i64 %t247, 0
  %t248 = zext i1 %t249 to i64
  %t250 = icmp ne i64 %t248, 0
  %t251 = zext i1 %t250 to i64
  br label %L84
L83:
  br label %L84
L84:
  %t252 = phi i64 [ %t251, %L82 ], [ 0, %L83 ]
  %t253 = icmp ne i64 %t252, 0
  br i1 %t253, label %L74, label %L75
L74:
  %t254 = load ptr, ptr %t222
  %t255 = call ptr @parse_stmt(ptr %t0)
  call void @node_add_child(ptr %t254, ptr %t255)
  br label %L73
L75:
  %t257 = load ptr, ptr %t219
  %t258 = load ptr, ptr %t222
  call void @node_add_child(ptr %t257, ptr %t258)
  %t260 = load ptr, ptr %t219
  ret ptr %t260
L85:
  br label %L72
L72:
  %t261 = call i32 @check(ptr %t0, i64 25)
  %t262 = sext i32 %t261 to i64
  %t263 = icmp ne i64 %t262, 0
  br i1 %t263, label %L86, label %L88
L86:
  call void @advance(ptr %t0)
  %t265 = alloca ptr
  %t266 = load i64, ptr %t1
  %t267 = call ptr @node_new(i64 17, i64 %t266)
  store ptr %t267, ptr %t265
  %t268 = call ptr @expect_ident(ptr %t0)
  %t269 = load ptr, ptr %t265
  %t270 = getelementptr i8, ptr %t269, i64 0
  store ptr %t268, ptr %t270
  call void @expect(ptr %t0, i64 78)
  %t272 = load ptr, ptr %t265
  ret ptr %t272
L89:
  br label %L88
L88:
  %t273 = call i32 @check(ptr %t0, i64 4)
  %t274 = sext i32 %t273 to i64
  %t275 = icmp ne i64 %t274, 0
  br i1 %t275, label %L90, label %L91
L90:
  %t276 = call i64 @peek(ptr %t0)
  %t277 = inttoptr i64 %t276 to ptr
  %t278 = getelementptr i8, ptr %t277, i64 0
  %t279 = load i64, ptr %t278
  %t281 = sext i32 71 to i64
  %t280 = icmp eq i64 %t279, %t281
  %t282 = zext i1 %t280 to i64
  %t283 = icmp ne i64 %t282, 0
  %t284 = zext i1 %t283 to i64
  br label %L92
L91:
  br label %L92
L92:
  %t285 = phi i64 [ %t284, %L90 ], [ 0, %L91 ]
  %t286 = icmp ne i64 %t285, 0
  br i1 %t286, label %L93, label %L95
L93:
  %t287 = alloca ptr
  %t288 = load i64, ptr %t1
  %t289 = call ptr @node_new(i64 16, i64 %t288)
  store ptr %t289, ptr %t287
  %t290 = getelementptr i8, ptr %t0, i64 0
  %t291 = getelementptr i8, ptr %t290, i64 0
  %t292 = load i64, ptr %t291
  %t293 = call ptr @strdup(i64 %t292)
  %t294 = load ptr, ptr %t287
  %t295 = getelementptr i8, ptr %t294, i64 0
  store ptr %t293, ptr %t295
  call void @advance(ptr %t0)
  call void @advance(ptr %t0)
  %t298 = load ptr, ptr %t287
  %t299 = call ptr @parse_stmt(ptr %t0)
  call void @node_add_child(ptr %t298, ptr %t299)
  %t301 = load ptr, ptr %t287
  ret ptr %t301
L96:
  br label %L95
L95:
  %t302 = call i32 @is_type_start(ptr %t0)
  %t303 = sext i32 %t302 to i64
  %t304 = icmp ne i64 %t303, 0
  br i1 %t304, label %L97, label %L99
L97:
  %t305 = call ptr @parse_local_decl(ptr %t0)
  ret ptr %t305
L100:
  br label %L99
L99:
  %t306 = call i32 @check(ptr %t0, i64 78)
  %t307 = sext i32 %t306 to i64
  %t308 = icmp ne i64 %t307, 0
  br i1 %t308, label %L101, label %L103
L101:
  call void @advance(ptr %t0)
  %t310 = load i64, ptr %t1
  %t311 = call ptr @node_new(i64 5, i64 %t310)
  ret ptr %t311
L104:
  br label %L103
L103:
  %t312 = alloca ptr
  %t313 = load i64, ptr %t1
  %t314 = call ptr @node_new(i64 18, i64 %t313)
  store ptr %t314, ptr %t312
  %t315 = load ptr, ptr %t312
  %t316 = call ptr @parse_expr(ptr %t0)
  call void @node_add_child(ptr %t315, ptr %t316)
  call void @expect(ptr %t0, i64 78)
  %t319 = load ptr, ptr %t312
  ret ptr %t319
L105:
  ret ptr null
}

define internal ptr @parse_block(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  call void @expect(ptr %t0, i64 74)
  %t6 = alloca ptr
  %t7 = load i64, ptr %t1
  %t8 = call ptr @node_new(i64 5, i64 %t7)
  store ptr %t8, ptr %t6
  %t9 = load ptr, ptr %t6
  %t10 = getelementptr i8, ptr %t9, i64 0
  %t11 = sext i32 1 to i64
  store i64 %t11, ptr %t10
  br label %L0
L0:
  %t12 = call i32 @check(ptr %t0, i64 75)
  %t13 = sext i32 %t12 to i64
  %t15 = icmp eq i64 %t13, 0
  %t14 = zext i1 %t15 to i64
  %t16 = icmp ne i64 %t14, 0
  br i1 %t16, label %L3, label %L4
L3:
  %t17 = call i32 @check(ptr %t0, i64 81)
  %t18 = sext i32 %t17 to i64
  %t20 = icmp eq i64 %t18, 0
  %t19 = zext i1 %t20 to i64
  %t21 = icmp ne i64 %t19, 0
  %t22 = zext i1 %t21 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t23 = phi i64 [ %t22, %L3 ], [ 0, %L4 ]
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L1, label %L2
L1:
  %t25 = load ptr, ptr %t6
  %t26 = call ptr @parse_stmt(ptr %t0)
  call void @node_add_child(ptr %t25, ptr %t26)
  br label %L0
L2:
  call void @expect(ptr %t0, i64 75)
  %t29 = load ptr, ptr %t6
  ret ptr %t29
L6:
  ret ptr null
}

define internal ptr @parse_toplevel(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  call void @skip_gcc_extension(ptr %t0)
  %t6 = alloca i64
  %t7 = sext i32 0 to i64
  store i64 %t7, ptr %t6
  %t8 = alloca i64
  %t9 = sext i32 0 to i64
  store i64 %t9, ptr %t8
  %t10 = alloca i64
  %t11 = sext i32 0 to i64
  store i64 %t11, ptr %t10
  %t12 = alloca ptr
  %t13 = call ptr @parse_type_specifier(ptr %t0, ptr %t6, ptr %t8, ptr %t10)
  store ptr %t13, ptr %t12
  %t14 = load ptr, ptr %t12
  %t16 = ptrtoint ptr %t14 to i64
  %t17 = icmp eq i64 %t16, 0
  %t15 = zext i1 %t17 to i64
  %t18 = icmp ne i64 %t15, 0
  br i1 %t18, label %L0, label %L2
L0:
  %t19 = getelementptr [21 x i8], ptr @.str34, i64 0, i64 0
  call void @p_error(ptr %t0, ptr %t19)
  %t22 = sext i32 0 to i64
  %t21 = inttoptr i64 %t22 to ptr
  ret ptr %t21
L3:
  br label %L2
L2:
  %t23 = call i32 @check(ptr %t0, i64 78)
  %t24 = sext i32 %t23 to i64
  %t25 = icmp ne i64 %t24, 0
  br i1 %t25, label %L4, label %L6
L4:
  call void @advance(ptr %t0)
  %t27 = load i64, ptr %t1
  %t28 = call ptr @node_new(i64 5, i64 %t27)
  ret ptr %t28
L7:
  br label %L6
L6:
  %t29 = alloca ptr
  %t31 = sext i32 0 to i64
  %t30 = inttoptr i64 %t31 to ptr
  store ptr %t30, ptr %t29
  %t32 = alloca ptr
  %t33 = load ptr, ptr %t12
  %t34 = call ptr @parse_declarator(ptr %t0, ptr %t33, ptr %t29)
  store ptr %t34, ptr %t32
  call void @skip_gcc_extension(ptr %t0)
  %t36 = load i64, ptr %t6
  %t38 = sext i32 %t36 to i64
  %t37 = icmp ne i64 %t38, 0
  br i1 %t37, label %L8, label %L10
L8:
  %t39 = load ptr, ptr %t29
  %t40 = icmp ne ptr %t39, null
  br i1 %t40, label %L11, label %L13
L11:
  %t41 = load ptr, ptr %t29
  %t42 = load ptr, ptr %t32
  call void @register_typedef(ptr %t0, ptr %t41, ptr %t42)
  br label %L13
L13:
  %t44 = alloca ptr
  %t45 = load i64, ptr %t1
  %t46 = call ptr @node_new(i64 3, i64 %t45)
  store ptr %t46, ptr %t44
  %t47 = load ptr, ptr %t29
  %t48 = load ptr, ptr %t44
  %t49 = getelementptr i8, ptr %t48, i64 0
  store ptr %t47, ptr %t49
  %t50 = load ptr, ptr %t32
  %t51 = load ptr, ptr %t44
  %t52 = getelementptr i8, ptr %t51, i64 0
  store ptr %t50, ptr %t52
  call void @expect(ptr %t0, i64 78)
  %t54 = load ptr, ptr %t44
  ret ptr %t54
L14:
  br label %L10
L10:
  %t55 = load ptr, ptr %t32
  %t56 = getelementptr i8, ptr %t55, i64 0
  %t57 = load i64, ptr %t56
  %t59 = sext i32 17 to i64
  %t58 = icmp eq i64 %t57, %t59
  %t60 = zext i1 %t58 to i64
  %t61 = icmp ne i64 %t60, 0
  br i1 %t61, label %L15, label %L16
L15:
  %t62 = call i32 @check(ptr %t0, i64 74)
  %t63 = sext i32 %t62 to i64
  %t64 = icmp ne i64 %t63, 0
  %t65 = zext i1 %t64 to i64
  br label %L17
L16:
  br label %L17
L17:
  %t66 = phi i64 [ %t65, %L15 ], [ 0, %L16 ]
  %t67 = icmp ne i64 %t66, 0
  br i1 %t67, label %L18, label %L20
L18:
  %t68 = alloca ptr
  %t69 = load i64, ptr %t1
  %t70 = call ptr @node_new(i64 1, i64 %t69)
  store ptr %t70, ptr %t68
  %t71 = load ptr, ptr %t29
  %t72 = load ptr, ptr %t68
  %t73 = getelementptr i8, ptr %t72, i64 0
  store ptr %t71, ptr %t73
  %t74 = load ptr, ptr %t32
  %t75 = load ptr, ptr %t68
  %t76 = getelementptr i8, ptr %t75, i64 0
  store ptr %t74, ptr %t76
  %t77 = load i64, ptr %t8
  %t78 = load ptr, ptr %t68
  %t79 = getelementptr i8, ptr %t78, i64 0
  %t80 = sext i32 %t77 to i64
  store i64 %t80, ptr %t79
  %t81 = load i64, ptr %t10
  %t82 = load ptr, ptr %t68
  %t83 = getelementptr i8, ptr %t82, i64 0
  %t84 = sext i32 %t81 to i64
  store i64 %t84, ptr %t83
  %t85 = load ptr, ptr %t32
  %t86 = getelementptr i8, ptr %t85, i64 0
  %t87 = load i64, ptr %t86
  %t89 = sext i32 8 to i64
  %t88 = mul i64 %t87, %t89
  %t90 = call ptr @malloc(i64 %t88)
  %t91 = load ptr, ptr %t68
  %t92 = getelementptr i8, ptr %t91, i64 0
  store ptr %t90, ptr %t92
  %t93 = alloca i64
  %t94 = sext i32 0 to i64
  store i64 %t94, ptr %t93
  br label %L21
L21:
  %t95 = load i64, ptr %t93
  %t96 = load ptr, ptr %t32
  %t97 = getelementptr i8, ptr %t96, i64 0
  %t98 = load i64, ptr %t97
  %t100 = sext i32 %t95 to i64
  %t99 = icmp slt i64 %t100, %t98
  %t101 = zext i1 %t99 to i64
  %t102 = icmp ne i64 %t101, 0
  br i1 %t102, label %L22, label %L24
L22:
  %t103 = load ptr, ptr %t32
  %t104 = getelementptr i8, ptr %t103, i64 0
  %t105 = load i64, ptr %t104
  %t106 = load i64, ptr %t93
  %t108 = inttoptr i64 %t105 to ptr
  %t109 = sext i32 %t106 to i64
  %t107 = getelementptr ptr, ptr %t108, i64 %t109
  %t110 = getelementptr i8, ptr %t107, i64 0
  %t111 = load i64, ptr %t110
  %t112 = icmp ne i64 %t111, 0
  br i1 %t112, label %L25, label %L26
L25:
  %t113 = load ptr, ptr %t32
  %t114 = getelementptr i8, ptr %t113, i64 0
  %t115 = load i64, ptr %t114
  %t116 = load i64, ptr %t93
  %t118 = inttoptr i64 %t115 to ptr
  %t119 = sext i32 %t116 to i64
  %t117 = getelementptr ptr, ptr %t118, i64 %t119
  %t120 = getelementptr i8, ptr %t117, i64 0
  %t121 = load i64, ptr %t120
  %t122 = call ptr @strdup(i64 %t121)
  %t123 = ptrtoint ptr %t122 to i64
  br label %L27
L26:
  %t125 = sext i32 0 to i64
  %t124 = inttoptr i64 %t125 to ptr
  %t126 = ptrtoint ptr %t124 to i64
  br label %L27
L27:
  %t127 = phi i64 [ %t123, %L25 ], [ %t126, %L26 ]
  %t128 = load ptr, ptr %t68
  %t129 = getelementptr i8, ptr %t128, i64 0
  %t130 = load i64, ptr %t129
  %t131 = load i64, ptr %t93
  %t133 = inttoptr i64 %t130 to ptr
  %t134 = sext i32 %t131 to i64
  %t132 = getelementptr ptr, ptr %t133, i64 %t134
  store i64 %t127, ptr %t132
  br label %L23
L23:
  %t135 = load i64, ptr %t93
  %t137 = sext i32 %t135 to i64
  %t136 = add i64 %t137, 1
  store i64 %t136, ptr %t93
  br label %L21
L24:
  %t138 = call ptr @parse_block(ptr %t0)
  %t139 = load ptr, ptr %t68
  %t140 = getelementptr i8, ptr %t139, i64 0
  store ptr %t138, ptr %t140
  %t141 = load ptr, ptr %t68
  ret ptr %t141
L28:
  br label %L20
L20:
  %t142 = alloca ptr
  %t143 = load i64, ptr %t1
  %t144 = call ptr @node_new(i64 2, i64 %t143)
  store ptr %t144, ptr %t142
  %t145 = load ptr, ptr %t29
  %t146 = load ptr, ptr %t142
  %t147 = getelementptr i8, ptr %t146, i64 0
  store ptr %t145, ptr %t147
  %t148 = load ptr, ptr %t32
  %t149 = load ptr, ptr %t142
  %t150 = getelementptr i8, ptr %t149, i64 0
  store ptr %t148, ptr %t150
  %t151 = load ptr, ptr %t142
  %t152 = getelementptr i8, ptr %t151, i64 0
  %t153 = sext i32 1 to i64
  store i64 %t153, ptr %t152
  %t154 = load i64, ptr %t8
  %t155 = load ptr, ptr %t142
  %t156 = getelementptr i8, ptr %t155, i64 0
  %t157 = sext i32 %t154 to i64
  store i64 %t157, ptr %t156
  %t158 = load i64, ptr %t10
  %t159 = load ptr, ptr %t142
  %t160 = getelementptr i8, ptr %t159, i64 0
  %t161 = sext i32 %t158 to i64
  store i64 %t161, ptr %t160
  %t162 = call i32 @match(ptr %t0, i64 55)
  %t163 = sext i32 %t162 to i64
  %t164 = icmp ne i64 %t163, 0
  br i1 %t164, label %L29, label %L31
L29:
  %t165 = load ptr, ptr %t142
  %t166 = call ptr @parse_initializer(ptr %t0)
  call void @node_add_child(ptr %t165, ptr %t166)
  br label %L31
L31:
  br label %L32
L32:
  %t168 = call i32 @match(ptr %t0, i64 79)
  %t169 = sext i32 %t168 to i64
  %t170 = icmp ne i64 %t169, 0
  br i1 %t170, label %L33, label %L34
L33:
  %t171 = alloca ptr
  %t173 = sext i32 0 to i64
  %t172 = inttoptr i64 %t173 to ptr
  store ptr %t172, ptr %t171
  %t174 = alloca ptr
  %t175 = load ptr, ptr %t12
  %t176 = call ptr @parse_declarator(ptr %t0, ptr %t175, ptr %t171)
  store ptr %t176, ptr %t174
  %t177 = alloca ptr
  %t178 = load i64, ptr %t1
  %t179 = call ptr @node_new(i64 2, i64 %t178)
  store ptr %t179, ptr %t177
  %t180 = load ptr, ptr %t171
  %t181 = load ptr, ptr %t177
  %t182 = getelementptr i8, ptr %t181, i64 0
  store ptr %t180, ptr %t182
  %t183 = load ptr, ptr %t174
  %t184 = load ptr, ptr %t177
  %t185 = getelementptr i8, ptr %t184, i64 0
  store ptr %t183, ptr %t185
  %t186 = load ptr, ptr %t177
  %t187 = getelementptr i8, ptr %t186, i64 0
  %t188 = sext i32 1 to i64
  store i64 %t188, ptr %t187
  %t189 = call i32 @match(ptr %t0, i64 55)
  %t190 = sext i32 %t189 to i64
  %t191 = icmp ne i64 %t190, 0
  br i1 %t191, label %L35, label %L37
L35:
  %t192 = load ptr, ptr %t177
  %t193 = call ptr @parse_initializer(ptr %t0)
  call void @node_add_child(ptr %t192, ptr %t193)
  br label %L37
L37:
  %t195 = load ptr, ptr %t142
  %t196 = load ptr, ptr %t177
  call void @node_add_child(ptr %t195, ptr %t196)
  br label %L32
L34:
  call void @expect(ptr %t0, i64 78)
  %t199 = load ptr, ptr %t142
  ret ptr %t199
L38:
  ret ptr null
}

define dso_local ptr @parser_new(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @calloc(i64 1, i64 0)
  store ptr %t2, ptr %t1
  %t3 = load ptr, ptr %t1
  %t5 = ptrtoint ptr %t3 to i64
  %t6 = icmp eq i64 %t5, 0
  %t4 = zext i1 %t6 to i64
  %t7 = icmp ne i64 %t4, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = getelementptr [7 x i8], ptr @.str35, i64 0, i64 0
  call void @perror(ptr %t8)
  call void @exit(i64 1)
  br label %L2
L2:
  %t11 = load ptr, ptr %t1
  %t12 = getelementptr i8, ptr %t11, i64 0
  store ptr %t0, ptr %t12
  %t13 = call i64 @lexer_next(ptr %t0)
  %t14 = load ptr, ptr %t1
  %t15 = getelementptr i8, ptr %t14, i64 0
  store i64 %t13, ptr %t15
  %t16 = call ptr @calloc(i64 512, i64 8)
  %t17 = load ptr, ptr %t1
  %t18 = getelementptr i8, ptr %t17, i64 0
  store ptr %t16, ptr %t18
  %t19 = call ptr @calloc(i64 1024, i64 8)
  %t20 = load ptr, ptr %t1
  %t21 = getelementptr i8, ptr %t20, i64 0
  store ptr %t19, ptr %t21
  %t22 = call ptr @calloc(i64 256, i64 8)
  %t23 = load ptr, ptr %t1
  %t24 = getelementptr i8, ptr %t23, i64 0
  store ptr %t22, ptr %t24
  %t25 = alloca i64
  %t26 = sext i32 0 to i64
  store i64 %t26, ptr %t25
  br label %L3
L3:
  %t27 = load i64, ptr %t25
  %t28 = call ptr @__c0c_get_td_name(i64 %t27)
  %t29 = icmp ne ptr %t28, null
  br i1 %t29, label %L4, label %L6
L4:
  %t30 = alloca ptr
  %t31 = load i64, ptr %t25
  %t32 = call i64 @__c0c_get_td_kind(i64 %t31)
  %t33 = call ptr @type_new(i64 %t32)
  store ptr %t33, ptr %t30
  %t34 = load ptr, ptr %t1
  %t35 = load i64, ptr %t25
  %t36 = call ptr @__c0c_get_td_name(i64 %t35)
  %t37 = load ptr, ptr %t30
  call void @register_typedef(ptr %t34, ptr %t36, ptr %t37)
  br label %L5
L5:
  %t39 = load i64, ptr %t25
  %t41 = sext i32 %t39 to i64
  %t40 = add i64 %t41, 1
  store i64 %t40, ptr %t25
  br label %L3
L6:
  %t42 = load ptr, ptr %t1
  ret ptr %t42
L7:
  ret ptr null
}

define dso_local void @parser_free(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  call void @token_free(i64 %t2)
  %t4 = alloca i64
  %t5 = sext i32 0 to i64
  store i64 %t5, ptr %t4
  br label %L0
L0:
  %t6 = load i64, ptr %t4
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = load i64, ptr %t7
  %t10 = sext i32 %t6 to i64
  %t9 = icmp slt i64 %t10, %t8
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L1, label %L3
L1:
  %t13 = alloca ptr
  %t14 = getelementptr i8, ptr %t0, i64 0
  %t15 = load i64, ptr %t14
  %t16 = load i64, ptr %t4
  %t17 = inttoptr i64 %t15 to ptr
  %t18 = sext i32 %t16 to i64
  %t19 = getelementptr ptr, ptr %t17, i64 %t18
  %t20 = load ptr, ptr %t19
  store ptr %t20, ptr %t13
  %t21 = load ptr, ptr %t13
  %t22 = icmp ne ptr %t21, null
  br i1 %t22, label %L4, label %L6
L4:
  %t23 = load ptr, ptr %t13
  %t24 = getelementptr i8, ptr %t23, i64 0
  %t25 = load i64, ptr %t24
  call void @free(i64 %t25)
  %t27 = load ptr, ptr %t13
  call void @free(ptr %t27)
  br label %L6
L6:
  br label %L2
L2:
  %t29 = load i64, ptr %t4
  %t31 = sext i32 %t29 to i64
  %t30 = add i64 %t31, 1
  store i64 %t30, ptr %t4
  br label %L0
L3:
  %t32 = getelementptr i8, ptr %t0, i64 0
  %t33 = load i64, ptr %t32
  call void @free(i64 %t33)
  %t35 = alloca i64
  %t36 = sext i32 0 to i64
  store i64 %t36, ptr %t35
  br label %L7
L7:
  %t37 = load i64, ptr %t35
  %t38 = getelementptr i8, ptr %t0, i64 0
  %t39 = load i64, ptr %t38
  %t41 = sext i32 %t37 to i64
  %t40 = icmp slt i64 %t41, %t39
  %t42 = zext i1 %t40 to i64
  %t43 = icmp ne i64 %t42, 0
  br i1 %t43, label %L8, label %L10
L8:
  %t44 = alloca ptr
  %t45 = getelementptr i8, ptr %t0, i64 0
  %t46 = load i64, ptr %t45
  %t47 = load i64, ptr %t35
  %t48 = inttoptr i64 %t46 to ptr
  %t49 = sext i32 %t47 to i64
  %t50 = getelementptr ptr, ptr %t48, i64 %t49
  %t51 = load ptr, ptr %t50
  store ptr %t51, ptr %t44
  %t52 = load ptr, ptr %t44
  %t53 = icmp ne ptr %t52, null
  br i1 %t53, label %L11, label %L13
L11:
  %t54 = load ptr, ptr %t44
  %t55 = getelementptr i8, ptr %t54, i64 0
  %t56 = load i64, ptr %t55
  call void @free(i64 %t56)
  %t58 = load ptr, ptr %t44
  call void @free(ptr %t58)
  br label %L13
L13:
  br label %L9
L9:
  %t60 = load i64, ptr %t35
  %t62 = sext i32 %t60 to i64
  %t61 = add i64 %t62, 1
  store i64 %t61, ptr %t35
  br label %L7
L10:
  %t63 = getelementptr i8, ptr %t0, i64 0
  %t64 = load i64, ptr %t63
  call void @free(i64 %t64)
  %t66 = alloca i64
  %t67 = sext i32 0 to i64
  store i64 %t67, ptr %t66
  br label %L14
L14:
  %t68 = load i64, ptr %t66
  %t69 = getelementptr i8, ptr %t0, i64 0
  %t70 = load i64, ptr %t69
  %t72 = sext i32 %t68 to i64
  %t71 = icmp slt i64 %t72, %t70
  %t73 = zext i1 %t71 to i64
  %t74 = icmp ne i64 %t73, 0
  br i1 %t74, label %L15, label %L17
L15:
  %t75 = alloca ptr
  %t76 = getelementptr i8, ptr %t0, i64 0
  %t77 = load i64, ptr %t76
  %t78 = load i64, ptr %t66
  %t79 = inttoptr i64 %t77 to ptr
  %t80 = sext i32 %t78 to i64
  %t81 = getelementptr ptr, ptr %t79, i64 %t80
  %t82 = load ptr, ptr %t81
  store ptr %t82, ptr %t75
  %t83 = load ptr, ptr %t75
  %t84 = icmp ne ptr %t83, null
  br i1 %t84, label %L18, label %L20
L18:
  %t85 = load ptr, ptr %t75
  %t86 = getelementptr i8, ptr %t85, i64 0
  %t87 = load i64, ptr %t86
  call void @free(i64 %t87)
  %t89 = load ptr, ptr %t75
  call void @free(ptr %t89)
  br label %L20
L20:
  br label %L16
L16:
  %t91 = load i64, ptr %t66
  %t93 = sext i32 %t91 to i64
  %t92 = add i64 %t93, 1
  store i64 %t92, ptr %t66
  br label %L14
L17:
  %t94 = getelementptr i8, ptr %t0, i64 0
  %t95 = load i64, ptr %t94
  call void @free(i64 %t95)
  call void @free(ptr %t0)
  ret void
}

define dso_local ptr @parser_parse(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = call ptr @node_new(i64 0, i64 0)
  store ptr %t2, ptr %t1
  br label %L0
L0:
  %t3 = call i32 @check(ptr %t0, i64 81)
  %t4 = sext i32 %t3 to i64
  %t6 = icmp eq i64 %t4, 0
  %t5 = zext i1 %t6 to i64
  %t7 = icmp ne i64 %t5, 0
  br i1 %t7, label %L1, label %L2
L1:
  br label %L3
L3:
  %t8 = call i32 @match(ptr %t0, i64 78)
  %t9 = sext i32 %t8 to i64
  %t10 = icmp ne i64 %t9, 0
  br i1 %t10, label %L4, label %L5
L4:
  br label %L3
L5:
  call void @skip_gcc_extension(ptr %t0)
  %t12 = call i32 @check(ptr %t0, i64 81)
  %t13 = sext i32 %t12 to i64
  %t14 = icmp ne i64 %t13, 0
  br i1 %t14, label %L6, label %L8
L6:
  br label %L2
L9:
  br label %L8
L8:
  %t15 = load ptr, ptr %t1
  %t16 = call ptr @parse_toplevel(ptr %t0)
  call void @node_add_child(ptr %t15, ptr %t16)
  br label %L0
L2:
  %t18 = load ptr, ptr %t1
  ret ptr %t18
L10:
  ret ptr null
}

@.str0 = private unnamed_addr constant [38 x i8] c"parse error (line %d): %s (got '%s')\0A\00"
@.str1 = private unnamed_addr constant [2 x i8] c"?\00"
@.str2 = private unnamed_addr constant [12 x i8] c"expected %s\00"
@.str3 = private unnamed_addr constant [20 x i8] c"expected identifier\00"
@.str4 = private unnamed_addr constant [18 x i8] c"too many typedefs\00"
@.str5 = private unnamed_addr constant [14 x i8] c"__attribute__\00"
@.str6 = private unnamed_addr constant [14 x i8] c"__extension__\00"
@.str7 = private unnamed_addr constant [8 x i8] c"__asm__\00"
@.str8 = private unnamed_addr constant [6 x i8] c"__asm\00"
@.str9 = private unnamed_addr constant [11 x i8] c"__inline__\00"
@.str10 = private unnamed_addr constant [9 x i8] c"__inline\00"
@.str11 = private unnamed_addr constant [13 x i8] c"__volatile__\00"
@.str12 = private unnamed_addr constant [11 x i8] c"__volatile\00"
@.str13 = private unnamed_addr constant [11 x i8] c"__restrict\00"
@.str14 = private unnamed_addr constant [13 x i8] c"__restrict__\00"
@.str15 = private unnamed_addr constant [8 x i8] c"__const\00"
@.str16 = private unnamed_addr constant [10 x i8] c"__const__\00"
@.str17 = private unnamed_addr constant [11 x i8] c"__signed__\00"
@.str18 = private unnamed_addr constant [9 x i8] c"__signed\00"
@.str19 = private unnamed_addr constant [11 x i8] c"__typeof__\00"
@.str20 = private unnamed_addr constant [9 x i8] c"__typeof\00"
@.str21 = private unnamed_addr constant [8 x i8] c"__cdecl\00"
@.str22 = private unnamed_addr constant [11 x i8] c"__declspec\00"
@.str23 = private unnamed_addr constant [14 x i8] c"__forceinline\00"
@.str24 = private unnamed_addr constant [10 x i8] c"__nonnull\00"
@.str25 = private unnamed_addr constant [14 x i8] c"__attribute__\00"
@.str26 = private unnamed_addr constant [8 x i8] c"__asm__\00"
@.str27 = private unnamed_addr constant [6 x i8] c"__asm\00"
@.str28 = private unnamed_addr constant [11 x i8] c"__typeof__\00"
@.str29 = private unnamed_addr constant [9 x i8] c"__typeof\00"
@.str30 = private unnamed_addr constant [11 x i8] c"__declspec\00"
@.str31 = private unnamed_addr constant [8 x i8] c"realloc\00"
@.str32 = private unnamed_addr constant [28 x i8] c"expected primary expression\00"
@.str33 = private unnamed_addr constant [7 x i8] c"{init}\00"
@.str34 = private unnamed_addr constant [21 x i8] c"expected declaration\00"
@.str35 = private unnamed_addr constant [7 x i8] c"calloc\00"
