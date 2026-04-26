; ModuleID = 'codegen.c'
source_filename = "codegen.c"
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
declare ptr @lexer_new(ptr, ptr)
declare void @lexer_free(ptr)
declare i64 @lexer_next(ptr)
declare i64 @lexer_peek(ptr)
declare void @token_free(ptr)
declare ptr @token_type_name(ptr)
@tbuf_idx = internal global i32 zeroinitializer

define internal i32 @new_reg(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = add i64 %t2, 1
  %t4 = getelementptr i8, ptr %t0, i64 0
  store i64 %t3, ptr %t4
  %t5 = trunc i64 %t2 to i32
  ret i32 %t5
L0:
  ret i32 0
}

define internal i32 @new_label(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = add i64 %t2, 1
  %t4 = getelementptr i8, ptr %t0, i64 0
  store i64 %t3, ptr %t4
  %t5 = trunc i64 %t2 to i32
  ret i32 %t5
L0:
  ret i32 0
}

define internal ptr @reg_name(i64 %t0, ptr %t1, ptr %t2) {
entry:
  %t3 = getelementptr [6 x i8], ptr @.str0, i64 0, i64 0
  %t4 = call i32 (ptr, ...) @snprintf(ptr %t1, ptr %t2, ptr %t3, i64 %t0)
  %t5 = sext i32 %t4 to i64
  ret ptr %t1
L0:
  ret ptr null
}

define internal ptr @llvm_type(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = load i64, ptr @tbuf_idx
  %t4 = sext i32 %t2 to i64
  %t3 = add i64 %t4, 1
  store i64 %t3, ptr @tbuf_idx
  %t5 = call ptr @__c0c_get_tbuf(i64 %t2)
  store ptr %t5, ptr %t1
  %t6 = load ptr, ptr %t1
  %t8 = ptrtoint ptr %t6 to i64
  %t9 = icmp eq i64 %t8, 0
  %t7 = zext i1 %t9 to i64
  %t10 = icmp ne i64 %t7, 0
  br i1 %t10, label %L0, label %L2
L0:
  %t11 = call ptr @__c0c_get_tbuf(i64 0)
  store ptr %t11, ptr %t1
  br label %L2
L2:
  %t13 = ptrtoint ptr %t0 to i64
  %t14 = icmp eq i64 %t13, 0
  %t12 = zext i1 %t14 to i64
  %t15 = icmp ne i64 %t12, 0
  br i1 %t15, label %L3, label %L5
L3:
  %t16 = load ptr, ptr %t1
  %t17 = getelementptr [4 x i8], ptr @.str1, i64 0, i64 0
  %t18 = call ptr @strcpy(ptr %t16, ptr %t17)
  %t19 = load ptr, ptr %t1
  ret ptr %t19
L6:
  br label %L5
L5:
  %t20 = getelementptr i8, ptr %t0, i64 0
  %t21 = load i64, ptr %t20
  %t22 = add i64 %t21, 0
  switch i64 %t22, label %L30 [
    i64 0, label %L8
    i64 1, label %L9
    i64 2, label %L10
    i64 3, label %L11
    i64 4, label %L12
    i64 5, label %L13
    i64 6, label %L14
    i64 7, label %L15
    i64 8, label %L16
    i64 20, label %L17
    i64 9, label %L18
    i64 10, label %L19
    i64 11, label %L20
    i64 12, label %L21
    i64 13, label %L22
    i64 14, label %L23
    i64 15, label %L24
    i64 16, label %L25
    i64 17, label %L26
    i64 18, label %L27
    i64 19, label %L28
    i64 21, label %L29
  ]
L8:
  %t23 = load ptr, ptr %t1
  %t24 = getelementptr [5 x i8], ptr @.str2, i64 0, i64 0
  %t25 = call ptr @strcpy(ptr %t23, ptr %t24)
  br label %L7
L31:
  br label %L9
L9:
  %t26 = load ptr, ptr %t1
  %t27 = getelementptr [3 x i8], ptr @.str3, i64 0, i64 0
  %t28 = call ptr @strcpy(ptr %t26, ptr %t27)
  br label %L7
L32:
  br label %L10
L10:
  br label %L11
L11:
  br label %L12
L12:
  %t29 = load ptr, ptr %t1
  %t30 = getelementptr [3 x i8], ptr @.str4, i64 0, i64 0
  %t31 = call ptr @strcpy(ptr %t29, ptr %t30)
  br label %L7
L33:
  br label %L13
L13:
  br label %L14
L14:
  %t32 = load ptr, ptr %t1
  %t33 = getelementptr [4 x i8], ptr @.str5, i64 0, i64 0
  %t34 = call ptr @strcpy(ptr %t32, ptr %t33)
  br label %L7
L34:
  br label %L15
L15:
  br label %L16
L16:
  br label %L17
L17:
  %t35 = load ptr, ptr %t1
  %t36 = getelementptr [4 x i8], ptr @.str6, i64 0, i64 0
  %t37 = call ptr @strcpy(ptr %t35, ptr %t36)
  br label %L7
L35:
  br label %L18
L18:
  br label %L19
L19:
  br label %L20
L20:
  br label %L21
L21:
  %t38 = load ptr, ptr %t1
  %t39 = getelementptr [4 x i8], ptr @.str7, i64 0, i64 0
  %t40 = call ptr @strcpy(ptr %t38, ptr %t39)
  br label %L7
L36:
  br label %L22
L22:
  %t41 = load ptr, ptr %t1
  %t42 = getelementptr [6 x i8], ptr @.str8, i64 0, i64 0
  %t43 = call ptr @strcpy(ptr %t41, ptr %t42)
  br label %L7
L37:
  br label %L23
L23:
  %t44 = load ptr, ptr %t1
  %t45 = getelementptr [7 x i8], ptr @.str9, i64 0, i64 0
  %t46 = call ptr @strcpy(ptr %t44, ptr %t45)
  br label %L7
L38:
  br label %L24
L24:
  %t47 = load ptr, ptr %t1
  %t48 = getelementptr [4 x i8], ptr @.str10, i64 0, i64 0
  %t49 = call ptr @strcpy(ptr %t47, ptr %t48)
  br label %L7
L39:
  br label %L25
L25:
  %t50 = load ptr, ptr %t1
  %t51 = getelementptr [4 x i8], ptr @.str11, i64 0, i64 0
  %t52 = call ptr @strcpy(ptr %t50, ptr %t51)
  br label %L7
L40:
  br label %L26
L26:
  %t53 = load ptr, ptr %t1
  %t54 = getelementptr [4 x i8], ptr @.str12, i64 0, i64 0
  %t55 = call ptr @strcpy(ptr %t53, ptr %t54)
  br label %L7
L41:
  br label %L27
L27:
  br label %L28
L28:
  %t56 = getelementptr i8, ptr %t0, i64 0
  %t57 = load i64, ptr %t56
  %t58 = icmp ne i64 %t57, 0
  br i1 %t58, label %L42, label %L43
L42:
  %t59 = load ptr, ptr %t1
  %t60 = getelementptr [12 x i8], ptr @.str13, i64 0, i64 0
  %t61 = getelementptr i8, ptr %t0, i64 0
  %t62 = load i64, ptr %t61
  %t63 = call i32 (ptr, ...) @snprintf(ptr %t59, i64 256, ptr %t60, i64 %t62)
  %t64 = sext i32 %t63 to i64
  br label %L44
L43:
  %t65 = load ptr, ptr %t1
  %t66 = getelementptr [4 x i8], ptr @.str14, i64 0, i64 0
  %t67 = call ptr @strcpy(ptr %t65, ptr %t66)
  br label %L44
L44:
  br label %L7
L45:
  br label %L29
L29:
  %t68 = load ptr, ptr %t1
  %t69 = getelementptr [4 x i8], ptr @.str15, i64 0, i64 0
  %t70 = call ptr @strcpy(ptr %t68, ptr %t69)
  br label %L7
L46:
  br label %L7
L30:
  %t71 = load ptr, ptr %t1
  %t72 = getelementptr [4 x i8], ptr @.str16, i64 0, i64 0
  %t73 = call ptr @strcpy(ptr %t71, ptr %t72)
  br label %L7
L47:
  br label %L7
L7:
  %t74 = load ptr, ptr %t1
  ret ptr %t74
L48:
  ret ptr null
}

define internal ptr @llvm_ret_type(ptr %t0) {
entry:
  %t2 = ptrtoint ptr %t0 to i64
  %t3 = icmp eq i64 %t2, 0
  %t1 = zext i1 %t3 to i64
  %t4 = icmp ne i64 %t1, 0
  br i1 %t4, label %L0, label %L1
L0:
  br label %L2
L1:
  %t5 = getelementptr i8, ptr %t0, i64 0
  %t6 = load i64, ptr %t5
  %t8 = sext i32 17 to i64
  %t7 = icmp ne i64 %t6, %t8
  %t9 = zext i1 %t7 to i64
  %t10 = icmp ne i64 %t9, 0
  %t11 = zext i1 %t10 to i64
  br label %L2
L2:
  %t12 = phi i64 [ 1, %L0 ], [ %t11, %L1 ]
  %t13 = icmp ne i64 %t12, 0
  br i1 %t13, label %L3, label %L5
L3:
  %t14 = getelementptr [4 x i8], ptr @.str17, i64 0, i64 0
  ret ptr %t14
L6:
  br label %L5
L5:
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = load i64, ptr %t15
  %t17 = call ptr @llvm_type(i64 %t16)
  ret ptr %t17
L7:
  ret ptr null
}

define internal i32 @type_is_fp(ptr %t0) {
entry:
  %t2 = ptrtoint ptr %t0 to i64
  %t3 = icmp eq i64 %t2, 0
  %t1 = zext i1 %t3 to i64
  %t4 = icmp ne i64 %t1, 0
  br i1 %t4, label %L0, label %L2
L0:
  %t5 = sext i32 0 to i64
  %t6 = trunc i64 %t5 to i32
  ret i32 %t6
L3:
  br label %L2
L2:
  %t7 = getelementptr i8, ptr %t0, i64 0
  %t8 = load i64, ptr %t7
  %t10 = sext i32 13 to i64
  %t9 = icmp eq i64 %t8, %t10
  %t11 = zext i1 %t9 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L4, label %L5
L4:
  br label %L6
L5:
  %t13 = getelementptr i8, ptr %t0, i64 0
  %t14 = load i64, ptr %t13
  %t16 = sext i32 14 to i64
  %t15 = icmp eq i64 %t14, %t16
  %t17 = zext i1 %t15 to i64
  %t18 = icmp ne i64 %t17, 0
  %t19 = zext i1 %t18 to i64
  br label %L6
L6:
  %t20 = phi i64 [ 1, %L4 ], [ %t19, %L5 ]
  %t21 = trunc i64 %t20 to i32
  ret i32 %t21
L7:
  ret i32 0
}

define internal void @scope_push(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t5 = getelementptr i8, ptr %t0, i64 0
  %t6 = load i64, ptr %t5
  %t7 = add i64 %t6, 1
  %t8 = getelementptr i8, ptr %t0, i64 0
  store i64 %t7, ptr %t8
  %t10 = inttoptr i64 %t4 to ptr
  %t9 = getelementptr ptr, ptr %t10, i64 %t6
  store i64 %t2, ptr %t9
  ret void
}

define internal void @scope_pop(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = load i64, ptr %t2
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t6 = sub i64 %t5, 1
  %t7 = getelementptr i8, ptr %t0, i64 0
  store i64 %t6, ptr %t7
  %t8 = inttoptr i64 %t3 to ptr
  %t9 = getelementptr ptr, ptr %t8, i64 %t6
  %t10 = load ptr, ptr %t9
  store ptr %t10, ptr %t1
  %t11 = alloca i64
  %t12 = load i64, ptr %t1
  %t13 = sext i32 %t12 to i64
  store i64 %t13, ptr %t11
  br label %L0
L0:
  %t14 = load i64, ptr %t11
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = load i64, ptr %t15
  %t18 = sext i32 %t14 to i64
  %t17 = icmp slt i64 %t18, %t16
  %t19 = zext i1 %t17 to i64
  %t20 = icmp ne i64 %t19, 0
  br i1 %t20, label %L1, label %L3
L1:
  %t21 = getelementptr i8, ptr %t0, i64 0
  %t22 = load i64, ptr %t21
  %t23 = load i64, ptr %t11
  %t25 = inttoptr i64 %t22 to ptr
  %t26 = sext i32 %t23 to i64
  %t24 = getelementptr ptr, ptr %t25, i64 %t26
  %t27 = getelementptr i8, ptr %t24, i64 0
  %t28 = load i64, ptr %t27
  call void @free(i64 %t28)
  %t30 = getelementptr i8, ptr %t0, i64 0
  %t31 = load i64, ptr %t30
  %t32 = load i64, ptr %t11
  %t34 = inttoptr i64 %t31 to ptr
  %t35 = sext i32 %t32 to i64
  %t33 = getelementptr ptr, ptr %t34, i64 %t35
  %t36 = getelementptr i8, ptr %t33, i64 0
  %t37 = load i64, ptr %t36
  call void @free(i64 %t37)
  br label %L2
L2:
  %t39 = load i64, ptr %t11
  %t41 = sext i32 %t39 to i64
  %t40 = add i64 %t41, 1
  store i64 %t40, ptr %t11
  br label %L0
L3:
  %t42 = load i64, ptr %t1
  %t43 = getelementptr i8, ptr %t0, i64 0
  %t44 = sext i32 %t42 to i64
  store i64 %t44, ptr %t43
  ret void
}

define internal ptr @find_local(ptr %t0, ptr %t1) {
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
  %t13 = getelementptr i8, ptr %t0, i64 0
  %t14 = load i64, ptr %t13
  %t15 = load i64, ptr %t2
  %t17 = inttoptr i64 %t14 to ptr
  %t18 = sext i32 %t15 to i64
  %t16 = getelementptr ptr, ptr %t17, i64 %t18
  %t19 = getelementptr i8, ptr %t16, i64 0
  %t20 = load i64, ptr %t19
  %t21 = call i32 @strcmp(i64 %t20, ptr %t1)
  %t22 = sext i32 %t21 to i64
  %t24 = sext i32 0 to i64
  %t23 = icmp eq i64 %t22, %t24
  %t25 = zext i1 %t23 to i64
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L4, label %L6
L4:
  %t27 = getelementptr i8, ptr %t0, i64 0
  %t28 = load i64, ptr %t27
  %t29 = load i64, ptr %t2
  %t31 = inttoptr i64 %t28 to ptr
  %t32 = sext i32 %t29 to i64
  %t30 = getelementptr ptr, ptr %t31, i64 %t32
  ret ptr %t30
L7:
  br label %L6
L6:
  br label %L2
L2:
  %t33 = load i64, ptr %t2
  %t35 = sext i32 %t33 to i64
  %t34 = sub i64 %t35, 1
  store i64 %t34, ptr %t2
  br label %L0
L3:
  %t37 = sext i32 0 to i64
  %t36 = inttoptr i64 %t37 to ptr
  ret ptr %t36
L8:
  ret ptr null
}

define internal ptr @find_global(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = sext i32 0 to i64
  store i64 %t3, ptr %t2
  br label %L0
L0:
  %t4 = load i64, ptr %t2
  %t5 = getelementptr i8, ptr %t0, i64 0
  %t6 = load i64, ptr %t5
  %t8 = sext i32 %t4 to i64
  %t7 = icmp slt i64 %t8, %t6
  %t9 = zext i1 %t7 to i64
  %t10 = icmp ne i64 %t9, 0
  br i1 %t10, label %L1, label %L3
L1:
  %t11 = getelementptr i8, ptr %t0, i64 0
  %t12 = load i64, ptr %t11
  %t13 = load i64, ptr %t2
  %t15 = inttoptr i64 %t12 to ptr
  %t16 = sext i32 %t13 to i64
  %t14 = getelementptr ptr, ptr %t15, i64 %t16
  %t17 = getelementptr i8, ptr %t14, i64 0
  %t18 = load i64, ptr %t17
  %t19 = call i32 @strcmp(i64 %t18, ptr %t1)
  %t20 = sext i32 %t19 to i64
  %t22 = sext i32 0 to i64
  %t21 = icmp eq i64 %t20, %t22
  %t23 = zext i1 %t21 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L4, label %L6
L4:
  %t25 = getelementptr i8, ptr %t0, i64 0
  %t26 = load i64, ptr %t25
  %t27 = load i64, ptr %t2
  %t29 = inttoptr i64 %t26 to ptr
  %t30 = sext i32 %t27 to i64
  %t28 = getelementptr ptr, ptr %t29, i64 %t30
  ret ptr %t28
L7:
  br label %L6
L6:
  br label %L2
L2:
  %t31 = load i64, ptr %t2
  %t33 = sext i32 %t31 to i64
  %t32 = add i64 %t33, 1
  store i64 %t32, ptr %t2
  br label %L0
L3:
  %t35 = sext i32 0 to i64
  %t34 = inttoptr i64 %t35 to ptr
  ret ptr %t34
L8:
  ret ptr null
}

define internal ptr @add_local(ptr %t0, ptr %t1, ptr %t2, i64 %t3) {
entry:
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t7 = sext i32 2048 to i64
  %t6 = icmp sge i64 %t5, %t7
  %t8 = zext i1 %t6 to i64
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L0, label %L2
L0:
  %t10 = call ptr @__c0c_stderr()
  %t11 = getelementptr [22 x i8], ptr @.str18, i64 0, i64 0
  %t12 = call i32 (ptr, ...) @fprintf(ptr %t10, ptr %t11)
  %t13 = sext i32 %t12 to i64
  call void @exit(i64 1)
  br label %L2
L2:
  %t15 = alloca ptr
  %t16 = getelementptr i8, ptr %t0, i64 0
  %t17 = load i64, ptr %t16
  %t18 = getelementptr i8, ptr %t0, i64 0
  %t19 = load i64, ptr %t18
  %t20 = add i64 %t19, 1
  %t21 = getelementptr i8, ptr %t0, i64 0
  store i64 %t20, ptr %t21
  %t23 = inttoptr i64 %t17 to ptr
  %t22 = getelementptr ptr, ptr %t23, i64 %t19
  store ptr %t22, ptr %t15
  %t24 = call ptr @strdup(ptr %t1)
  %t25 = load ptr, ptr %t15
  %t26 = getelementptr i8, ptr %t25, i64 0
  store ptr %t24, ptr %t26
  %t27 = alloca i64
  %t28 = call i32 @new_reg(ptr %t0)
  %t29 = sext i32 %t28 to i64
  store i64 %t29, ptr %t27
  %t30 = call ptr @malloc(i64 32)
  %t31 = load ptr, ptr %t15
  %t32 = getelementptr i8, ptr %t31, i64 0
  store ptr %t30, ptr %t32
  %t33 = load ptr, ptr %t15
  %t34 = getelementptr i8, ptr %t33, i64 0
  %t35 = load i64, ptr %t34
  %t36 = getelementptr [6 x i8], ptr @.str19, i64 0, i64 0
  %t37 = load i64, ptr %t27
  %t38 = call i32 (ptr, ...) @snprintf(i64 %t35, i64 32, ptr %t36, i64 %t37)
  %t39 = sext i32 %t38 to i64
  %t40 = load ptr, ptr %t15
  %t41 = getelementptr i8, ptr %t40, i64 0
  store ptr %t2, ptr %t41
  %t42 = load ptr, ptr %t15
  %t43 = getelementptr i8, ptr %t42, i64 0
  store i64 %t3, ptr %t43
  %t44 = load ptr, ptr %t15
  ret ptr %t44
L3:
  ret ptr null
}

define internal i32 @intern_string(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t5 = add i64 %t4, 1
  %t6 = getelementptr i8, ptr %t0, i64 0
  store i64 %t5, ptr %t6
  store i64 %t4, ptr %t2
  %t7 = call ptr @strdup(ptr %t1)
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  %t10 = getelementptr i8, ptr %t0, i64 0
  %t11 = load i64, ptr %t10
  %t13 = inttoptr i64 %t9 to ptr
  %t12 = getelementptr ptr, ptr %t13, i64 %t11
  store ptr %t7, ptr %t12
  %t14 = load i64, ptr %t2
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = load i64, ptr %t15
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t20 = inttoptr i64 %t16 to ptr
  %t19 = getelementptr ptr, ptr %t20, i64 %t18
  %t21 = sext i32 %t14 to i64
  store i64 %t21, ptr %t19
  %t22 = getelementptr i8, ptr %t0, i64 0
  %t23 = load i64, ptr %t22
  %t24 = add i64 %t23, 1
  %t25 = getelementptr i8, ptr %t0, i64 0
  store i64 %t24, ptr %t25
  %t26 = load i64, ptr %t2
  %t27 = sext i32 %t26 to i64
  %t28 = trunc i64 %t27 to i32
  ret i32 %t28
L0:
  ret i32 0
}

define internal i32 @str_literal_len(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = sext i32 0 to i64
  store i64 %t2, ptr %t1
  %t3 = alloca ptr
  %t5 = ptrtoint ptr %t0 to i64
  %t6 = sext i32 1 to i64
  %t7 = inttoptr i64 %t5 to ptr
  %t4 = getelementptr i8, ptr %t7, i64 %t6
  store ptr %t4, ptr %t3
  br label %L0
L0:
  %t8 = load ptr, ptr %t3
  %t9 = load i64, ptr %t8
  %t10 = icmp ne i64 %t9, 0
  br i1 %t10, label %L3, label %L4
L3:
  %t11 = load ptr, ptr %t3
  %t12 = load i64, ptr %t11
  %t14 = sext i32 34 to i64
  %t13 = icmp ne i64 %t12, %t14
  %t15 = zext i1 %t13 to i64
  %t16 = icmp ne i64 %t15, 0
  %t17 = zext i1 %t16 to i64
  br label %L5
L4:
  br label %L5
L5:
  %t18 = phi i64 [ %t17, %L3 ], [ 0, %L4 ]
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L1, label %L2
L1:
  %t20 = load ptr, ptr %t3
  %t21 = load i64, ptr %t20
  %t23 = sext i32 92 to i64
  %t22 = icmp eq i64 %t21, %t23
  %t24 = zext i1 %t22 to i64
  %t25 = icmp ne i64 %t24, 0
  br i1 %t25, label %L6, label %L7
L6:
  %t26 = load ptr, ptr %t3
  %t28 = ptrtoint ptr %t26 to i64
  %t27 = add i64 %t28, 1
  store i64 %t27, ptr %t3
  %t29 = load ptr, ptr %t3
  %t30 = load i64, ptr %t29
  %t31 = icmp ne i64 %t30, 0
  br i1 %t31, label %L9, label %L11
L9:
  %t32 = load ptr, ptr %t3
  %t34 = ptrtoint ptr %t32 to i64
  %t33 = add i64 %t34, 1
  store i64 %t33, ptr %t3
  br label %L11
L11:
  br label %L8
L7:
  %t35 = load ptr, ptr %t3
  %t37 = ptrtoint ptr %t35 to i64
  %t36 = add i64 %t37, 1
  store i64 %t36, ptr %t3
  br label %L8
L8:
  %t38 = load i64, ptr %t1
  %t40 = sext i32 %t38 to i64
  %t39 = add i64 %t40, 1
  store i64 %t39, ptr %t1
  br label %L0
L2:
  %t41 = load i64, ptr %t1
  %t43 = sext i32 %t41 to i64
  %t44 = sext i32 1 to i64
  %t42 = add i64 %t43, %t44
  %t45 = trunc i64 %t42 to i32
  ret i32 %t45
L12:
  ret i32 0
}

define internal void @emit_str_content(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t4 = ptrtoint ptr %t1 to i64
  %t5 = sext i32 1 to i64
  %t6 = inttoptr i64 %t4 to ptr
  %t3 = getelementptr i8, ptr %t6, i64 %t5
  store ptr %t3, ptr %t2
  br label %L0
L0:
  %t7 = load ptr, ptr %t2
  %t8 = load i64, ptr %t7
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L3, label %L4
L3:
  %t10 = load ptr, ptr %t2
  %t11 = load i64, ptr %t10
  %t13 = sext i32 34 to i64
  %t12 = icmp ne i64 %t11, %t13
  %t14 = zext i1 %t12 to i64
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
  %t19 = load ptr, ptr %t2
  %t20 = load i64, ptr %t19
  %t22 = sext i32 92 to i64
  %t21 = icmp eq i64 %t20, %t22
  %t23 = zext i1 %t21 to i64
  %t24 = icmp ne i64 %t23, 0
  br i1 %t24, label %L6, label %L7
L6:
  %t25 = load ptr, ptr %t2
  %t27 = ptrtoint ptr %t25 to i64
  %t28 = sext i32 1 to i64
  %t29 = inttoptr i64 %t27 to ptr
  %t26 = getelementptr i8, ptr %t29, i64 %t28
  %t30 = load i64, ptr %t26
  %t31 = icmp ne i64 %t30, 0
  %t32 = zext i1 %t31 to i64
  br label %L8
L7:
  br label %L8
L8:
  %t33 = phi i64 [ %t32, %L6 ], [ 0, %L7 ]
  %t34 = icmp ne i64 %t33, 0
  br i1 %t34, label %L9, label %L10
L9:
  %t35 = load ptr, ptr %t2
  %t37 = ptrtoint ptr %t35 to i64
  %t36 = add i64 %t37, 1
  store i64 %t36, ptr %t2
  %t38 = load ptr, ptr %t2
  %t39 = load i64, ptr %t38
  %t40 = add i64 %t39, 0
  switch i64 %t40, label %L19 [
    i64 110, label %L13
    i64 116, label %L14
    i64 114, label %L15
    i64 48, label %L16
    i64 34, label %L17
    i64 92, label %L18
  ]
L13:
  %t41 = getelementptr i8, ptr %t0, i64 0
  %t42 = load i64, ptr %t41
  %t43 = getelementptr [4 x i8], ptr @.str20, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t42, ptr %t43)
  br label %L12
L20:
  br label %L14
L14:
  %t45 = getelementptr i8, ptr %t0, i64 0
  %t46 = load i64, ptr %t45
  %t47 = getelementptr [4 x i8], ptr @.str21, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t46, ptr %t47)
  br label %L12
L21:
  br label %L15
L15:
  %t49 = getelementptr i8, ptr %t0, i64 0
  %t50 = load i64, ptr %t49
  %t51 = getelementptr [4 x i8], ptr @.str22, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t50, ptr %t51)
  br label %L12
L22:
  br label %L16
L16:
  %t53 = getelementptr i8, ptr %t0, i64 0
  %t54 = load i64, ptr %t53
  %t55 = getelementptr [4 x i8], ptr @.str23, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t54, ptr %t55)
  br label %L12
L23:
  br label %L17
L17:
  %t57 = getelementptr i8, ptr %t0, i64 0
  %t58 = load i64, ptr %t57
  %t59 = getelementptr [4 x i8], ptr @.str24, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t58, ptr %t59)
  br label %L12
L24:
  br label %L18
L18:
  %t61 = getelementptr i8, ptr %t0, i64 0
  %t62 = load i64, ptr %t61
  %t63 = getelementptr [4 x i8], ptr @.str25, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t62, ptr %t63)
  br label %L12
L25:
  br label %L12
L19:
  %t65 = getelementptr i8, ptr %t0, i64 0
  %t66 = load i64, ptr %t65
  %t67 = getelementptr [6 x i8], ptr @.str26, i64 0, i64 0
  %t68 = load ptr, ptr %t2
  %t69 = load i64, ptr %t68
  %t70 = add i64 %t69, 0
  call void (ptr, ...) @__c0c_emit(i64 %t66, ptr %t67, i64 %t70)
  br label %L12
L26:
  br label %L12
L12:
  %t72 = load ptr, ptr %t2
  %t74 = ptrtoint ptr %t72 to i64
  %t73 = add i64 %t74, 1
  store i64 %t73, ptr %t2
  br label %L11
L10:
  %t75 = load ptr, ptr %t2
  %t76 = load i64, ptr %t75
  %t78 = sext i32 34 to i64
  %t77 = icmp eq i64 %t76, %t78
  %t79 = zext i1 %t77 to i64
  %t80 = icmp ne i64 %t79, 0
  br i1 %t80, label %L27, label %L29
L27:
  br label %L2
L30:
  br label %L29
L29:
  %t81 = getelementptr i8, ptr %t0, i64 0
  %t82 = load i64, ptr %t81
  %t83 = getelementptr [3 x i8], ptr @.str27, i64 0, i64 0
  %t84 = load ptr, ptr %t2
  %t86 = ptrtoint ptr %t84 to i64
  %t85 = add i64 %t86, 1
  store i64 %t85, ptr %t2
  %t87 = load i64, ptr %t84
  call void (ptr, ...) @__c0c_emit(i64 %t82, ptr %t83, i64 %t87)
  br label %L11
L11:
  br label %L0
L2:
  %t89 = getelementptr i8, ptr %t0, i64 0
  %t90 = load i64, ptr %t89
  %t91 = getelementptr [4 x i8], ptr @.str28, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t90, ptr %t91)
  ret void
}

define internal i64 @make_val(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = getelementptr i8, ptr %t2, i64 0
  %t4 = load i64, ptr %t3
  %t5 = call ptr @strncpy(i64 %t4, ptr %t0, i64 63)
  %t6 = getelementptr i8, ptr %t2, i64 0
  %t7 = load i64, ptr %t6
  %t9 = inttoptr i64 %t7 to ptr
  %t10 = sext i32 63 to i64
  %t8 = getelementptr ptr, ptr %t9, i64 %t10
  %t11 = sext i32 0 to i64
  store i64 %t11, ptr %t8
  %t12 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1, ptr %t12
  %t13 = load i64, ptr %t2
  %t14 = sext i32 %t13 to i64
  ret i64 %t14
L0:
  ret i64 0
}

define internal ptr @emit_lvalue_addr(ptr %t0, ptr %t1) {
entry:
  %t2 = getelementptr i8, ptr %t1, i64 0
  %t3 = load i64, ptr %t2
  %t5 = sext i32 23 to i64
  %t4 = icmp eq i64 %t3, %t5
  %t6 = zext i1 %t4 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = alloca ptr
  %t9 = getelementptr i8, ptr %t1, i64 0
  %t10 = load i64, ptr %t9
  %t11 = call ptr @find_local(ptr %t0, i64 %t10)
  store ptr %t11, ptr %t8
  %t12 = load ptr, ptr %t8
  %t13 = icmp ne ptr %t12, null
  br i1 %t13, label %L3, label %L5
L3:
  %t14 = load ptr, ptr %t8
  %t15 = getelementptr i8, ptr %t14, i64 0
  %t16 = load i64, ptr %t15
  %t17 = call ptr @strdup(i64 %t16)
  ret ptr %t17
L6:
  br label %L5
L5:
  %t18 = alloca ptr
  %t19 = getelementptr i8, ptr %t1, i64 0
  %t20 = load i64, ptr %t19
  %t21 = call ptr @find_global(ptr %t0, i64 %t20)
  store ptr %t21, ptr %t18
  %t22 = load ptr, ptr %t18
  %t23 = icmp ne ptr %t22, null
  br i1 %t23, label %L7, label %L9
L7:
  %t24 = alloca ptr
  %t25 = call ptr @malloc(i64 128)
  store ptr %t25, ptr %t24
  %t26 = load ptr, ptr %t24
  %t27 = getelementptr [4 x i8], ptr @.str29, i64 0, i64 0
  %t28 = getelementptr i8, ptr %t1, i64 0
  %t29 = load i64, ptr %t28
  %t30 = call i32 (ptr, ...) @snprintf(ptr %t26, i64 128, ptr %t27, i64 %t29)
  %t31 = sext i32 %t30 to i64
  %t32 = load ptr, ptr %t24
  ret ptr %t32
L10:
  br label %L9
L9:
  %t33 = alloca ptr
  %t34 = call ptr @malloc(i64 128)
  store ptr %t34, ptr %t33
  %t35 = load ptr, ptr %t33
  %t36 = getelementptr [4 x i8], ptr @.str30, i64 0, i64 0
  %t37 = getelementptr i8, ptr %t1, i64 0
  %t38 = load i64, ptr %t37
  %t39 = call i32 (ptr, ...) @snprintf(ptr %t35, i64 128, ptr %t36, i64 %t38)
  %t40 = sext i32 %t39 to i64
  %t41 = load ptr, ptr %t33
  ret ptr %t41
L11:
  br label %L2
L2:
  %t42 = getelementptr i8, ptr %t1, i64 0
  %t43 = load i64, ptr %t42
  %t45 = sext i32 37 to i64
  %t44 = icmp eq i64 %t43, %t45
  %t46 = zext i1 %t44 to i64
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L12, label %L14
L12:
  %t48 = alloca i64
  %t49 = getelementptr i8, ptr %t1, i64 0
  %t50 = load i64, ptr %t49
  %t51 = inttoptr i64 %t50 to ptr
  %t52 = sext i32 0 to i64
  %t53 = getelementptr ptr, ptr %t51, i64 %t52
  %t54 = load ptr, ptr %t53
  %t55 = call i64 @emit_expr(ptr %t0, ptr %t54)
  store i64 %t55, ptr %t48
  %t56 = load i64, ptr %t48
  %t57 = call i32 @val_is_ptr(i64 %t56)
  %t58 = sext i32 %t57 to i64
  %t59 = icmp ne i64 %t58, 0
  br i1 %t59, label %L15, label %L17
L15:
  %t60 = getelementptr i8, ptr %t48, i64 0
  %t61 = load i64, ptr %t60
  %t62 = call ptr @strdup(i64 %t61)
  ret ptr %t62
L18:
  br label %L17
L17:
  %t63 = alloca i64
  %t64 = call i32 @new_reg(ptr %t0)
  %t65 = sext i32 %t64 to i64
  store i64 %t65, ptr %t63
  %t66 = getelementptr i8, ptr %t0, i64 0
  %t67 = load i64, ptr %t66
  %t68 = getelementptr [34 x i8], ptr @.str31, i64 0, i64 0
  %t69 = load i64, ptr %t63
  %t70 = getelementptr i8, ptr %t48, i64 0
  %t71 = load i64, ptr %t70
  call void (ptr, ...) @__c0c_emit(i64 %t67, ptr %t68, i64 %t69, i64 %t71)
  %t73 = alloca ptr
  %t74 = call ptr @malloc(i64 32)
  store ptr %t74, ptr %t73
  %t75 = load ptr, ptr %t73
  %t76 = getelementptr [6 x i8], ptr @.str32, i64 0, i64 0
  %t77 = load i64, ptr %t63
  %t78 = call i32 (ptr, ...) @snprintf(ptr %t75, i64 32, ptr %t76, i64 %t77)
  %t79 = sext i32 %t78 to i64
  %t80 = load ptr, ptr %t73
  ret ptr %t80
L19:
  br label %L14
L14:
  %t81 = getelementptr i8, ptr %t1, i64 0
  %t82 = load i64, ptr %t81
  %t84 = sext i32 33 to i64
  %t83 = icmp eq i64 %t82, %t84
  %t85 = zext i1 %t83 to i64
  %t86 = icmp ne i64 %t85, 0
  br i1 %t86, label %L20, label %L22
L20:
  %t87 = alloca i64
  %t88 = getelementptr i8, ptr %t1, i64 0
  %t89 = load i64, ptr %t88
  %t90 = inttoptr i64 %t89 to ptr
  %t91 = sext i32 0 to i64
  %t92 = getelementptr ptr, ptr %t90, i64 %t91
  %t93 = load ptr, ptr %t92
  %t94 = call i64 @emit_expr(ptr %t0, ptr %t93)
  store i64 %t94, ptr %t87
  %t95 = alloca i64
  %t96 = getelementptr i8, ptr %t1, i64 0
  %t97 = load i64, ptr %t96
  %t98 = inttoptr i64 %t97 to ptr
  %t99 = sext i32 1 to i64
  %t100 = getelementptr ptr, ptr %t98, i64 %t99
  %t101 = load ptr, ptr %t100
  %t102 = call i64 @emit_expr(ptr %t0, ptr %t101)
  store i64 %t102, ptr %t95
  %t103 = alloca i64
  %t104 = call i32 @new_reg(ptr %t0)
  %t105 = sext i32 %t104 to i64
  store i64 %t105, ptr %t103
  %t106 = alloca ptr
  %t107 = getelementptr i8, ptr %t1, i64 0
  %t108 = load i64, ptr %t107
  %t109 = inttoptr i64 %t108 to ptr
  %t110 = sext i32 0 to i64
  %t111 = getelementptr ptr, ptr %t109, i64 %t110
  %t112 = load ptr, ptr %t111
  %t113 = getelementptr i8, ptr %t112, i64 0
  %t114 = load i64, ptr %t113
  %t115 = icmp ne i64 %t114, 0
  br i1 %t115, label %L23, label %L24
L23:
  %t116 = getelementptr i8, ptr %t1, i64 0
  %t117 = load i64, ptr %t116
  %t118 = inttoptr i64 %t117 to ptr
  %t119 = sext i32 0 to i64
  %t120 = getelementptr ptr, ptr %t118, i64 %t119
  %t121 = load ptr, ptr %t120
  %t122 = getelementptr i8, ptr %t121, i64 0
  %t123 = load i64, ptr %t122
  %t124 = inttoptr i64 %t123 to ptr
  %t125 = getelementptr i8, ptr %t124, i64 0
  %t126 = load i64, ptr %t125
  %t127 = icmp ne i64 %t126, 0
  %t128 = zext i1 %t127 to i64
  br label %L25
L24:
  br label %L25
L25:
  %t129 = phi i64 [ %t128, %L23 ], [ 0, %L24 ]
  %t130 = icmp ne i64 %t129, 0
  br i1 %t130, label %L26, label %L27
L26:
  %t131 = getelementptr i8, ptr %t1, i64 0
  %t132 = load i64, ptr %t131
  %t133 = inttoptr i64 %t132 to ptr
  %t134 = sext i32 0 to i64
  %t135 = getelementptr ptr, ptr %t133, i64 %t134
  %t136 = load ptr, ptr %t135
  %t137 = getelementptr i8, ptr %t136, i64 0
  %t138 = load i64, ptr %t137
  %t139 = inttoptr i64 %t138 to ptr
  %t140 = getelementptr i8, ptr %t139, i64 0
  %t141 = load i64, ptr %t140
  br label %L28
L27:
  %t143 = sext i32 0 to i64
  %t142 = inttoptr i64 %t143 to ptr
  %t144 = ptrtoint ptr %t142 to i64
  br label %L28
L28:
  %t145 = phi i64 [ %t141, %L26 ], [ %t144, %L27 ]
  store i64 %t145, ptr %t106
  %t146 = alloca ptr
  %t147 = load ptr, ptr %t106
  %t148 = icmp ne ptr %t147, null
  br i1 %t148, label %L29, label %L30
L29:
  %t149 = load ptr, ptr %t106
  %t150 = call ptr @llvm_type(ptr %t149)
  %t151 = ptrtoint ptr %t150 to i64
  br label %L31
L30:
  %t152 = getelementptr [4 x i8], ptr @.str33, i64 0, i64 0
  %t153 = ptrtoint ptr %t152 to i64
  br label %L31
L31:
  %t154 = phi i64 [ %t151, %L29 ], [ %t153, %L30 ]
  store i64 %t154, ptr %t146
  %t155 = alloca ptr
  %t156 = load i64, ptr %t87
  %t157 = call i32 @val_is_ptr(i64 %t156)
  %t158 = sext i32 %t157 to i64
  %t159 = icmp ne i64 %t158, 0
  br i1 %t159, label %L32, label %L33
L32:
  %t160 = load ptr, ptr %t155
  %t161 = getelementptr i8, ptr %t87, i64 0
  %t162 = load i64, ptr %t161
  %t163 = call ptr @strncpy(ptr %t160, i64 %t162, i64 63)
  %t164 = load ptr, ptr %t155
  %t166 = sext i32 63 to i64
  %t165 = getelementptr ptr, ptr %t164, i64 %t166
  %t167 = sext i32 0 to i64
  store i64 %t167, ptr %t165
  br label %L34
L33:
  %t168 = alloca i64
  %t169 = call i32 @new_reg(ptr %t0)
  %t170 = sext i32 %t169 to i64
  store i64 %t170, ptr %t168
  %t171 = getelementptr i8, ptr %t0, i64 0
  %t172 = load i64, ptr %t171
  %t173 = getelementptr [34 x i8], ptr @.str34, i64 0, i64 0
  %t174 = load i64, ptr %t168
  %t175 = getelementptr i8, ptr %t87, i64 0
  %t176 = load i64, ptr %t175
  call void (ptr, ...) @__c0c_emit(i64 %t172, ptr %t173, i64 %t174, i64 %t176)
  %t178 = load ptr, ptr %t155
  %t179 = getelementptr [6 x i8], ptr @.str35, i64 0, i64 0
  %t180 = load i64, ptr %t168
  %t181 = call i32 (ptr, ...) @snprintf(ptr %t178, i64 64, ptr %t179, i64 %t180)
  %t182 = sext i32 %t181 to i64
  br label %L34
L34:
  %t183 = alloca ptr
  %t184 = load i64, ptr %t95
  %t185 = load ptr, ptr %t183
  %t186 = call i32 @promote_to_i64(ptr %t0, i64 %t184, ptr %t185, i64 64)
  %t187 = sext i32 %t186 to i64
  %t188 = getelementptr i8, ptr %t0, i64 0
  %t189 = load i64, ptr %t188
  %t190 = getelementptr [44 x i8], ptr @.str36, i64 0, i64 0
  %t191 = load i64, ptr %t103
  %t192 = load ptr, ptr %t146
  %t193 = load ptr, ptr %t155
  %t194 = load ptr, ptr %t183
  call void (ptr, ...) @__c0c_emit(i64 %t189, ptr %t190, i64 %t191, ptr %t192, ptr %t193, ptr %t194)
  %t196 = alloca ptr
  %t197 = call ptr @malloc(i64 32)
  store ptr %t197, ptr %t196
  %t198 = load ptr, ptr %t196
  %t199 = getelementptr [6 x i8], ptr @.str37, i64 0, i64 0
  %t200 = load i64, ptr %t103
  %t201 = call i32 (ptr, ...) @snprintf(ptr %t198, i64 32, ptr %t199, i64 %t200)
  %t202 = sext i32 %t201 to i64
  %t203 = load ptr, ptr %t196
  ret ptr %t203
L35:
  br label %L22
L22:
  %t204 = getelementptr i8, ptr %t1, i64 0
  %t205 = load i64, ptr %t204
  %t207 = sext i32 34 to i64
  %t206 = icmp eq i64 %t205, %t207
  %t208 = zext i1 %t206 to i64
  %t209 = icmp ne i64 %t208, 0
  br i1 %t209, label %L36, label %L37
L36:
  br label %L38
L37:
  %t210 = getelementptr i8, ptr %t1, i64 0
  %t211 = load i64, ptr %t210
  %t213 = sext i32 35 to i64
  %t212 = icmp eq i64 %t211, %t213
  %t214 = zext i1 %t212 to i64
  %t215 = icmp ne i64 %t214, 0
  %t216 = zext i1 %t215 to i64
  br label %L38
L38:
  %t217 = phi i64 [ 1, %L36 ], [ %t216, %L37 ]
  %t218 = icmp ne i64 %t217, 0
  br i1 %t218, label %L39, label %L41
L39:
  %t219 = alloca i64
  %t220 = getelementptr i8, ptr %t1, i64 0
  %t221 = load i64, ptr %t220
  %t223 = sext i32 35 to i64
  %t222 = icmp eq i64 %t221, %t223
  %t224 = zext i1 %t222 to i64
  %t225 = icmp ne i64 %t224, 0
  br i1 %t225, label %L42, label %L43
L42:
  %t226 = getelementptr i8, ptr %t1, i64 0
  %t227 = load i64, ptr %t226
  %t228 = inttoptr i64 %t227 to ptr
  %t229 = sext i32 0 to i64
  %t230 = getelementptr ptr, ptr %t228, i64 %t229
  %t231 = load ptr, ptr %t230
  %t232 = call i64 @emit_expr(ptr %t0, ptr %t231)
  store i64 %t232, ptr %t219
  br label %L44
L43:
  %t233 = alloca ptr
  %t234 = getelementptr i8, ptr %t1, i64 0
  %t235 = load i64, ptr %t234
  %t236 = inttoptr i64 %t235 to ptr
  %t237 = sext i32 0 to i64
  %t238 = getelementptr ptr, ptr %t236, i64 %t237
  %t239 = load ptr, ptr %t238
  %t240 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t239)
  store ptr %t240, ptr %t233
  %t241 = load ptr, ptr %t233
  %t242 = icmp ne ptr %t241, null
  br i1 %t242, label %L45, label %L46
L45:
  %t243 = load ptr, ptr %t233
  %t244 = call ptr @default_ptr_type()
  %t245 = call i64 @make_val(ptr %t243, ptr %t244)
  store i64 %t245, ptr %t219
  %t246 = load ptr, ptr %t233
  call void @free(ptr %t246)
  br label %L47
L46:
  %t248 = getelementptr i8, ptr %t1, i64 0
  %t249 = load i64, ptr %t248
  %t250 = inttoptr i64 %t249 to ptr
  %t251 = sext i32 0 to i64
  %t252 = getelementptr ptr, ptr %t250, i64 %t251
  %t253 = load ptr, ptr %t252
  %t254 = call i64 @emit_expr(ptr %t0, ptr %t253)
  store i64 %t254, ptr %t219
  %t255 = load i64, ptr %t219
  %t256 = call i32 @val_is_ptr(i64 %t255)
  %t257 = sext i32 %t256 to i64
  %t259 = icmp eq i64 %t257, 0
  %t258 = zext i1 %t259 to i64
  %t260 = icmp ne i64 %t258, 0
  br i1 %t260, label %L48, label %L50
L48:
  %t261 = alloca i64
  %t262 = call i32 @new_reg(ptr %t0)
  %t263 = sext i32 %t262 to i64
  store i64 %t263, ptr %t261
  %t264 = alloca ptr
  %t265 = load i64, ptr %t219
  %t266 = load ptr, ptr %t264
  %t267 = call i32 @promote_to_i64(ptr %t0, i64 %t265, ptr %t266, i64 64)
  %t268 = sext i32 %t267 to i64
  %t269 = getelementptr i8, ptr %t0, i64 0
  %t270 = load i64, ptr %t269
  %t271 = getelementptr [34 x i8], ptr @.str38, i64 0, i64 0
  %t272 = load i64, ptr %t261
  %t273 = load ptr, ptr %t264
  call void (ptr, ...) @__c0c_emit(i64 %t270, ptr %t271, i64 %t272, ptr %t273)
  %t275 = alloca ptr
  %t276 = load ptr, ptr %t275
  %t277 = getelementptr [6 x i8], ptr @.str39, i64 0, i64 0
  %t278 = load i64, ptr %t261
  %t279 = call i32 (ptr, ...) @snprintf(ptr %t276, i64 32, ptr %t277, i64 %t278)
  %t280 = sext i32 %t279 to i64
  %t281 = load ptr, ptr %t275
  %t282 = call ptr @default_ptr_type()
  %t283 = call i64 @make_val(ptr %t281, ptr %t282)
  store i64 %t283, ptr %t219
  br label %L50
L50:
  br label %L47
L47:
  br label %L44
L44:
  %t284 = alloca ptr
  %t285 = getelementptr i8, ptr %t1, i64 0
  %t286 = load i64, ptr %t285
  store i64 %t286, ptr %t284
  %t287 = alloca ptr
  %t288 = getelementptr i8, ptr %t1, i64 0
  %t289 = load i64, ptr %t288
  %t290 = inttoptr i64 %t289 to ptr
  %t291 = sext i32 0 to i64
  %t292 = getelementptr ptr, ptr %t290, i64 %t291
  %t293 = load ptr, ptr %t292
  %t294 = icmp ne ptr %t293, null
  br i1 %t294, label %L51, label %L52
L51:
  %t295 = getelementptr i8, ptr %t1, i64 0
  %t296 = load i64, ptr %t295
  %t297 = inttoptr i64 %t296 to ptr
  %t298 = sext i32 0 to i64
  %t299 = getelementptr ptr, ptr %t297, i64 %t298
  %t300 = load ptr, ptr %t299
  %t301 = getelementptr i8, ptr %t300, i64 0
  %t302 = load i64, ptr %t301
  br label %L53
L52:
  %t304 = sext i32 0 to i64
  %t303 = inttoptr i64 %t304 to ptr
  %t305 = ptrtoint ptr %t303 to i64
  br label %L53
L53:
  %t306 = phi i64 [ %t302, %L51 ], [ %t305, %L52 ]
  store i64 %t306, ptr %t287
  %t307 = alloca i64
  %t308 = sext i32 0 to i64
  store i64 %t308, ptr %t307
  %t309 = load ptr, ptr %t287
  %t310 = ptrtoint ptr %t309 to i64
  %t311 = icmp ne i64 %t310, 0
  br i1 %t311, label %L54, label %L55
L54:
  %t312 = load ptr, ptr %t287
  %t313 = getelementptr i8, ptr %t312, i64 0
  %t314 = load i64, ptr %t313
  %t316 = sext i32 18 to i64
  %t315 = icmp eq i64 %t314, %t316
  %t317 = zext i1 %t315 to i64
  %t318 = icmp ne i64 %t317, 0
  br i1 %t318, label %L57, label %L58
L57:
  br label %L59
L58:
  %t319 = load ptr, ptr %t287
  %t320 = getelementptr i8, ptr %t319, i64 0
  %t321 = load i64, ptr %t320
  %t323 = sext i32 19 to i64
  %t322 = icmp eq i64 %t321, %t323
  %t324 = zext i1 %t322 to i64
  %t325 = icmp ne i64 %t324, 0
  %t326 = zext i1 %t325 to i64
  br label %L59
L59:
  %t327 = phi i64 [ 1, %L57 ], [ %t326, %L58 ]
  %t328 = icmp ne i64 %t327, 0
  %t329 = zext i1 %t328 to i64
  br label %L56
L55:
  br label %L56
L56:
  %t330 = phi i64 [ %t329, %L54 ], [ 0, %L55 ]
  %t331 = icmp ne i64 %t330, 0
  br i1 %t331, label %L60, label %L61
L60:
  %t332 = load ptr, ptr %t287
  %t333 = getelementptr i8, ptr %t332, i64 0
  %t334 = load i64, ptr %t333
  %t335 = icmp ne i64 %t334, 0
  %t336 = zext i1 %t335 to i64
  br label %L62
L61:
  br label %L62
L62:
  %t337 = phi i64 [ %t336, %L60 ], [ 0, %L61 ]
  %t338 = icmp ne i64 %t337, 0
  br i1 %t338, label %L63, label %L65
L63:
  %t339 = alloca i64
  %t340 = sext i32 0 to i64
  store i64 %t340, ptr %t339
  br label %L66
L66:
  %t341 = load i64, ptr %t339
  %t342 = load ptr, ptr %t287
  %t343 = getelementptr i8, ptr %t342, i64 0
  %t344 = load i64, ptr %t343
  %t346 = sext i32 %t341 to i64
  %t345 = icmp slt i64 %t346, %t344
  %t347 = zext i1 %t345 to i64
  %t348 = icmp ne i64 %t347, 0
  br i1 %t348, label %L67, label %L69
L67:
  %t349 = load ptr, ptr %t287
  %t350 = getelementptr i8, ptr %t349, i64 0
  %t351 = load i64, ptr %t350
  %t352 = load i64, ptr %t339
  %t354 = inttoptr i64 %t351 to ptr
  %t355 = sext i32 %t352 to i64
  %t353 = getelementptr ptr, ptr %t354, i64 %t355
  %t356 = getelementptr i8, ptr %t353, i64 0
  %t357 = load i64, ptr %t356
  %t358 = icmp ne i64 %t357, 0
  br i1 %t358, label %L70, label %L71
L70:
  %t359 = load ptr, ptr %t287
  %t360 = getelementptr i8, ptr %t359, i64 0
  %t361 = load i64, ptr %t360
  %t362 = load i64, ptr %t339
  %t364 = inttoptr i64 %t361 to ptr
  %t365 = sext i32 %t362 to i64
  %t363 = getelementptr ptr, ptr %t364, i64 %t365
  %t366 = getelementptr i8, ptr %t363, i64 0
  %t367 = load i64, ptr %t366
  %t368 = load ptr, ptr %t284
  %t369 = call i32 @strcmp(i64 %t367, ptr %t368)
  %t370 = sext i32 %t369 to i64
  %t372 = sext i32 0 to i64
  %t371 = icmp eq i64 %t370, %t372
  %t373 = zext i1 %t371 to i64
  %t374 = icmp ne i64 %t373, 0
  %t375 = zext i1 %t374 to i64
  br label %L72
L71:
  br label %L72
L72:
  %t376 = phi i64 [ %t375, %L70 ], [ 0, %L71 ]
  %t377 = icmp ne i64 %t376, 0
  br i1 %t377, label %L73, label %L75
L73:
  br label %L69
L76:
  br label %L75
L75:
  %t378 = load ptr, ptr %t287
  %t379 = getelementptr i8, ptr %t378, i64 0
  %t380 = load i64, ptr %t379
  %t381 = icmp ne i64 %t380, 0
  br i1 %t381, label %L77, label %L78
L77:
  %t382 = load ptr, ptr %t287
  %t383 = getelementptr i8, ptr %t382, i64 0
  %t384 = load i64, ptr %t383
  %t385 = load i64, ptr %t339
  %t386 = inttoptr i64 %t384 to ptr
  %t387 = sext i32 %t385 to i64
  %t388 = getelementptr ptr, ptr %t386, i64 %t387
  %t389 = load ptr, ptr %t388
  %t390 = ptrtoint ptr %t389 to i64
  %t391 = icmp ne i64 %t390, 0
  %t392 = zext i1 %t391 to i64
  br label %L79
L78:
  br label %L79
L79:
  %t393 = phi i64 [ %t392, %L77 ], [ 0, %L78 ]
  %t394 = icmp ne i64 %t393, 0
  br i1 %t394, label %L80, label %L82
L80:
  %t395 = load i64, ptr %t307
  %t396 = load ptr, ptr %t287
  %t397 = getelementptr i8, ptr %t396, i64 0
  %t398 = load i64, ptr %t397
  %t399 = load i64, ptr %t339
  %t400 = inttoptr i64 %t398 to ptr
  %t401 = sext i32 %t399 to i64
  %t402 = getelementptr ptr, ptr %t400, i64 %t401
  %t403 = load ptr, ptr %t402
  %t404 = call i32 @type_size(ptr %t403)
  %t405 = sext i32 %t404 to i64
  %t406 = add i64 %t395, %t405
  store i64 %t406, ptr %t307
  br label %L82
L82:
  br label %L68
L68:
  %t407 = load i64, ptr %t339
  %t409 = sext i32 %t407 to i64
  %t408 = add i64 %t409, 1
  store i64 %t408, ptr %t339
  br label %L66
L69:
  br label %L65
L65:
  %t410 = alloca i64
  %t411 = call i32 @new_reg(ptr %t0)
  %t412 = sext i32 %t411 to i64
  store i64 %t412, ptr %t410
  %t413 = getelementptr i8, ptr %t0, i64 0
  %t414 = load i64, ptr %t413
  %t415 = getelementptr [45 x i8], ptr @.str40, i64 0, i64 0
  %t416 = load i64, ptr %t410
  %t417 = getelementptr i8, ptr %t219, i64 0
  %t418 = load i64, ptr %t417
  %t419 = load i64, ptr %t307
  call void (ptr, ...) @__c0c_emit(i64 %t414, ptr %t415, i64 %t416, i64 %t418, i64 %t419)
  %t421 = alloca ptr
  %t422 = call ptr @malloc(i64 32)
  store ptr %t422, ptr %t421
  %t423 = load ptr, ptr %t421
  %t424 = getelementptr [6 x i8], ptr @.str41, i64 0, i64 0
  %t425 = load i64, ptr %t410
  %t426 = call i32 (ptr, ...) @snprintf(ptr %t423, i64 32, ptr %t424, i64 %t425)
  %t427 = sext i32 %t426 to i64
  %t428 = load ptr, ptr %t421
  ret ptr %t428
L83:
  br label %L41
L41:
  %t430 = sext i32 0 to i64
  %t429 = inttoptr i64 %t430 to ptr
  ret ptr %t429
L84:
  ret ptr null
}

define internal ptr @default_int_type() {
entry:
  %t0 = alloca i64
  %t1 = sext i32 0 to i64
  store i64 %t1, ptr %t0
  ret ptr %t0
L0:
  ret ptr null
}

define internal ptr @default_i64_type() {
entry:
  %t0 = alloca i64
  %t1 = sext i32 0 to i64
  store i64 %t1, ptr %t0
  ret ptr %t0
L0:
  ret ptr null
}

define internal ptr @default_ptr_type() {
entry:
  %t0 = alloca i64
  %t1 = sext i32 0 to i64
  store i64 %t1, ptr %t0
  ret ptr %t0
L0:
  ret ptr null
}

define internal ptr @default_fp_type() {
entry:
  %t0 = alloca i64
  %t1 = sext i32 0 to i64
  store i64 %t1, ptr %t0
  ret ptr %t0
L0:
  ret ptr null
}

define internal i32 @val_is_64bit(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t4 = icmp eq i64 %t2, 0
  %t3 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t3, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = sext i32 0 to i64
  %t7 = trunc i64 %t6 to i32
  ret i32 %t7
L3:
  br label %L2
L2:
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  %t10 = inttoptr i64 %t9 to ptr
  %t11 = getelementptr i8, ptr %t10, i64 0
  %t12 = load i64, ptr %t11
  %t13 = add i64 %t12, 0
  switch i64 %t13, label %L12 [
    i64 9, label %L5
    i64 10, label %L6
    i64 11, label %L7
    i64 12, label %L8
    i64 15, label %L9
    i64 16, label %L10
    i64 14, label %L11
  ]
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
  %t14 = sext i32 1 to i64
  %t15 = trunc i64 %t14 to i32
  ret i32 %t15
L13:
  br label %L4
L12:
  %t16 = sext i32 0 to i64
  %t17 = trunc i64 %t16 to i32
  ret i32 %t17
L14:
  br label %L4
L4:
  ret i32 0
}

define internal i32 @val_is_ptr(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t4 = icmp eq i64 %t2, 0
  %t3 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t3, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = sext i32 0 to i64
  %t7 = trunc i64 %t6 to i32
  ret i32 %t7
L3:
  br label %L2
L2:
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  %t10 = inttoptr i64 %t9 to ptr
  %t11 = getelementptr i8, ptr %t10, i64 0
  %t12 = load i64, ptr %t11
  %t14 = sext i32 15 to i64
  %t13 = icmp eq i64 %t12, %t14
  %t15 = zext i1 %t13 to i64
  %t16 = icmp ne i64 %t15, 0
  br i1 %t16, label %L4, label %L5
L4:
  br label %L6
L5:
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t19 = inttoptr i64 %t18 to ptr
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = load i64, ptr %t20
  %t23 = sext i32 16 to i64
  %t22 = icmp eq i64 %t21, %t23
  %t24 = zext i1 %t22 to i64
  %t25 = icmp ne i64 %t24, 0
  %t26 = zext i1 %t25 to i64
  br label %L6
L6:
  %t27 = phi i64 [ 1, %L4 ], [ %t26, %L5 ]
  %t28 = trunc i64 %t27 to i32
  ret i32 %t28
L7:
  ret i32 0
}

define internal i32 @promote_to_i64(ptr %t0, ptr %t1, ptr %t2, ptr %t3) {
entry:
  %t4 = call i32 @val_is_ptr(ptr %t1)
  %t5 = sext i32 %t4 to i64
  %t6 = icmp ne i64 %t5, 0
  br i1 %t6, label %L0, label %L1
L0:
  %t7 = alloca i64
  %t8 = call i32 @new_reg(ptr %t0)
  %t9 = sext i32 %t8 to i64
  store i64 %t9, ptr %t7
  %t10 = getelementptr i8, ptr %t0, i64 0
  %t11 = load i64, ptr %t10
  %t12 = getelementptr [34 x i8], ptr @.str42, i64 0, i64 0
  %t13 = load i64, ptr %t7
  %t14 = getelementptr i8, ptr %t1, i64 0
  %t15 = load i64, ptr %t14
  call void (ptr, ...) @__c0c_emit(i64 %t11, ptr %t12, i64 %t13, i64 %t15)
  %t17 = getelementptr [6 x i8], ptr @.str43, i64 0, i64 0
  %t18 = load i64, ptr %t7
  %t19 = call i32 (ptr, ...) @snprintf(ptr %t2, ptr %t3, ptr %t17, i64 %t18)
  %t20 = sext i32 %t19 to i64
  %t21 = load i64, ptr %t7
  %t22 = sext i32 %t21 to i64
  %t23 = trunc i64 %t22 to i32
  ret i32 %t23
L3:
  br label %L2
L1:
  %t24 = call i32 @val_is_64bit(ptr %t1)
  %t25 = sext i32 %t24 to i64
  %t26 = icmp ne i64 %t25, 0
  br i1 %t26, label %L4, label %L5
L4:
  %t27 = getelementptr i8, ptr %t1, i64 0
  %t28 = load i64, ptr %t27
  %t30 = ptrtoint ptr %t3 to i64
  %t31 = sext i32 1 to i64
  %t29 = sub i64 %t30, %t31
  %t32 = call ptr @strncpy(ptr %t2, i64 %t28, i64 %t29)
  %t34 = ptrtoint ptr %t3 to i64
  %t35 = sext i32 1 to i64
  %t33 = sub i64 %t34, %t35
  %t36 = getelementptr ptr, ptr %t2, i64 %t33
  %t37 = sext i32 0 to i64
  store i64 %t37, ptr %t36
  %t39 = sext i32 1 to i64
  %t38 = sub i64 0, %t39
  %t40 = trunc i64 %t38 to i32
  ret i32 %t40
L7:
  br label %L6
L5:
  %t41 = alloca i64
  %t42 = call i32 @new_reg(ptr %t0)
  %t43 = sext i32 %t42 to i64
  store i64 %t43, ptr %t41
  %t44 = getelementptr i8, ptr %t0, i64 0
  %t45 = load i64, ptr %t44
  %t46 = getelementptr [30 x i8], ptr @.str44, i64 0, i64 0
  %t47 = load i64, ptr %t41
  %t48 = getelementptr i8, ptr %t1, i64 0
  %t49 = load i64, ptr %t48
  call void (ptr, ...) @__c0c_emit(i64 %t45, ptr %t46, i64 %t47, i64 %t49)
  %t51 = getelementptr [6 x i8], ptr @.str45, i64 0, i64 0
  %t52 = load i64, ptr %t41
  %t53 = call i32 (ptr, ...) @snprintf(ptr %t2, ptr %t3, ptr %t51, i64 %t52)
  %t54 = sext i32 %t53 to i64
  %t55 = load i64, ptr %t41
  %t56 = sext i32 %t55 to i64
  %t57 = trunc i64 %t56 to i32
  ret i32 %t57
L8:
  br label %L6
L6:
  br label %L2
L2:
  ret i32 0
}

define internal i32 @emit_cond(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca i64
  %t3 = call i32 @new_reg(ptr %t0)
  %t4 = sext i32 %t3 to i64
  store i64 %t4, ptr %t2
  %t5 = call i32 @val_is_ptr(ptr %t1)
  %t6 = sext i32 %t5 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L1
L0:
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = load i64, ptr %t8
  %t10 = getelementptr [32 x i8], ptr @.str46, i64 0, i64 0
  %t11 = load i64, ptr %t2
  %t12 = getelementptr i8, ptr %t1, i64 0
  %t13 = load i64, ptr %t12
  call void (ptr, ...) @__c0c_emit(i64 %t9, ptr %t10, i64 %t11, i64 %t13)
  br label %L2
L1:
  %t15 = getelementptr i8, ptr %t1, i64 0
  %t16 = load i64, ptr %t15
  %t17 = call i32 @type_is_fp(i64 %t16)
  %t18 = sext i32 %t17 to i64
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L3, label %L4
L3:
  %t20 = getelementptr i8, ptr %t0, i64 0
  %t21 = load i64, ptr %t20
  %t22 = getelementptr [35 x i8], ptr @.str47, i64 0, i64 0
  %t23 = load i64, ptr %t2
  %t24 = getelementptr i8, ptr %t1, i64 0
  %t25 = load i64, ptr %t24
  call void (ptr, ...) @__c0c_emit(i64 %t21, ptr %t22, i64 %t23, i64 %t25)
  br label %L5
L4:
  %t27 = alloca ptr
  %t28 = load ptr, ptr %t27
  %t29 = call i32 @promote_to_i64(ptr %t0, ptr %t1, ptr %t28, i64 64)
  %t30 = sext i32 %t29 to i64
  %t31 = getelementptr i8, ptr %t0, i64 0
  %t32 = load i64, ptr %t31
  %t33 = getelementptr [29 x i8], ptr @.str48, i64 0, i64 0
  %t34 = load i64, ptr %t2
  %t35 = load ptr, ptr %t27
  call void (ptr, ...) @__c0c_emit(i64 %t32, ptr %t33, i64 %t34, ptr %t35)
  br label %L5
L5:
  br label %L2
L2:
  %t37 = load i64, ptr %t2
  %t38 = sext i32 %t37 to i64
  %t39 = trunc i64 %t38 to i32
  ret i32 %t39
L6:
  ret i32 0
}

define internal i64 @emit_expr(ptr %t0, ptr %t1) {
entry:
  %t3 = ptrtoint ptr %t1 to i64
  %t4 = icmp eq i64 %t3, 0
  %t2 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t2, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = getelementptr [2 x i8], ptr @.str49, i64 0, i64 0
  %t7 = call ptr @default_int_type()
  %t8 = call i64 @make_val(ptr %t6, ptr %t7)
  ret i64 %t8
L3:
  br label %L2
L2:
  %t9 = alloca i64
  %t10 = getelementptr i8, ptr %t1, i64 0
  %t11 = load i64, ptr %t10
  store i64 %t11, ptr %t9
  %t12 = load i64, ptr %t9
  %t14 = sext i32 %t12 to i64
  %t13 = add i64 %t14, 0
  %t15 = getelementptr i8, ptr %t1, i64 0
  %t16 = load i64, ptr %t15
  %t17 = add i64 %t16, 0
  switch i64 %t17, label %L29 [
    i64 19, label %L5
    i64 20, label %L6
    i64 21, label %L7
    i64 22, label %L8
    i64 23, label %L9
    i64 24, label %L10
    i64 25, label %L11
    i64 26, label %L12
    i64 27, label %L13
    i64 28, label %L14
    i64 38, label %L15
    i64 39, label %L16
    i64 40, label %L17
    i64 41, label %L18
    i64 36, label %L19
    i64 37, label %L20
    i64 33, label %L21
    i64 29, label %L22
    i64 30, label %L23
    i64 31, label %L24
    i64 32, label %L25
    i64 43, label %L26
    i64 34, label %L27
    i64 35, label %L28
  ]
L5:
  %t18 = alloca ptr
  %t19 = load ptr, ptr %t18
  %t20 = getelementptr [5 x i8], ptr @.str50, i64 0, i64 0
  %t21 = getelementptr i8, ptr %t1, i64 0
  %t22 = load i64, ptr %t21
  %t23 = call i32 (ptr, ...) @snprintf(ptr %t19, i64 8, ptr %t20, i64 %t22)
  %t24 = sext i32 %t23 to i64
  %t25 = load ptr, ptr %t18
  %t26 = call ptr @default_int_type()
  %t27 = call i64 @make_val(ptr %t25, ptr %t26)
  ret i64 %t27
L30:
  br label %L6
L6:
  %t28 = alloca i64
  %t29 = call i32 @new_reg(ptr %t0)
  %t30 = sext i32 %t29 to i64
  store i64 %t30, ptr %t28
  %t31 = getelementptr i8, ptr %t0, i64 0
  %t32 = load i64, ptr %t31
  %t33 = getelementptr [31 x i8], ptr @.str51, i64 0, i64 0
  %t34 = load i64, ptr %t28
  %t35 = getelementptr i8, ptr %t1, i64 0
  %t36 = load i64, ptr %t35
  call void (ptr, ...) @__c0c_emit(i64 %t32, ptr %t33, i64 %t34, i64 %t36)
  %t38 = alloca ptr
  %t39 = load ptr, ptr %t38
  %t40 = getelementptr [6 x i8], ptr @.str52, i64 0, i64 0
  %t41 = load i64, ptr %t28
  %t42 = call i32 (ptr, ...) @snprintf(ptr %t39, i64 8, ptr %t40, i64 %t41)
  %t43 = sext i32 %t42 to i64
  %t44 = alloca i64
  %t45 = sext i32 0 to i64
  store i64 %t45, ptr %t44
  %t46 = load ptr, ptr %t38
  %t47 = call i64 @make_val(ptr %t46, ptr %t44)
  ret i64 %t47
L31:
  br label %L7
L7:
  %t48 = alloca ptr
  %t49 = load ptr, ptr %t48
  %t50 = getelementptr [5 x i8], ptr @.str53, i64 0, i64 0
  %t51 = getelementptr i8, ptr %t1, i64 0
  %t52 = load i64, ptr %t51
  %t53 = call i32 (ptr, ...) @snprintf(ptr %t49, i64 8, ptr %t50, i64 %t52)
  %t54 = sext i32 %t53 to i64
  %t55 = alloca i64
  %t56 = sext i32 0 to i64
  store i64 %t56, ptr %t55
  %t57 = load ptr, ptr %t48
  %t58 = call i64 @make_val(ptr %t57, ptr %t55)
  ret i64 %t58
L32:
  br label %L8
L8:
  %t59 = alloca i64
  %t60 = getelementptr i8, ptr %t1, i64 0
  %t61 = load i64, ptr %t60
  %t62 = call i32 @intern_string(ptr %t0, i64 %t61)
  %t63 = sext i32 %t62 to i64
  store i64 %t63, ptr %t59
  %t64 = alloca i64
  %t65 = call i32 @new_reg(ptr %t0)
  %t66 = sext i32 %t65 to i64
  store i64 %t66, ptr %t64
  %t67 = alloca i64
  %t68 = getelementptr i8, ptr %t1, i64 0
  %t69 = load i64, ptr %t68
  %t70 = call i32 @str_literal_len(i64 %t69)
  %t71 = sext i32 %t70 to i64
  store i64 %t71, ptr %t67
  %t72 = getelementptr i8, ptr %t0, i64 0
  %t73 = load i64, ptr %t72
  %t74 = getelementptr [62 x i8], ptr @.str54, i64 0, i64 0
  %t75 = load i64, ptr %t64
  %t76 = load i64, ptr %t67
  %t77 = load i64, ptr %t59
  call void (ptr, ...) @__c0c_emit(i64 %t73, ptr %t74, i64 %t75, i64 %t76, i64 %t77)
  %t79 = alloca ptr
  %t80 = load ptr, ptr %t79
  %t81 = getelementptr [6 x i8], ptr @.str55, i64 0, i64 0
  %t82 = load i64, ptr %t64
  %t83 = call i32 (ptr, ...) @snprintf(ptr %t80, i64 8, ptr %t81, i64 %t82)
  %t84 = sext i32 %t83 to i64
  %t85 = load ptr, ptr %t79
  %t86 = call ptr @default_ptr_type()
  %t87 = call i64 @make_val(ptr %t85, ptr %t86)
  ret i64 %t87
L33:
  br label %L9
L9:
  %t88 = alloca ptr
  %t89 = getelementptr i8, ptr %t1, i64 0
  %t90 = load i64, ptr %t89
  %t91 = call ptr @find_local(ptr %t0, i64 %t90)
  store ptr %t91, ptr %t88
  %t92 = load ptr, ptr %t88
  %t93 = icmp ne ptr %t92, null
  br i1 %t93, label %L34, label %L36
L34:
  %t94 = load ptr, ptr %t88
  %t95 = getelementptr i8, ptr %t94, i64 0
  %t96 = load i64, ptr %t95
  %t97 = icmp ne i64 %t96, 0
  br i1 %t97, label %L37, label %L39
L37:
  %t98 = load ptr, ptr %t88
  %t99 = getelementptr i8, ptr %t98, i64 0
  %t100 = load i64, ptr %t99
  %t101 = load ptr, ptr %t88
  %t102 = getelementptr i8, ptr %t101, i64 0
  %t103 = load i64, ptr %t102
  %t104 = call i64 @make_val(i64 %t100, i64 %t103)
  ret i64 %t104
L40:
  br label %L39
L39:
  %t105 = alloca i64
  %t106 = call i32 @new_reg(ptr %t0)
  %t107 = sext i32 %t106 to i64
  store i64 %t107, ptr %t105
  %t108 = alloca ptr
  %t109 = alloca ptr
  %t110 = load ptr, ptr %t88
  %t111 = getelementptr i8, ptr %t110, i64 0
  %t112 = load i64, ptr %t111
  %t113 = icmp ne i64 %t112, 0
  br i1 %t113, label %L41, label %L42
L41:
  %t114 = load ptr, ptr %t88
  %t115 = getelementptr i8, ptr %t114, i64 0
  %t116 = load i64, ptr %t115
  %t117 = inttoptr i64 %t116 to ptr
  %t118 = getelementptr i8, ptr %t117, i64 0
  %t119 = load i64, ptr %t118
  %t121 = sext i32 15 to i64
  %t120 = icmp eq i64 %t119, %t121
  %t122 = zext i1 %t120 to i64
  %t123 = icmp ne i64 %t122, 0
  br i1 %t123, label %L44, label %L45
L44:
  br label %L46
L45:
  %t124 = load ptr, ptr %t88
  %t125 = getelementptr i8, ptr %t124, i64 0
  %t126 = load i64, ptr %t125
  %t127 = inttoptr i64 %t126 to ptr
  %t128 = getelementptr i8, ptr %t127, i64 0
  %t129 = load i64, ptr %t128
  %t131 = sext i32 16 to i64
  %t130 = icmp eq i64 %t129, %t131
  %t132 = zext i1 %t130 to i64
  %t133 = icmp ne i64 %t132, 0
  %t134 = zext i1 %t133 to i64
  br label %L46
L46:
  %t135 = phi i64 [ 1, %L44 ], [ %t134, %L45 ]
  %t136 = icmp ne i64 %t135, 0
  %t137 = zext i1 %t136 to i64
  br label %L43
L42:
  br label %L43
L43:
  %t138 = phi i64 [ %t137, %L41 ], [ 0, %L42 ]
  %t139 = icmp ne i64 %t138, 0
  br i1 %t139, label %L47, label %L48
L47:
  %t140 = getelementptr [4 x i8], ptr @.str56, i64 0, i64 0
  store ptr %t140, ptr %t108
  %t141 = call ptr @default_ptr_type()
  store ptr %t141, ptr %t109
  br label %L49
L48:
  %t142 = load ptr, ptr %t88
  %t143 = getelementptr i8, ptr %t142, i64 0
  %t144 = load i64, ptr %t143
  %t145 = icmp ne i64 %t144, 0
  br i1 %t145, label %L50, label %L51
L50:
  %t146 = load ptr, ptr %t88
  %t147 = getelementptr i8, ptr %t146, i64 0
  %t148 = load i64, ptr %t147
  %t149 = call i32 @type_is_fp(i64 %t148)
  %t150 = sext i32 %t149 to i64
  %t151 = icmp ne i64 %t150, 0
  %t152 = zext i1 %t151 to i64
  br label %L52
L51:
  br label %L52
L52:
  %t153 = phi i64 [ %t152, %L50 ], [ 0, %L51 ]
  %t154 = icmp ne i64 %t153, 0
  br i1 %t154, label %L53, label %L54
L53:
  %t155 = load ptr, ptr %t88
  %t156 = getelementptr i8, ptr %t155, i64 0
  %t157 = load i64, ptr %t156
  %t158 = call ptr @llvm_type(i64 %t157)
  store ptr %t158, ptr %t108
  %t159 = load ptr, ptr %t88
  %t160 = getelementptr i8, ptr %t159, i64 0
  %t161 = load i64, ptr %t160
  store i64 %t161, ptr %t109
  br label %L55
L54:
  %t162 = getelementptr [4 x i8], ptr @.str57, i64 0, i64 0
  store ptr %t162, ptr %t108
  %t163 = load ptr, ptr %t88
  %t164 = getelementptr i8, ptr %t163, i64 0
  %t165 = load i64, ptr %t164
  %t166 = icmp ne i64 %t165, 0
  br i1 %t166, label %L56, label %L57
L56:
  %t167 = load ptr, ptr %t88
  %t168 = getelementptr i8, ptr %t167, i64 0
  %t169 = load i64, ptr %t168
  br label %L58
L57:
  %t170 = call ptr @default_i64_type()
  %t171 = ptrtoint ptr %t170 to i64
  br label %L58
L58:
  %t172 = phi i64 [ %t169, %L56 ], [ %t171, %L57 ]
  store i64 %t172, ptr %t109
  br label %L55
L55:
  br label %L49
L49:
  %t173 = getelementptr i8, ptr %t0, i64 0
  %t174 = load i64, ptr %t173
  %t175 = getelementptr [27 x i8], ptr @.str58, i64 0, i64 0
  %t176 = load i64, ptr %t105
  %t177 = load ptr, ptr %t108
  %t178 = load ptr, ptr %t88
  %t179 = getelementptr i8, ptr %t178, i64 0
  %t180 = load i64, ptr %t179
  call void (ptr, ...) @__c0c_emit(i64 %t174, ptr %t175, i64 %t176, ptr %t177, i64 %t180)
  %t182 = alloca ptr
  %t183 = load ptr, ptr %t182
  %t184 = getelementptr [6 x i8], ptr @.str59, i64 0, i64 0
  %t185 = load i64, ptr %t105
  %t186 = call i32 (ptr, ...) @snprintf(ptr %t183, i64 8, ptr %t184, i64 %t185)
  %t187 = sext i32 %t186 to i64
  %t188 = load ptr, ptr %t182
  %t189 = load ptr, ptr %t109
  %t190 = call i64 @make_val(ptr %t188, ptr %t189)
  ret i64 %t190
L59:
  br label %L36
L36:
  %t191 = alloca ptr
  %t192 = getelementptr i8, ptr %t1, i64 0
  %t193 = load i64, ptr %t192
  %t194 = call ptr @find_global(ptr %t0, i64 %t193)
  store ptr %t194, ptr %t191
  %t195 = load ptr, ptr %t191
  %t196 = ptrtoint ptr %t195 to i64
  %t197 = icmp ne i64 %t196, 0
  br i1 %t197, label %L60, label %L61
L60:
  %t198 = load ptr, ptr %t191
  %t199 = getelementptr i8, ptr %t198, i64 0
  %t200 = load i64, ptr %t199
  %t201 = icmp ne i64 %t200, 0
  %t202 = zext i1 %t201 to i64
  br label %L62
L61:
  br label %L62
L62:
  %t203 = phi i64 [ %t202, %L60 ], [ 0, %L61 ]
  %t204 = icmp ne i64 %t203, 0
  br i1 %t204, label %L63, label %L64
L63:
  %t205 = load ptr, ptr %t191
  %t206 = getelementptr i8, ptr %t205, i64 0
  %t207 = load i64, ptr %t206
  %t208 = inttoptr i64 %t207 to ptr
  %t209 = getelementptr i8, ptr %t208, i64 0
  %t210 = load i64, ptr %t209
  %t212 = sext i32 17 to i64
  %t211 = icmp ne i64 %t210, %t212
  %t213 = zext i1 %t211 to i64
  %t214 = icmp ne i64 %t213, 0
  %t215 = zext i1 %t214 to i64
  br label %L65
L64:
  br label %L65
L65:
  %t216 = phi i64 [ %t215, %L63 ], [ 0, %L64 ]
  %t217 = icmp ne i64 %t216, 0
  br i1 %t217, label %L66, label %L68
L66:
  %t218 = alloca i64
  %t219 = call i32 @new_reg(ptr %t0)
  %t220 = sext i32 %t219 to i64
  store i64 %t220, ptr %t218
  %t221 = alloca ptr
  %t222 = alloca ptr
  %t223 = load ptr, ptr %t191
  %t224 = getelementptr i8, ptr %t223, i64 0
  %t225 = load i64, ptr %t224
  %t226 = inttoptr i64 %t225 to ptr
  %t227 = getelementptr i8, ptr %t226, i64 0
  %t228 = load i64, ptr %t227
  %t230 = sext i32 15 to i64
  %t229 = icmp eq i64 %t228, %t230
  %t231 = zext i1 %t229 to i64
  %t232 = icmp ne i64 %t231, 0
  br i1 %t232, label %L69, label %L70
L69:
  br label %L71
L70:
  %t233 = load ptr, ptr %t191
  %t234 = getelementptr i8, ptr %t233, i64 0
  %t235 = load i64, ptr %t234
  %t236 = inttoptr i64 %t235 to ptr
  %t237 = getelementptr i8, ptr %t236, i64 0
  %t238 = load i64, ptr %t237
  %t240 = sext i32 16 to i64
  %t239 = icmp eq i64 %t238, %t240
  %t241 = zext i1 %t239 to i64
  %t242 = icmp ne i64 %t241, 0
  %t243 = zext i1 %t242 to i64
  br label %L71
L71:
  %t244 = phi i64 [ 1, %L69 ], [ %t243, %L70 ]
  %t245 = icmp ne i64 %t244, 0
  br i1 %t245, label %L72, label %L73
L72:
  %t246 = getelementptr [4 x i8], ptr @.str60, i64 0, i64 0
  store ptr %t246, ptr %t221
  %t247 = call ptr @default_ptr_type()
  store ptr %t247, ptr %t222
  br label %L74
L73:
  %t248 = load ptr, ptr %t191
  %t249 = getelementptr i8, ptr %t248, i64 0
  %t250 = load i64, ptr %t249
  %t251 = call i32 @type_is_fp(i64 %t250)
  %t252 = sext i32 %t251 to i64
  %t253 = icmp ne i64 %t252, 0
  br i1 %t253, label %L75, label %L76
L75:
  %t254 = load ptr, ptr %t191
  %t255 = getelementptr i8, ptr %t254, i64 0
  %t256 = load i64, ptr %t255
  %t257 = call ptr @llvm_type(i64 %t256)
  store ptr %t257, ptr %t221
  %t258 = load ptr, ptr %t191
  %t259 = getelementptr i8, ptr %t258, i64 0
  %t260 = load i64, ptr %t259
  store i64 %t260, ptr %t222
  br label %L77
L76:
  %t261 = getelementptr [4 x i8], ptr @.str61, i64 0, i64 0
  store ptr %t261, ptr %t221
  %t262 = load ptr, ptr %t191
  %t263 = getelementptr i8, ptr %t262, i64 0
  %t264 = load i64, ptr %t263
  %t265 = icmp ne i64 %t264, 0
  br i1 %t265, label %L78, label %L79
L78:
  %t266 = load ptr, ptr %t191
  %t267 = getelementptr i8, ptr %t266, i64 0
  %t268 = load i64, ptr %t267
  br label %L80
L79:
  %t269 = call ptr @default_i64_type()
  %t270 = ptrtoint ptr %t269 to i64
  br label %L80
L80:
  %t271 = phi i64 [ %t268, %L78 ], [ %t270, %L79 ]
  store i64 %t271, ptr %t222
  br label %L77
L77:
  br label %L74
L74:
  %t272 = getelementptr i8, ptr %t0, i64 0
  %t273 = load i64, ptr %t272
  %t274 = getelementptr [28 x i8], ptr @.str62, i64 0, i64 0
  %t275 = load i64, ptr %t218
  %t276 = load ptr, ptr %t221
  %t277 = getelementptr i8, ptr %t1, i64 0
  %t278 = load i64, ptr %t277
  call void (ptr, ...) @__c0c_emit(i64 %t273, ptr %t274, i64 %t275, ptr %t276, i64 %t278)
  %t280 = alloca ptr
  %t281 = load ptr, ptr %t280
  %t282 = getelementptr [6 x i8], ptr @.str63, i64 0, i64 0
  %t283 = load i64, ptr %t218
  %t284 = call i32 (ptr, ...) @snprintf(ptr %t281, i64 8, ptr %t282, i64 %t283)
  %t285 = sext i32 %t284 to i64
  %t286 = load ptr, ptr %t280
  %t287 = load ptr, ptr %t222
  %t288 = call i64 @make_val(ptr %t286, ptr %t287)
  ret i64 %t288
L81:
  br label %L68
L68:
  %t289 = alloca ptr
  %t290 = load ptr, ptr %t289
  %t291 = getelementptr [4 x i8], ptr @.str64, i64 0, i64 0
  %t292 = getelementptr i8, ptr %t1, i64 0
  %t293 = load i64, ptr %t292
  %t294 = call i32 (ptr, ...) @snprintf(ptr %t290, i64 8, ptr %t291, i64 %t293)
  %t295 = sext i32 %t294 to i64
  %t296 = load ptr, ptr %t289
  %t297 = call ptr @default_ptr_type()
  %t298 = call i64 @make_val(ptr %t296, ptr %t297)
  ret i64 %t298
L82:
  br label %L10
L10:
  %t299 = alloca ptr
  %t300 = getelementptr i8, ptr %t1, i64 0
  %t301 = load i64, ptr %t300
  %t302 = inttoptr i64 %t301 to ptr
  %t303 = sext i32 0 to i64
  %t304 = getelementptr ptr, ptr %t302, i64 %t303
  %t305 = load ptr, ptr %t304
  store ptr %t305, ptr %t299
  %t306 = alloca ptr
  %t307 = call ptr @default_int_type()
  store ptr %t307, ptr %t306
  %t308 = alloca ptr
  %t309 = getelementptr i8, ptr %t1, i64 0
  %t310 = load i64, ptr %t309
  %t312 = sext i32 8 to i64
  %t311 = mul i64 %t310, %t312
  %t313 = call ptr @malloc(i64 %t311)
  store ptr %t313, ptr %t308
  %t314 = alloca ptr
  %t315 = getelementptr i8, ptr %t1, i64 0
  %t316 = load i64, ptr %t315
  %t318 = sext i32 8 to i64
  %t317 = mul i64 %t316, %t318
  %t319 = call ptr @malloc(i64 %t317)
  store ptr %t319, ptr %t314
  %t320 = alloca i64
  %t321 = sext i32 1 to i64
  store i64 %t321, ptr %t320
  br label %L83
L83:
  %t322 = load i64, ptr %t320
  %t323 = getelementptr i8, ptr %t1, i64 0
  %t324 = load i64, ptr %t323
  %t326 = sext i32 %t322 to i64
  %t325 = icmp slt i64 %t326, %t324
  %t327 = zext i1 %t325 to i64
  %t328 = icmp ne i64 %t327, 0
  br i1 %t328, label %L84, label %L86
L84:
  %t329 = alloca i64
  %t330 = getelementptr i8, ptr %t1, i64 0
  %t331 = load i64, ptr %t330
  %t332 = load i64, ptr %t320
  %t333 = inttoptr i64 %t331 to ptr
  %t334 = sext i32 %t332 to i64
  %t335 = getelementptr ptr, ptr %t333, i64 %t334
  %t336 = load ptr, ptr %t335
  %t337 = call i64 @emit_expr(ptr %t0, ptr %t336)
  store i64 %t337, ptr %t329
  %t338 = getelementptr i8, ptr %t329, i64 0
  %t339 = load i64, ptr %t338
  %t340 = call ptr @strdup(i64 %t339)
  %t341 = load ptr, ptr %t308
  %t342 = load i64, ptr %t320
  %t344 = sext i32 %t342 to i64
  %t343 = getelementptr ptr, ptr %t341, i64 %t344
  store ptr %t340, ptr %t343
  %t345 = getelementptr i8, ptr %t329, i64 0
  %t346 = load i64, ptr %t345
  %t347 = load ptr, ptr %t314
  %t348 = load i64, ptr %t320
  %t350 = sext i32 %t348 to i64
  %t349 = getelementptr ptr, ptr %t347, i64 %t350
  store i64 %t346, ptr %t349
  br label %L85
L85:
  %t351 = load i64, ptr %t320
  %t353 = sext i32 %t351 to i64
  %t352 = add i64 %t353, 1
  store i64 %t352, ptr %t320
  br label %L83
L86:
  %t354 = alloca ptr
  %t355 = sext i32 0 to i64
  store i64 %t355, ptr %t354
  %t356 = load ptr, ptr %t299
  %t357 = getelementptr i8, ptr %t356, i64 0
  %t358 = load i64, ptr %t357
  %t360 = sext i32 23 to i64
  %t359 = icmp eq i64 %t358, %t360
  %t361 = zext i1 %t359 to i64
  %t362 = icmp ne i64 %t361, 0
  br i1 %t362, label %L87, label %L88
L87:
  %t363 = load ptr, ptr %t354
  %t364 = getelementptr [4 x i8], ptr @.str65, i64 0, i64 0
  %t365 = load ptr, ptr %t299
  %t366 = getelementptr i8, ptr %t365, i64 0
  %t367 = load i64, ptr %t366
  %t368 = call i32 (ptr, ...) @snprintf(ptr %t363, i64 8, ptr %t364, i64 %t367)
  %t369 = sext i32 %t368 to i64
  %t370 = alloca ptr
  %t371 = load ptr, ptr %t299
  %t372 = getelementptr i8, ptr %t371, i64 0
  %t373 = load i64, ptr %t372
  %t374 = call ptr @find_global(ptr %t0, i64 %t373)
  store ptr %t374, ptr %t370
  %t375 = load ptr, ptr %t370
  %t376 = ptrtoint ptr %t375 to i64
  %t377 = icmp ne i64 %t376, 0
  br i1 %t377, label %L90, label %L91
L90:
  %t378 = load ptr, ptr %t370
  %t379 = getelementptr i8, ptr %t378, i64 0
  %t380 = load i64, ptr %t379
  %t381 = icmp ne i64 %t380, 0
  %t382 = zext i1 %t381 to i64
  br label %L92
L91:
  br label %L92
L92:
  %t383 = phi i64 [ %t382, %L90 ], [ 0, %L91 ]
  %t384 = icmp ne i64 %t383, 0
  br i1 %t384, label %L93, label %L94
L93:
  %t385 = load ptr, ptr %t370
  %t386 = getelementptr i8, ptr %t385, i64 0
  %t387 = load i64, ptr %t386
  %t388 = inttoptr i64 %t387 to ptr
  %t389 = getelementptr i8, ptr %t388, i64 0
  %t390 = load i64, ptr %t389
  %t392 = sext i32 17 to i64
  %t391 = icmp eq i64 %t390, %t392
  %t393 = zext i1 %t391 to i64
  %t394 = icmp ne i64 %t393, 0
  %t395 = zext i1 %t394 to i64
  br label %L95
L94:
  br label %L95
L95:
  %t396 = phi i64 [ %t395, %L93 ], [ 0, %L94 ]
  %t397 = icmp ne i64 %t396, 0
  br i1 %t397, label %L96, label %L97
L96:
  %t398 = load ptr, ptr %t370
  %t399 = getelementptr i8, ptr %t398, i64 0
  %t400 = load i64, ptr %t399
  %t401 = inttoptr i64 %t400 to ptr
  %t402 = getelementptr i8, ptr %t401, i64 0
  %t403 = load i64, ptr %t402
  store i64 %t403, ptr %t306
  br label %L98
L97:
  %t404 = alloca ptr
  %t405 = sext i32 0 to i64
  store i64 %t405, ptr %t404
  %t406 = alloca i64
  %t407 = sext i32 0 to i64
  store i64 %t407, ptr %t406
  br label %L99
L99:
  %t408 = load ptr, ptr %t404
  %t409 = load i64, ptr %t406
  %t410 = sext i32 %t409 to i64
  %t411 = getelementptr ptr, ptr %t408, i64 %t410
  %t412 = load ptr, ptr %t411
  %t413 = icmp ne ptr %t412, null
  br i1 %t413, label %L100, label %L102
L100:
  %t414 = load ptr, ptr %t299
  %t415 = getelementptr i8, ptr %t414, i64 0
  %t416 = load i64, ptr %t415
  %t417 = load ptr, ptr %t404
  %t418 = load i64, ptr %t406
  %t419 = sext i32 %t418 to i64
  %t420 = getelementptr ptr, ptr %t417, i64 %t419
  %t421 = load ptr, ptr %t420
  %t422 = call i32 @strcmp(i64 %t416, ptr %t421)
  %t423 = sext i32 %t422 to i64
  %t425 = sext i32 0 to i64
  %t424 = icmp eq i64 %t423, %t425
  %t426 = zext i1 %t424 to i64
  %t427 = icmp ne i64 %t426, 0
  br i1 %t427, label %L103, label %L105
L103:
  %t428 = call ptr @default_ptr_type()
  store ptr %t428, ptr %t306
  br label %L102
L106:
  br label %L105
L105:
  br label %L101
L101:
  %t429 = load i64, ptr %t406
  %t431 = sext i32 %t429 to i64
  %t430 = add i64 %t431, 1
  store i64 %t430, ptr %t406
  br label %L99
L102:
  %t432 = alloca ptr
  %t433 = sext i32 0 to i64
  store i64 %t433, ptr %t432
  %t434 = alloca i64
  %t435 = sext i32 0 to i64
  store i64 %t435, ptr %t434
  br label %L107
L107:
  %t436 = load ptr, ptr %t432
  %t437 = load i64, ptr %t434
  %t438 = sext i32 %t437 to i64
  %t439 = getelementptr ptr, ptr %t436, i64 %t438
  %t440 = load ptr, ptr %t439
  %t441 = icmp ne ptr %t440, null
  br i1 %t441, label %L108, label %L110
L108:
  %t442 = load ptr, ptr %t299
  %t443 = getelementptr i8, ptr %t442, i64 0
  %t444 = load i64, ptr %t443
  %t445 = load ptr, ptr %t432
  %t446 = load i64, ptr %t434
  %t447 = sext i32 %t446 to i64
  %t448 = getelementptr ptr, ptr %t445, i64 %t447
  %t449 = load ptr, ptr %t448
  %t450 = call i32 @strcmp(i64 %t444, ptr %t449)
  %t451 = sext i32 %t450 to i64
  %t453 = sext i32 0 to i64
  %t452 = icmp eq i64 %t451, %t453
  %t454 = zext i1 %t452 to i64
  %t455 = icmp ne i64 %t454, 0
  br i1 %t455, label %L111, label %L113
L111:
  %t456 = call ptr @default_i64_type()
  store ptr %t456, ptr %t306
  br label %L110
L114:
  br label %L113
L113:
  br label %L109
L109:
  %t457 = load i64, ptr %t434
  %t459 = sext i32 %t457 to i64
  %t458 = add i64 %t459, 1
  store i64 %t458, ptr %t434
  br label %L107
L110:
  %t460 = alloca i64
  %t461 = sext i32 0 to i64
  store i64 %t461, ptr %t460
  %t462 = alloca ptr
  %t463 = sext i32 0 to i64
  store i64 %t463, ptr %t462
  %t464 = alloca i64
  %t465 = sext i32 0 to i64
  store i64 %t465, ptr %t464
  br label %L115
L115:
  %t466 = load ptr, ptr %t462
  %t467 = load i64, ptr %t464
  %t468 = sext i32 %t467 to i64
  %t469 = getelementptr ptr, ptr %t466, i64 %t468
  %t470 = load ptr, ptr %t469
  %t471 = icmp ne ptr %t470, null
  br i1 %t471, label %L116, label %L118
L116:
  %t472 = load ptr, ptr %t299
  %t473 = getelementptr i8, ptr %t472, i64 0
  %t474 = load i64, ptr %t473
  %t475 = load ptr, ptr %t462
  %t476 = load i64, ptr %t464
  %t477 = sext i32 %t476 to i64
  %t478 = getelementptr ptr, ptr %t475, i64 %t477
  %t479 = load ptr, ptr %t478
  %t480 = call i32 @strcmp(i64 %t474, ptr %t479)
  %t481 = sext i32 %t480 to i64
  %t483 = sext i32 0 to i64
  %t482 = icmp eq i64 %t481, %t483
  %t484 = zext i1 %t482 to i64
  %t485 = icmp ne i64 %t484, 0
  br i1 %t485, label %L119, label %L121
L119:
  store ptr %t460, ptr %t306
  br label %L118
L122:
  br label %L121
L121:
  br label %L117
L117:
  %t486 = load i64, ptr %t464
  %t488 = sext i32 %t486 to i64
  %t487 = add i64 %t488, 1
  store i64 %t487, ptr %t464
  br label %L115
L118:
  br label %L98
L98:
  br label %L89
L88:
  %t489 = alloca i64
  %t490 = load ptr, ptr %t299
  %t491 = call i64 @emit_expr(ptr %t0, ptr %t490)
  store i64 %t491, ptr %t489
  %t492 = load ptr, ptr %t354
  %t493 = getelementptr i8, ptr %t489, i64 0
  %t494 = load i64, ptr %t493
  %t496 = sext i32 8 to i64
  %t497 = sext i32 1 to i64
  %t495 = sub i64 %t496, %t497
  %t498 = call ptr @strncpy(ptr %t492, i64 %t494, i64 %t495)
  br label %L89
L89:
  %t499 = alloca i64
  %t500 = sext i32 0 to i64
  store i64 %t500, ptr %t499
  %t501 = load ptr, ptr %t299
  %t502 = getelementptr i8, ptr %t501, i64 0
  %t503 = load i64, ptr %t502
  %t505 = sext i32 23 to i64
  %t504 = icmp eq i64 %t503, %t505
  %t506 = zext i1 %t504 to i64
  %t507 = icmp ne i64 %t506, 0
  br i1 %t507, label %L123, label %L125
L123:
  %t508 = alloca ptr
  %t509 = sext i32 0 to i64
  store i64 %t509, ptr %t508
  %t510 = alloca i64
  %t511 = sext i32 0 to i64
  store i64 %t511, ptr %t510
  br label %L126
L126:
  %t512 = load ptr, ptr %t508
  %t513 = load i64, ptr %t510
  %t514 = sext i32 %t513 to i64
  %t515 = getelementptr ptr, ptr %t512, i64 %t514
  %t516 = load ptr, ptr %t515
  %t517 = icmp ne ptr %t516, null
  br i1 %t517, label %L127, label %L129
L127:
  %t518 = load ptr, ptr %t299
  %t519 = getelementptr i8, ptr %t518, i64 0
  %t520 = load i64, ptr %t519
  %t521 = load ptr, ptr %t508
  %t522 = load i64, ptr %t510
  %t523 = sext i32 %t522 to i64
  %t524 = getelementptr ptr, ptr %t521, i64 %t523
  %t525 = load ptr, ptr %t524
  %t526 = call i32 @strcmp(i64 %t520, ptr %t525)
  %t527 = sext i32 %t526 to i64
  %t529 = sext i32 0 to i64
  %t528 = icmp eq i64 %t527, %t529
  %t530 = zext i1 %t528 to i64
  %t531 = icmp ne i64 %t530, 0
  br i1 %t531, label %L130, label %L132
L130:
  %t532 = sext i32 1 to i64
  store i64 %t532, ptr %t499
  br label %L129
L133:
  br label %L132
L132:
  br label %L128
L128:
  %t533 = load i64, ptr %t510
  %t535 = sext i32 %t533 to i64
  %t534 = add i64 %t535, 1
  store i64 %t534, ptr %t510
  br label %L126
L129:
  br label %L125
L125:
  %t536 = alloca i64
  %t537 = call i32 @new_reg(ptr %t0)
  %t538 = sext i32 %t537 to i64
  store i64 %t538, ptr %t536
  %t539 = alloca ptr
  %t540 = load ptr, ptr %t306
  %t541 = call ptr @llvm_type(ptr %t540)
  store ptr %t541, ptr %t539
  %t542 = alloca i64
  %t543 = load ptr, ptr %t306
  %t544 = getelementptr i8, ptr %t543, i64 0
  %t545 = load i64, ptr %t544
  %t547 = sext i32 0 to i64
  %t546 = icmp eq i64 %t545, %t547
  %t548 = zext i1 %t546 to i64
  store i64 %t548, ptr %t542
  %t549 = load i64, ptr %t542
  %t550 = sext i32 %t549 to i64
  %t551 = icmp ne i64 %t550, 0
  br i1 %t551, label %L134, label %L135
L134:
  %t552 = load i64, ptr %t499
  %t553 = sext i32 %t552 to i64
  %t554 = icmp ne i64 %t553, 0
  %t555 = zext i1 %t554 to i64
  br label %L136
L135:
  br label %L136
L136:
  %t556 = phi i64 [ %t555, %L134 ], [ 0, %L135 ]
  %t557 = icmp ne i64 %t556, 0
  br i1 %t557, label %L137, label %L138
L137:
  %t558 = getelementptr i8, ptr %t0, i64 0
  %t559 = load i64, ptr %t558
  %t560 = getelementptr [27 x i8], ptr @.str66, i64 0, i64 0
  %t561 = load ptr, ptr %t354
  call void (ptr, ...) @__c0c_emit(i64 %t559, ptr %t560, ptr %t561)
  br label %L139
L138:
  %t563 = load i64, ptr %t542
  %t565 = sext i32 %t563 to i64
  %t564 = icmp ne i64 %t565, 0
  br i1 %t564, label %L140, label %L141
L140:
  %t566 = getelementptr i8, ptr %t0, i64 0
  %t567 = load i64, ptr %t566
  %t568 = getelementptr [16 x i8], ptr @.str67, i64 0, i64 0
  %t569 = load ptr, ptr %t354
  call void (ptr, ...) @__c0c_emit(i64 %t567, ptr %t568, ptr %t569)
  br label %L142
L141:
  %t571 = load i64, ptr %t499
  %t573 = sext i32 %t571 to i64
  %t572 = icmp ne i64 %t573, 0
  br i1 %t572, label %L143, label %L144
L143:
  %t574 = getelementptr i8, ptr %t0, i64 0
  %t575 = load i64, ptr %t574
  %t576 = getelementptr [33 x i8], ptr @.str68, i64 0, i64 0
  %t577 = load i64, ptr %t536
  %t578 = load ptr, ptr %t539
  %t579 = load ptr, ptr %t354
  call void (ptr, ...) @__c0c_emit(i64 %t575, ptr %t576, i64 %t577, ptr %t578, ptr %t579)
  br label %L145
L144:
  %t581 = getelementptr i8, ptr %t0, i64 0
  %t582 = load i64, ptr %t581
  %t583 = getelementptr [22 x i8], ptr @.str69, i64 0, i64 0
  %t584 = load i64, ptr %t536
  %t585 = load ptr, ptr %t539
  %t586 = load ptr, ptr %t354
  call void (ptr, ...) @__c0c_emit(i64 %t582, ptr %t583, i64 %t584, ptr %t585, ptr %t586)
  br label %L145
L145:
  br label %L142
L142:
  br label %L139
L139:
  %t588 = alloca i64
  %t589 = sext i32 1 to i64
  store i64 %t589, ptr %t588
  br label %L146
L146:
  %t590 = load i64, ptr %t588
  %t591 = getelementptr i8, ptr %t1, i64 0
  %t592 = load i64, ptr %t591
  %t594 = sext i32 %t590 to i64
  %t593 = icmp slt i64 %t594, %t592
  %t595 = zext i1 %t593 to i64
  %t596 = icmp ne i64 %t595, 0
  br i1 %t596, label %L147, label %L149
L147:
  %t597 = load i64, ptr %t588
  %t599 = sext i32 %t597 to i64
  %t600 = sext i32 1 to i64
  %t598 = icmp sgt i64 %t599, %t600
  %t601 = zext i1 %t598 to i64
  %t602 = icmp ne i64 %t601, 0
  br i1 %t602, label %L150, label %L152
L150:
  %t603 = getelementptr i8, ptr %t0, i64 0
  %t604 = load i64, ptr %t603
  %t605 = getelementptr [3 x i8], ptr @.str70, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t604, ptr %t605)
  br label %L152
L152:
  %t607 = alloca ptr
  %t608 = load ptr, ptr %t314
  %t609 = load i64, ptr %t588
  %t610 = sext i32 %t609 to i64
  %t611 = getelementptr ptr, ptr %t608, i64 %t610
  %t612 = load ptr, ptr %t611
  %t613 = ptrtoint ptr %t612 to i64
  %t614 = icmp ne i64 %t613, 0
  br i1 %t614, label %L153, label %L154
L153:
  %t615 = load ptr, ptr %t314
  %t616 = load i64, ptr %t588
  %t617 = sext i32 %t616 to i64
  %t618 = getelementptr ptr, ptr %t615, i64 %t617
  %t619 = load ptr, ptr %t618
  %t620 = getelementptr i8, ptr %t619, i64 0
  %t621 = load i64, ptr %t620
  %t623 = sext i32 15 to i64
  %t622 = icmp eq i64 %t621, %t623
  %t624 = zext i1 %t622 to i64
  %t625 = icmp ne i64 %t624, 0
  br i1 %t625, label %L156, label %L157
L156:
  br label %L158
L157:
  %t626 = load ptr, ptr %t314
  %t627 = load i64, ptr %t588
  %t628 = sext i32 %t627 to i64
  %t629 = getelementptr ptr, ptr %t626, i64 %t628
  %t630 = load ptr, ptr %t629
  %t631 = getelementptr i8, ptr %t630, i64 0
  %t632 = load i64, ptr %t631
  %t634 = sext i32 16 to i64
  %t633 = icmp eq i64 %t632, %t634
  %t635 = zext i1 %t633 to i64
  %t636 = icmp ne i64 %t635, 0
  %t637 = zext i1 %t636 to i64
  br label %L158
L158:
  %t638 = phi i64 [ 1, %L156 ], [ %t637, %L157 ]
  %t639 = icmp ne i64 %t638, 0
  %t640 = zext i1 %t639 to i64
  br label %L155
L154:
  br label %L155
L155:
  %t641 = phi i64 [ %t640, %L153 ], [ 0, %L154 ]
  %t642 = icmp ne i64 %t641, 0
  br i1 %t642, label %L159, label %L160
L159:
  %t643 = getelementptr [4 x i8], ptr @.str71, i64 0, i64 0
  store ptr %t643, ptr %t607
  br label %L161
L160:
  %t644 = load ptr, ptr %t314
  %t645 = load i64, ptr %t588
  %t646 = sext i32 %t645 to i64
  %t647 = getelementptr ptr, ptr %t644, i64 %t646
  %t648 = load ptr, ptr %t647
  %t649 = ptrtoint ptr %t648 to i64
  %t650 = icmp ne i64 %t649, 0
  br i1 %t650, label %L162, label %L163
L162:
  %t651 = load ptr, ptr %t314
  %t652 = load i64, ptr %t588
  %t653 = sext i32 %t652 to i64
  %t654 = getelementptr ptr, ptr %t651, i64 %t653
  %t655 = load ptr, ptr %t654
  %t656 = call i32 @type_is_fp(ptr %t655)
  %t657 = sext i32 %t656 to i64
  %t658 = icmp ne i64 %t657, 0
  %t659 = zext i1 %t658 to i64
  br label %L164
L163:
  br label %L164
L164:
  %t660 = phi i64 [ %t659, %L162 ], [ 0, %L163 ]
  %t661 = icmp ne i64 %t660, 0
  br i1 %t661, label %L165, label %L166
L165:
  %t662 = load ptr, ptr %t314
  %t663 = load i64, ptr %t588
  %t664 = sext i32 %t663 to i64
  %t665 = getelementptr ptr, ptr %t662, i64 %t664
  %t666 = load ptr, ptr %t665
  %t667 = call ptr @llvm_type(ptr %t666)
  store ptr %t667, ptr %t607
  br label %L167
L166:
  %t668 = getelementptr [4 x i8], ptr @.str72, i64 0, i64 0
  store ptr %t668, ptr %t607
  br label %L167
L167:
  br label %L161
L161:
  %t669 = getelementptr i8, ptr %t0, i64 0
  %t670 = load i64, ptr %t669
  %t671 = getelementptr [6 x i8], ptr @.str73, i64 0, i64 0
  %t672 = load ptr, ptr %t607
  %t673 = load ptr, ptr %t308
  %t674 = load i64, ptr %t588
  %t675 = sext i32 %t674 to i64
  %t676 = getelementptr ptr, ptr %t673, i64 %t675
  %t677 = load ptr, ptr %t676
  call void (ptr, ...) @__c0c_emit(i64 %t670, ptr %t671, ptr %t672, ptr %t677)
  br label %L148
L148:
  %t679 = load i64, ptr %t588
  %t681 = sext i32 %t679 to i64
  %t680 = add i64 %t681, 1
  store i64 %t680, ptr %t588
  br label %L146
L149:
  %t682 = getelementptr i8, ptr %t0, i64 0
  %t683 = load i64, ptr %t682
  %t684 = getelementptr [3 x i8], ptr @.str74, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t683, ptr %t684)
  %t686 = alloca i64
  %t687 = sext i32 1 to i64
  store i64 %t687, ptr %t686
  br label %L168
L168:
  %t688 = load i64, ptr %t686
  %t689 = getelementptr i8, ptr %t1, i64 0
  %t690 = load i64, ptr %t689
  %t692 = sext i32 %t688 to i64
  %t691 = icmp slt i64 %t692, %t690
  %t693 = zext i1 %t691 to i64
  %t694 = icmp ne i64 %t693, 0
  br i1 %t694, label %L169, label %L171
L169:
  %t695 = load ptr, ptr %t308
  %t696 = load i64, ptr %t686
  %t697 = sext i32 %t696 to i64
  %t698 = getelementptr ptr, ptr %t695, i64 %t697
  %t699 = load ptr, ptr %t698
  call void @free(ptr %t699)
  br label %L170
L170:
  %t701 = load i64, ptr %t686
  %t703 = sext i32 %t701 to i64
  %t702 = add i64 %t703, 1
  store i64 %t702, ptr %t686
  br label %L168
L171:
  %t704 = load ptr, ptr %t308
  call void @free(ptr %t704)
  %t706 = load ptr, ptr %t314
  call void @free(ptr %t706)
  %t708 = load i64, ptr %t542
  %t710 = sext i32 %t708 to i64
  %t709 = icmp ne i64 %t710, 0
  br i1 %t709, label %L172, label %L174
L172:
  %t711 = getelementptr [2 x i8], ptr @.str75, i64 0, i64 0
  %t712 = load ptr, ptr %t306
  %t713 = call i64 @make_val(ptr %t711, ptr %t712)
  ret i64 %t713
L175:
  br label %L174
L174:
  %t714 = alloca ptr
  %t715 = load ptr, ptr %t714
  %t716 = getelementptr [6 x i8], ptr @.str76, i64 0, i64 0
  %t717 = load i64, ptr %t536
  %t718 = call i32 (ptr, ...) @snprintf(ptr %t715, i64 8, ptr %t716, i64 %t717)
  %t719 = sext i32 %t718 to i64
  %t720 = alloca ptr
  %t721 = load ptr, ptr %t306
  store ptr %t721, ptr %t720
  %t722 = load ptr, ptr %t306
  %t723 = call i32 @type_is_fp(ptr %t722)
  %t724 = sext i32 %t723 to i64
  %t726 = icmp eq i64 %t724, 0
  %t725 = zext i1 %t726 to i64
  %t727 = icmp ne i64 %t725, 0
  br i1 %t727, label %L176, label %L177
L176:
  %t728 = load ptr, ptr %t306
  %t729 = getelementptr i8, ptr %t728, i64 0
  %t730 = load i64, ptr %t729
  %t732 = sext i32 15 to i64
  %t731 = icmp ne i64 %t730, %t732
  %t733 = zext i1 %t731 to i64
  %t734 = icmp ne i64 %t733, 0
  %t735 = zext i1 %t734 to i64
  br label %L178
L177:
  br label %L178
L178:
  %t736 = phi i64 [ %t735, %L176 ], [ 0, %L177 ]
  %t737 = icmp ne i64 %t736, 0
  br i1 %t737, label %L179, label %L180
L179:
  %t738 = load ptr, ptr %t306
  %t739 = getelementptr i8, ptr %t738, i64 0
  %t740 = load i64, ptr %t739
  %t742 = sext i32 16 to i64
  %t741 = icmp ne i64 %t740, %t742
  %t743 = zext i1 %t741 to i64
  %t744 = icmp ne i64 %t743, 0
  %t745 = zext i1 %t744 to i64
  br label %L181
L180:
  br label %L181
L181:
  %t746 = phi i64 [ %t745, %L179 ], [ 0, %L180 ]
  %t747 = icmp ne i64 %t746, 0
  br i1 %t747, label %L182, label %L183
L182:
  %t748 = load ptr, ptr %t306
  %t749 = getelementptr i8, ptr %t748, i64 0
  %t750 = load i64, ptr %t749
  %t752 = sext i32 0 to i64
  %t751 = icmp ne i64 %t750, %t752
  %t753 = zext i1 %t751 to i64
  %t754 = icmp ne i64 %t753, 0
  %t755 = zext i1 %t754 to i64
  br label %L184
L183:
  br label %L184
L184:
  %t756 = phi i64 [ %t755, %L182 ], [ 0, %L183 ]
  %t757 = icmp ne i64 %t756, 0
  br i1 %t757, label %L185, label %L187
L185:
  %t758 = alloca i64
  %t759 = load ptr, ptr %t306
  %t760 = call i32 @type_size(ptr %t759)
  %t761 = sext i32 %t760 to i64
  store i64 %t761, ptr %t758
  %t762 = load i64, ptr %t758
  %t764 = sext i32 %t762 to i64
  %t765 = sext i32 0 to i64
  %t763 = icmp sgt i64 %t764, %t765
  %t766 = zext i1 %t763 to i64
  %t767 = icmp ne i64 %t766, 0
  br i1 %t767, label %L188, label %L189
L188:
  %t768 = load i64, ptr %t758
  %t770 = sext i32 %t768 to i64
  %t771 = sext i32 8 to i64
  %t769 = icmp slt i64 %t770, %t771
  %t772 = zext i1 %t769 to i64
  %t773 = icmp ne i64 %t772, 0
  %t774 = zext i1 %t773 to i64
  br label %L190
L189:
  br label %L190
L190:
  %t775 = phi i64 [ %t774, %L188 ], [ 0, %L189 ]
  %t776 = icmp ne i64 %t775, 0
  br i1 %t776, label %L191, label %L192
L191:
  %t777 = load ptr, ptr %t539
  %t778 = getelementptr [4 x i8], ptr @.str77, i64 0, i64 0
  %t779 = call i32 @strcmp(ptr %t777, ptr %t778)
  %t780 = sext i32 %t779 to i64
  %t782 = sext i32 0 to i64
  %t781 = icmp ne i64 %t780, %t782
  %t783 = zext i1 %t781 to i64
  %t784 = icmp ne i64 %t783, 0
  %t785 = zext i1 %t784 to i64
  br label %L193
L192:
  br label %L193
L193:
  %t786 = phi i64 [ %t785, %L191 ], [ 0, %L192 ]
  %t787 = icmp ne i64 %t786, 0
  br i1 %t787, label %L194, label %L196
L194:
  %t788 = alloca i64
  %t789 = call i32 @new_reg(ptr %t0)
  %t790 = sext i32 %t789 to i64
  store i64 %t790, ptr %t788
  %t791 = getelementptr i8, ptr %t0, i64 0
  %t792 = load i64, ptr %t791
  %t793 = getelementptr [32 x i8], ptr @.str78, i64 0, i64 0
  %t794 = load i64, ptr %t788
  %t795 = load ptr, ptr %t539
  %t796 = load i64, ptr %t536
  call void (ptr, ...) @__c0c_emit(i64 %t792, ptr %t793, i64 %t794, ptr %t795, i64 %t796)
  %t798 = load ptr, ptr %t714
  %t799 = getelementptr [6 x i8], ptr @.str79, i64 0, i64 0
  %t800 = load i64, ptr %t788
  %t801 = call i32 (ptr, ...) @snprintf(ptr %t798, i64 8, ptr %t799, i64 %t800)
  %t802 = sext i32 %t801 to i64
  br label %L196
L196:
  %t803 = call ptr @default_i64_type()
  store ptr %t803, ptr %t720
  br label %L187
L187:
  %t804 = load ptr, ptr %t714
  %t805 = load ptr, ptr %t720
  %t806 = call i64 @make_val(ptr %t804, ptr %t805)
  ret i64 %t806
L197:
  br label %L11
L11:
  %t807 = getelementptr i8, ptr %t1, i64 0
  %t808 = load i64, ptr %t807
  %t810 = sext i32 52 to i64
  %t809 = icmp eq i64 %t808, %t810
  %t811 = zext i1 %t809 to i64
  %t812 = icmp ne i64 %t811, 0
  br i1 %t812, label %L198, label %L200
L198:
  %t813 = alloca i64
  %t814 = getelementptr i8, ptr %t1, i64 0
  %t815 = load i64, ptr %t814
  %t816 = inttoptr i64 %t815 to ptr
  %t817 = sext i32 0 to i64
  %t818 = getelementptr ptr, ptr %t816, i64 %t817
  %t819 = load ptr, ptr %t818
  %t820 = call i64 @emit_expr(ptr %t0, ptr %t819)
  store i64 %t820, ptr %t813
  %t821 = alloca i64
  %t822 = call i32 @new_label(ptr %t0)
  %t823 = sext i32 %t822 to i64
  store i64 %t823, ptr %t821
  %t824 = alloca i64
  %t825 = call i32 @new_label(ptr %t0)
  %t826 = sext i32 %t825 to i64
  store i64 %t826, ptr %t824
  %t827 = alloca i64
  %t828 = call i32 @new_label(ptr %t0)
  %t829 = sext i32 %t828 to i64
  store i64 %t829, ptr %t827
  %t830 = alloca ptr
  %t831 = load i64, ptr %t813
  %t832 = load ptr, ptr %t830
  %t833 = call i32 @promote_to_i64(ptr %t0, i64 %t831, ptr %t832, i64 64)
  %t834 = sext i32 %t833 to i64
  %t835 = alloca i64
  %t836 = call i32 @new_reg(ptr %t0)
  %t837 = sext i32 %t836 to i64
  store i64 %t837, ptr %t835
  %t838 = getelementptr i8, ptr %t0, i64 0
  %t839 = load i64, ptr %t838
  %t840 = getelementptr [29 x i8], ptr @.str80, i64 0, i64 0
  %t841 = load i64, ptr %t835
  %t842 = load ptr, ptr %t830
  call void (ptr, ...) @__c0c_emit(i64 %t839, ptr %t840, i64 %t841, ptr %t842)
  %t844 = getelementptr i8, ptr %t0, i64 0
  %t845 = load i64, ptr %t844
  %t846 = getelementptr [41 x i8], ptr @.str81, i64 0, i64 0
  %t847 = load i64, ptr %t835
  %t848 = load i64, ptr %t821
  %t849 = load i64, ptr %t824
  call void (ptr, ...) @__c0c_emit(i64 %t845, ptr %t846, i64 %t847, i64 %t848, i64 %t849)
  %t851 = getelementptr i8, ptr %t0, i64 0
  %t852 = load i64, ptr %t851
  %t853 = getelementptr [6 x i8], ptr @.str82, i64 0, i64 0
  %t854 = load i64, ptr %t821
  call void (ptr, ...) @__c0c_emit(i64 %t852, ptr %t853, i64 %t854)
  %t856 = alloca i64
  %t857 = getelementptr i8, ptr %t1, i64 0
  %t858 = load i64, ptr %t857
  %t859 = inttoptr i64 %t858 to ptr
  %t860 = sext i32 1 to i64
  %t861 = getelementptr ptr, ptr %t859, i64 %t860
  %t862 = load ptr, ptr %t861
  %t863 = call i64 @emit_expr(ptr %t0, ptr %t862)
  store i64 %t863, ptr %t856
  %t864 = alloca ptr
  %t865 = load i64, ptr %t856
  %t866 = load ptr, ptr %t864
  %t867 = call i32 @promote_to_i64(ptr %t0, i64 %t865, ptr %t866, i64 64)
  %t868 = sext i32 %t867 to i64
  %t869 = alloca i64
  %t870 = call i32 @new_reg(ptr %t0)
  %t871 = sext i32 %t870 to i64
  store i64 %t871, ptr %t869
  %t872 = alloca i64
  %t873 = call i32 @new_reg(ptr %t0)
  %t874 = sext i32 %t873 to i64
  store i64 %t874, ptr %t872
  %t875 = getelementptr i8, ptr %t0, i64 0
  %t876 = load i64, ptr %t875
  %t877 = getelementptr [29 x i8], ptr @.str83, i64 0, i64 0
  %t878 = load i64, ptr %t869
  %t879 = load ptr, ptr %t864
  call void (ptr, ...) @__c0c_emit(i64 %t876, ptr %t877, i64 %t878, ptr %t879)
  %t881 = getelementptr i8, ptr %t0, i64 0
  %t882 = load i64, ptr %t881
  %t883 = getelementptr [32 x i8], ptr @.str84, i64 0, i64 0
  %t884 = load i64, ptr %t872
  %t885 = load i64, ptr %t869
  call void (ptr, ...) @__c0c_emit(i64 %t882, ptr %t883, i64 %t884, i64 %t885)
  %t887 = getelementptr i8, ptr %t0, i64 0
  %t888 = load i64, ptr %t887
  %t889 = getelementptr [18 x i8], ptr @.str85, i64 0, i64 0
  %t890 = load i64, ptr %t827
  call void (ptr, ...) @__c0c_emit(i64 %t888, ptr %t889, i64 %t890)
  %t892 = getelementptr i8, ptr %t0, i64 0
  %t893 = load i64, ptr %t892
  %t894 = getelementptr [6 x i8], ptr @.str86, i64 0, i64 0
  %t895 = load i64, ptr %t824
  call void (ptr, ...) @__c0c_emit(i64 %t893, ptr %t894, i64 %t895)
  %t897 = getelementptr i8, ptr %t0, i64 0
  %t898 = load i64, ptr %t897
  %t899 = getelementptr [18 x i8], ptr @.str87, i64 0, i64 0
  %t900 = load i64, ptr %t827
  call void (ptr, ...) @__c0c_emit(i64 %t898, ptr %t899, i64 %t900)
  %t902 = getelementptr i8, ptr %t0, i64 0
  %t903 = load i64, ptr %t902
  %t904 = getelementptr [6 x i8], ptr @.str88, i64 0, i64 0
  %t905 = load i64, ptr %t827
  call void (ptr, ...) @__c0c_emit(i64 %t903, ptr %t904, i64 %t905)
  %t907 = alloca i64
  %t908 = call i32 @new_reg(ptr %t0)
  %t909 = sext i32 %t908 to i64
  store i64 %t909, ptr %t907
  %t910 = getelementptr i8, ptr %t0, i64 0
  %t911 = load i64, ptr %t910
  %t912 = getelementptr [50 x i8], ptr @.str89, i64 0, i64 0
  %t913 = load i64, ptr %t907
  %t914 = load i64, ptr %t872
  %t915 = load i64, ptr %t821
  %t916 = load i64, ptr %t824
  call void (ptr, ...) @__c0c_emit(i64 %t911, ptr %t912, i64 %t913, i64 %t914, i64 %t915, i64 %t916)
  %t918 = alloca ptr
  %t919 = load ptr, ptr %t918
  %t920 = getelementptr [6 x i8], ptr @.str90, i64 0, i64 0
  %t921 = load i64, ptr %t907
  %t922 = call i32 (ptr, ...) @snprintf(ptr %t919, i64 8, ptr %t920, i64 %t921)
  %t923 = sext i32 %t922 to i64
  %t924 = load ptr, ptr %t918
  %t925 = call ptr @default_i64_type()
  %t926 = call i64 @make_val(ptr %t924, ptr %t925)
  ret i64 %t926
L201:
  br label %L200
L200:
  %t927 = getelementptr i8, ptr %t1, i64 0
  %t928 = load i64, ptr %t927
  %t930 = sext i32 53 to i64
  %t929 = icmp eq i64 %t928, %t930
  %t931 = zext i1 %t929 to i64
  %t932 = icmp ne i64 %t931, 0
  br i1 %t932, label %L202, label %L204
L202:
  %t933 = alloca i64
  %t934 = getelementptr i8, ptr %t1, i64 0
  %t935 = load i64, ptr %t934
  %t936 = inttoptr i64 %t935 to ptr
  %t937 = sext i32 0 to i64
  %t938 = getelementptr ptr, ptr %t936, i64 %t937
  %t939 = load ptr, ptr %t938
  %t940 = call i64 @emit_expr(ptr %t0, ptr %t939)
  store i64 %t940, ptr %t933
  %t941 = alloca i64
  %t942 = call i32 @new_label(ptr %t0)
  %t943 = sext i32 %t942 to i64
  store i64 %t943, ptr %t941
  %t944 = alloca i64
  %t945 = call i32 @new_label(ptr %t0)
  %t946 = sext i32 %t945 to i64
  store i64 %t946, ptr %t944
  %t947 = alloca i64
  %t948 = call i32 @new_label(ptr %t0)
  %t949 = sext i32 %t948 to i64
  store i64 %t949, ptr %t947
  %t950 = alloca ptr
  %t951 = load i64, ptr %t933
  %t952 = load ptr, ptr %t950
  %t953 = call i32 @promote_to_i64(ptr %t0, i64 %t951, ptr %t952, i64 64)
  %t954 = sext i32 %t953 to i64
  %t955 = alloca i64
  %t956 = call i32 @new_reg(ptr %t0)
  %t957 = sext i32 %t956 to i64
  store i64 %t957, ptr %t955
  %t958 = getelementptr i8, ptr %t0, i64 0
  %t959 = load i64, ptr %t958
  %t960 = getelementptr [29 x i8], ptr @.str91, i64 0, i64 0
  %t961 = load i64, ptr %t955
  %t962 = load ptr, ptr %t950
  call void (ptr, ...) @__c0c_emit(i64 %t959, ptr %t960, i64 %t961, ptr %t962)
  %t964 = getelementptr i8, ptr %t0, i64 0
  %t965 = load i64, ptr %t964
  %t966 = getelementptr [41 x i8], ptr @.str92, i64 0, i64 0
  %t967 = load i64, ptr %t955
  %t968 = load i64, ptr %t941
  %t969 = load i64, ptr %t944
  call void (ptr, ...) @__c0c_emit(i64 %t965, ptr %t966, i64 %t967, i64 %t968, i64 %t969)
  %t971 = getelementptr i8, ptr %t0, i64 0
  %t972 = load i64, ptr %t971
  %t973 = getelementptr [6 x i8], ptr @.str93, i64 0, i64 0
  %t974 = load i64, ptr %t941
  call void (ptr, ...) @__c0c_emit(i64 %t972, ptr %t973, i64 %t974)
  %t976 = getelementptr i8, ptr %t0, i64 0
  %t977 = load i64, ptr %t976
  %t978 = getelementptr [18 x i8], ptr @.str94, i64 0, i64 0
  %t979 = load i64, ptr %t947
  call void (ptr, ...) @__c0c_emit(i64 %t977, ptr %t978, i64 %t979)
  %t981 = getelementptr i8, ptr %t0, i64 0
  %t982 = load i64, ptr %t981
  %t983 = getelementptr [6 x i8], ptr @.str95, i64 0, i64 0
  %t984 = load i64, ptr %t944
  call void (ptr, ...) @__c0c_emit(i64 %t982, ptr %t983, i64 %t984)
  %t986 = alloca i64
  %t987 = getelementptr i8, ptr %t1, i64 0
  %t988 = load i64, ptr %t987
  %t989 = inttoptr i64 %t988 to ptr
  %t990 = sext i32 1 to i64
  %t991 = getelementptr ptr, ptr %t989, i64 %t990
  %t992 = load ptr, ptr %t991
  %t993 = call i64 @emit_expr(ptr %t0, ptr %t992)
  store i64 %t993, ptr %t986
  %t994 = alloca ptr
  %t995 = load i64, ptr %t986
  %t996 = load ptr, ptr %t994
  %t997 = call i32 @promote_to_i64(ptr %t0, i64 %t995, ptr %t996, i64 64)
  %t998 = sext i32 %t997 to i64
  %t999 = alloca i64
  %t1000 = call i32 @new_reg(ptr %t0)
  %t1001 = sext i32 %t1000 to i64
  store i64 %t1001, ptr %t999
  %t1002 = alloca i64
  %t1003 = call i32 @new_reg(ptr %t0)
  %t1004 = sext i32 %t1003 to i64
  store i64 %t1004, ptr %t1002
  %t1005 = getelementptr i8, ptr %t0, i64 0
  %t1006 = load i64, ptr %t1005
  %t1007 = getelementptr [29 x i8], ptr @.str96, i64 0, i64 0
  %t1008 = load i64, ptr %t999
  %t1009 = load ptr, ptr %t994
  call void (ptr, ...) @__c0c_emit(i64 %t1006, ptr %t1007, i64 %t1008, ptr %t1009)
  %t1011 = getelementptr i8, ptr %t0, i64 0
  %t1012 = load i64, ptr %t1011
  %t1013 = getelementptr [32 x i8], ptr @.str97, i64 0, i64 0
  %t1014 = load i64, ptr %t1002
  %t1015 = load i64, ptr %t999
  call void (ptr, ...) @__c0c_emit(i64 %t1012, ptr %t1013, i64 %t1014, i64 %t1015)
  %t1017 = getelementptr i8, ptr %t0, i64 0
  %t1018 = load i64, ptr %t1017
  %t1019 = getelementptr [18 x i8], ptr @.str98, i64 0, i64 0
  %t1020 = load i64, ptr %t947
  call void (ptr, ...) @__c0c_emit(i64 %t1018, ptr %t1019, i64 %t1020)
  %t1022 = getelementptr i8, ptr %t0, i64 0
  %t1023 = load i64, ptr %t1022
  %t1024 = getelementptr [6 x i8], ptr @.str99, i64 0, i64 0
  %t1025 = load i64, ptr %t947
  call void (ptr, ...) @__c0c_emit(i64 %t1023, ptr %t1024, i64 %t1025)
  %t1027 = alloca i64
  %t1028 = call i32 @new_reg(ptr %t0)
  %t1029 = sext i32 %t1028 to i64
  store i64 %t1029, ptr %t1027
  %t1030 = getelementptr i8, ptr %t0, i64 0
  %t1031 = load i64, ptr %t1030
  %t1032 = getelementptr [50 x i8], ptr @.str100, i64 0, i64 0
  %t1033 = load i64, ptr %t1027
  %t1034 = load i64, ptr %t941
  %t1035 = load i64, ptr %t1002
  %t1036 = load i64, ptr %t944
  call void (ptr, ...) @__c0c_emit(i64 %t1031, ptr %t1032, i64 %t1033, i64 %t1034, i64 %t1035, i64 %t1036)
  %t1038 = alloca ptr
  %t1039 = load ptr, ptr %t1038
  %t1040 = getelementptr [6 x i8], ptr @.str101, i64 0, i64 0
  %t1041 = load i64, ptr %t1027
  %t1042 = call i32 (ptr, ...) @snprintf(ptr %t1039, i64 8, ptr %t1040, i64 %t1041)
  %t1043 = sext i32 %t1042 to i64
  %t1044 = load ptr, ptr %t1038
  %t1045 = call ptr @default_i64_type()
  %t1046 = call i64 @make_val(ptr %t1044, ptr %t1045)
  ret i64 %t1046
L205:
  br label %L204
L204:
  %t1047 = alloca i64
  %t1048 = getelementptr i8, ptr %t1, i64 0
  %t1049 = load i64, ptr %t1048
  %t1050 = inttoptr i64 %t1049 to ptr
  %t1051 = sext i32 0 to i64
  %t1052 = getelementptr ptr, ptr %t1050, i64 %t1051
  %t1053 = load ptr, ptr %t1052
  %t1054 = call i64 @emit_expr(ptr %t0, ptr %t1053)
  store i64 %t1054, ptr %t1047
  %t1055 = alloca i64
  %t1056 = getelementptr i8, ptr %t1, i64 0
  %t1057 = load i64, ptr %t1056
  %t1058 = inttoptr i64 %t1057 to ptr
  %t1059 = sext i32 1 to i64
  %t1060 = getelementptr ptr, ptr %t1058, i64 %t1059
  %t1061 = load ptr, ptr %t1060
  %t1062 = call i64 @emit_expr(ptr %t0, ptr %t1061)
  store i64 %t1062, ptr %t1055
  %t1063 = alloca i64
  %t1064 = call i32 @new_reg(ptr %t0)
  %t1065 = sext i32 %t1064 to i64
  store i64 %t1065, ptr %t1063
  %t1066 = alloca i64
  %t1067 = getelementptr i8, ptr %t1047, i64 0
  %t1068 = load i64, ptr %t1067
  %t1069 = call i32 @type_is_fp(i64 %t1068)
  %t1070 = sext i32 %t1069 to i64
  %t1071 = icmp ne i64 %t1070, 0
  br i1 %t1071, label %L206, label %L207
L206:
  br label %L208
L207:
  %t1072 = getelementptr i8, ptr %t1055, i64 0
  %t1073 = load i64, ptr %t1072
  %t1074 = call i32 @type_is_fp(i64 %t1073)
  %t1075 = sext i32 %t1074 to i64
  %t1076 = icmp ne i64 %t1075, 0
  %t1077 = zext i1 %t1076 to i64
  br label %L208
L208:
  %t1078 = phi i64 [ 1, %L206 ], [ %t1077, %L207 ]
  store i64 %t1078, ptr %t1066
  %t1079 = alloca i64
  %t1080 = load i64, ptr %t1047
  %t1081 = call i32 @val_is_ptr(i64 %t1080)
  %t1082 = sext i32 %t1081 to i64
  store i64 %t1082, ptr %t1079
  %t1083 = alloca ptr
  %t1084 = load i64, ptr %t1066
  %t1086 = sext i32 %t1084 to i64
  %t1085 = icmp ne i64 %t1086, 0
  br i1 %t1085, label %L209, label %L210
L209:
  %t1087 = getelementptr i8, ptr %t1047, i64 0
  %t1088 = load i64, ptr %t1087
  %t1089 = call ptr @llvm_type(i64 %t1088)
  %t1090 = ptrtoint ptr %t1089 to i64
  br label %L211
L210:
  %t1091 = getelementptr [4 x i8], ptr @.str102, i64 0, i64 0
  %t1092 = ptrtoint ptr %t1091 to i64
  br label %L211
L211:
  %t1093 = phi i64 [ %t1090, %L209 ], [ %t1092, %L210 ]
  store i64 %t1093, ptr %t1083
  %t1094 = alloca ptr
  %t1095 = alloca ptr
  %t1096 = load ptr, ptr %t1094
  %t1098 = sext i32 0 to i64
  %t1097 = getelementptr ptr, ptr %t1096, i64 %t1098
  %t1099 = sext i32 0 to i64
  store i64 %t1099, ptr %t1097
  %t1100 = load ptr, ptr %t1095
  %t1102 = sext i32 0 to i64
  %t1101 = getelementptr ptr, ptr %t1100, i64 %t1102
  %t1103 = sext i32 0 to i64
  store i64 %t1103, ptr %t1101
  %t1104 = alloca i64
  %t1105 = sext i32 0 to i64
  store i64 %t1105, ptr %t1104
  %t1106 = getelementptr i8, ptr %t1, i64 0
  %t1107 = load i64, ptr %t1106
  %t1108 = add i64 %t1107, 0
  switch i64 %t1108, label %L219 [
    i64 46, label %L213
    i64 47, label %L214
    i64 48, label %L215
    i64 49, label %L216
    i64 50, label %L217
    i64 51, label %L218
  ]
L213:
  br label %L214
L214:
  br label %L215
L215:
  br label %L216
L216:
  br label %L217
L217:
  br label %L218
L218:
  %t1109 = sext i32 1 to i64
  store i64 %t1109, ptr %t1104
  br label %L212
L220:
  br label %L212
L219:
  br label %L212
L221:
  br label %L212
L212:
  %t1110 = load i64, ptr %t1066
  %t1112 = sext i32 %t1110 to i64
  %t1113 = icmp eq i64 %t1112, 0
  %t1111 = zext i1 %t1113 to i64
  %t1114 = icmp ne i64 %t1111, 0
  br i1 %t1114, label %L222, label %L223
L222:
  %t1115 = load i64, ptr %t1047
  %t1116 = load ptr, ptr %t1094
  %t1117 = call i32 @promote_to_i64(ptr %t0, i64 %t1115, ptr %t1116, i64 64)
  %t1118 = sext i32 %t1117 to i64
  %t1119 = load i64, ptr %t1055
  %t1120 = load ptr, ptr %t1095
  %t1121 = call i32 @promote_to_i64(ptr %t0, i64 %t1119, ptr %t1120, i64 64)
  %t1122 = sext i32 %t1121 to i64
  %t1123 = getelementptr [4 x i8], ptr @.str103, i64 0, i64 0
  store ptr %t1123, ptr %t1083
  br label %L224
L223:
  %t1124 = load ptr, ptr %t1094
  %t1125 = getelementptr i8, ptr %t1047, i64 0
  %t1126 = load i64, ptr %t1125
  %t1127 = call ptr @strncpy(ptr %t1124, i64 %t1126, i64 63)
  %t1128 = load ptr, ptr %t1094
  %t1130 = sext i32 63 to i64
  %t1129 = getelementptr ptr, ptr %t1128, i64 %t1130
  %t1131 = sext i32 0 to i64
  store i64 %t1131, ptr %t1129
  %t1132 = load i64, ptr %t1104
  %t1133 = sext i32 %t1132 to i64
  %t1134 = icmp ne i64 %t1133, 0
  br i1 %t1134, label %L225, label %L226
L225:
  %t1135 = getelementptr i8, ptr %t1055, i64 0
  %t1136 = load i64, ptr %t1135
  %t1137 = icmp ne i64 %t1136, 0
  %t1138 = zext i1 %t1137 to i64
  br label %L227
L226:
  br label %L227
L227:
  %t1139 = phi i64 [ %t1138, %L225 ], [ 0, %L226 ]
  %t1140 = icmp ne i64 %t1139, 0
  br i1 %t1140, label %L228, label %L229
L228:
  %t1141 = getelementptr i8, ptr %t1055, i64 0
  %t1142 = load i64, ptr %t1141
  %t1143 = call i32 @type_is_fp(i64 %t1142)
  %t1144 = sext i32 %t1143 to i64
  %t1146 = icmp eq i64 %t1144, 0
  %t1145 = zext i1 %t1146 to i64
  %t1147 = icmp ne i64 %t1145, 0
  %t1148 = zext i1 %t1147 to i64
  br label %L230
L229:
  br label %L230
L230:
  %t1149 = phi i64 [ %t1148, %L228 ], [ 0, %L229 ]
  %t1150 = icmp ne i64 %t1149, 0
  br i1 %t1150, label %L231, label %L232
L231:
  %t1151 = alloca i64
  %t1152 = call i32 @new_reg(ptr %t0)
  %t1153 = sext i32 %t1152 to i64
  store i64 %t1153, ptr %t1151
  %t1154 = getelementptr i8, ptr %t0, i64 0
  %t1155 = load i64, ptr %t1154
  %t1156 = getelementptr [35 x i8], ptr @.str104, i64 0, i64 0
  %t1157 = load i64, ptr %t1151
  %t1158 = getelementptr i8, ptr %t1055, i64 0
  %t1159 = load i64, ptr %t1158
  call void (ptr, ...) @__c0c_emit(i64 %t1155, ptr %t1156, i64 %t1157, i64 %t1159)
  %t1161 = load ptr, ptr %t1095
  %t1162 = getelementptr [6 x i8], ptr @.str105, i64 0, i64 0
  %t1163 = load i64, ptr %t1151
  %t1164 = call i32 (ptr, ...) @snprintf(ptr %t1161, i64 64, ptr %t1162, i64 %t1163)
  %t1165 = sext i32 %t1164 to i64
  br label %L233
L232:
  %t1166 = load ptr, ptr %t1095
  %t1167 = getelementptr i8, ptr %t1055, i64 0
  %t1168 = load i64, ptr %t1167
  %t1169 = call ptr @strncpy(ptr %t1166, i64 %t1168, i64 63)
  %t1170 = load ptr, ptr %t1095
  %t1172 = sext i32 63 to i64
  %t1171 = getelementptr ptr, ptr %t1170, i64 %t1172
  %t1173 = sext i32 0 to i64
  store i64 %t1173, ptr %t1171
  br label %L233
L233:
  br label %L224
L224:
  %t1174 = alloca ptr
  %t1176 = sext i32 0 to i64
  %t1175 = inttoptr i64 %t1176 to ptr
  store ptr %t1175, ptr %t1174
  %t1177 = getelementptr i8, ptr %t1, i64 0
  %t1178 = load i64, ptr %t1177
  %t1179 = add i64 %t1178, 0
  switch i64 %t1179, label %L253 [
    i64 35, label %L235
    i64 36, label %L236
    i64 37, label %L237
    i64 38, label %L238
    i64 39, label %L239
    i64 40, label %L240
    i64 41, label %L241
    i64 42, label %L242
    i64 44, label %L243
    i64 45, label %L244
    i64 46, label %L245
    i64 47, label %L246
    i64 48, label %L247
    i64 49, label %L248
    i64 50, label %L249
    i64 51, label %L250
    i64 52, label %L251
    i64 53, label %L252
  ]
L235:
  %t1180 = load i64, ptr %t1066
  %t1182 = sext i32 %t1180 to i64
  %t1181 = icmp ne i64 %t1182, 0
  br i1 %t1181, label %L254, label %L255
L254:
  %t1183 = getelementptr [5 x i8], ptr @.str106, i64 0, i64 0
  %t1184 = ptrtoint ptr %t1183 to i64
  br label %L256
L255:
  %t1185 = load i64, ptr %t1079
  %t1187 = sext i32 %t1185 to i64
  %t1186 = icmp ne i64 %t1187, 0
  br i1 %t1186, label %L257, label %L258
L257:
  %t1188 = getelementptr [14 x i8], ptr @.str107, i64 0, i64 0
  %t1189 = ptrtoint ptr %t1188 to i64
  br label %L259
L258:
  %t1190 = getelementptr [4 x i8], ptr @.str108, i64 0, i64 0
  %t1191 = ptrtoint ptr %t1190 to i64
  br label %L259
L259:
  %t1192 = phi i64 [ %t1189, %L257 ], [ %t1191, %L258 ]
  br label %L256
L256:
  %t1193 = phi i64 [ %t1184, %L254 ], [ %t1192, %L255 ]
  store i64 %t1193, ptr %t1174
  br label %L234
L260:
  br label %L236
L236:
  %t1194 = load i64, ptr %t1066
  %t1196 = sext i32 %t1194 to i64
  %t1195 = icmp ne i64 %t1196, 0
  br i1 %t1195, label %L261, label %L262
L261:
  %t1197 = getelementptr [5 x i8], ptr @.str109, i64 0, i64 0
  %t1198 = ptrtoint ptr %t1197 to i64
  br label %L263
L262:
  %t1199 = getelementptr [4 x i8], ptr @.str110, i64 0, i64 0
  %t1200 = ptrtoint ptr %t1199 to i64
  br label %L263
L263:
  %t1201 = phi i64 [ %t1198, %L261 ], [ %t1200, %L262 ]
  store i64 %t1201, ptr %t1174
  br label %L234
L264:
  br label %L237
L237:
  %t1202 = load i64, ptr %t1066
  %t1204 = sext i32 %t1202 to i64
  %t1203 = icmp ne i64 %t1204, 0
  br i1 %t1203, label %L265, label %L266
L265:
  %t1205 = getelementptr [5 x i8], ptr @.str111, i64 0, i64 0
  %t1206 = ptrtoint ptr %t1205 to i64
  br label %L267
L266:
  %t1207 = getelementptr [4 x i8], ptr @.str112, i64 0, i64 0
  %t1208 = ptrtoint ptr %t1207 to i64
  br label %L267
L267:
  %t1209 = phi i64 [ %t1206, %L265 ], [ %t1208, %L266 ]
  store i64 %t1209, ptr %t1174
  br label %L234
L268:
  br label %L238
L238:
  %t1210 = load i64, ptr %t1066
  %t1212 = sext i32 %t1210 to i64
  %t1211 = icmp ne i64 %t1212, 0
  br i1 %t1211, label %L269, label %L270
L269:
  %t1213 = getelementptr [5 x i8], ptr @.str113, i64 0, i64 0
  %t1214 = ptrtoint ptr %t1213 to i64
  br label %L271
L270:
  %t1215 = getelementptr [5 x i8], ptr @.str114, i64 0, i64 0
  %t1216 = ptrtoint ptr %t1215 to i64
  br label %L271
L271:
  %t1217 = phi i64 [ %t1214, %L269 ], [ %t1216, %L270 ]
  store i64 %t1217, ptr %t1174
  br label %L234
L272:
  br label %L239
L239:
  %t1218 = load i64, ptr %t1066
  %t1220 = sext i32 %t1218 to i64
  %t1219 = icmp ne i64 %t1220, 0
  br i1 %t1219, label %L273, label %L274
L273:
  %t1221 = getelementptr [5 x i8], ptr @.str115, i64 0, i64 0
  %t1222 = ptrtoint ptr %t1221 to i64
  br label %L275
L274:
  %t1223 = getelementptr [5 x i8], ptr @.str116, i64 0, i64 0
  %t1224 = ptrtoint ptr %t1223 to i64
  br label %L275
L275:
  %t1225 = phi i64 [ %t1222, %L273 ], [ %t1224, %L274 ]
  store i64 %t1225, ptr %t1174
  br label %L234
L276:
  br label %L240
L240:
  %t1226 = getelementptr [4 x i8], ptr @.str117, i64 0, i64 0
  store ptr %t1226, ptr %t1174
  br label %L234
L277:
  br label %L241
L241:
  %t1227 = getelementptr [3 x i8], ptr @.str118, i64 0, i64 0
  store ptr %t1227, ptr %t1174
  br label %L234
L278:
  br label %L242
L242:
  %t1228 = getelementptr [4 x i8], ptr @.str119, i64 0, i64 0
  store ptr %t1228, ptr %t1174
  br label %L234
L279:
  br label %L243
L243:
  %t1229 = getelementptr [4 x i8], ptr @.str120, i64 0, i64 0
  store ptr %t1229, ptr %t1174
  br label %L234
L280:
  br label %L244
L244:
  %t1230 = getelementptr [5 x i8], ptr @.str121, i64 0, i64 0
  store ptr %t1230, ptr %t1174
  br label %L234
L281:
  br label %L245
L245:
  %t1231 = load i64, ptr %t1066
  %t1233 = sext i32 %t1231 to i64
  %t1232 = icmp ne i64 %t1233, 0
  br i1 %t1232, label %L282, label %L283
L282:
  %t1234 = getelementptr [9 x i8], ptr @.str122, i64 0, i64 0
  %t1235 = ptrtoint ptr %t1234 to i64
  br label %L284
L283:
  %t1236 = getelementptr [8 x i8], ptr @.str123, i64 0, i64 0
  %t1237 = ptrtoint ptr %t1236 to i64
  br label %L284
L284:
  %t1238 = phi i64 [ %t1235, %L282 ], [ %t1237, %L283 ]
  store i64 %t1238, ptr %t1174
  %t1239 = sext i32 1 to i64
  store i64 %t1239, ptr %t1104
  br label %L234
L285:
  br label %L246
L246:
  %t1240 = load i64, ptr %t1066
  %t1242 = sext i32 %t1240 to i64
  %t1241 = icmp ne i64 %t1242, 0
  br i1 %t1241, label %L286, label %L287
L286:
  %t1243 = getelementptr [9 x i8], ptr @.str124, i64 0, i64 0
  %t1244 = ptrtoint ptr %t1243 to i64
  br label %L288
L287:
  %t1245 = getelementptr [8 x i8], ptr @.str125, i64 0, i64 0
  %t1246 = ptrtoint ptr %t1245 to i64
  br label %L288
L288:
  %t1247 = phi i64 [ %t1244, %L286 ], [ %t1246, %L287 ]
  store i64 %t1247, ptr %t1174
  %t1248 = sext i32 1 to i64
  store i64 %t1248, ptr %t1104
  br label %L234
L289:
  br label %L247
L247:
  %t1249 = load i64, ptr %t1066
  %t1251 = sext i32 %t1249 to i64
  %t1250 = icmp ne i64 %t1251, 0
  br i1 %t1250, label %L290, label %L291
L290:
  %t1252 = getelementptr [9 x i8], ptr @.str126, i64 0, i64 0
  %t1253 = ptrtoint ptr %t1252 to i64
  br label %L292
L291:
  %t1254 = getelementptr [9 x i8], ptr @.str127, i64 0, i64 0
  %t1255 = ptrtoint ptr %t1254 to i64
  br label %L292
L292:
  %t1256 = phi i64 [ %t1253, %L290 ], [ %t1255, %L291 ]
  store i64 %t1256, ptr %t1174
  %t1257 = sext i32 1 to i64
  store i64 %t1257, ptr %t1104
  br label %L234
L293:
  br label %L248
L248:
  %t1258 = load i64, ptr %t1066
  %t1260 = sext i32 %t1258 to i64
  %t1259 = icmp ne i64 %t1260, 0
  br i1 %t1259, label %L294, label %L295
L294:
  %t1261 = getelementptr [9 x i8], ptr @.str128, i64 0, i64 0
  %t1262 = ptrtoint ptr %t1261 to i64
  br label %L296
L295:
  %t1263 = getelementptr [9 x i8], ptr @.str129, i64 0, i64 0
  %t1264 = ptrtoint ptr %t1263 to i64
  br label %L296
L296:
  %t1265 = phi i64 [ %t1262, %L294 ], [ %t1264, %L295 ]
  store i64 %t1265, ptr %t1174
  %t1266 = sext i32 1 to i64
  store i64 %t1266, ptr %t1104
  br label %L234
L297:
  br label %L249
L249:
  %t1267 = load i64, ptr %t1066
  %t1269 = sext i32 %t1267 to i64
  %t1268 = icmp ne i64 %t1269, 0
  br i1 %t1268, label %L298, label %L299
L298:
  %t1270 = getelementptr [9 x i8], ptr @.str130, i64 0, i64 0
  %t1271 = ptrtoint ptr %t1270 to i64
  br label %L300
L299:
  %t1272 = getelementptr [9 x i8], ptr @.str131, i64 0, i64 0
  %t1273 = ptrtoint ptr %t1272 to i64
  br label %L300
L300:
  %t1274 = phi i64 [ %t1271, %L298 ], [ %t1273, %L299 ]
  store i64 %t1274, ptr %t1174
  %t1275 = sext i32 1 to i64
  store i64 %t1275, ptr %t1104
  br label %L234
L301:
  br label %L250
L250:
  %t1276 = load i64, ptr %t1066
  %t1278 = sext i32 %t1276 to i64
  %t1277 = icmp ne i64 %t1278, 0
  br i1 %t1277, label %L302, label %L303
L302:
  %t1279 = getelementptr [9 x i8], ptr @.str132, i64 0, i64 0
  %t1280 = ptrtoint ptr %t1279 to i64
  br label %L304
L303:
  %t1281 = getelementptr [9 x i8], ptr @.str133, i64 0, i64 0
  %t1282 = ptrtoint ptr %t1281 to i64
  br label %L304
L304:
  %t1283 = phi i64 [ %t1280, %L302 ], [ %t1282, %L303 ]
  store i64 %t1283, ptr %t1174
  %t1284 = sext i32 1 to i64
  store i64 %t1284, ptr %t1104
  br label %L234
L305:
  br label %L251
L251:
  br label %L252
L252:
  %t1285 = getelementptr [4 x i8], ptr @.str134, i64 0, i64 0
  store ptr %t1285, ptr %t1174
  br label %L234
L306:
  br label %L234
L253:
  %t1286 = getelementptr [4 x i8], ptr @.str135, i64 0, i64 0
  store ptr %t1286, ptr %t1174
  br label %L234
L234:
  %t1287 = getelementptr i8, ptr %t1, i64 0
  %t1288 = load i64, ptr %t1287
  %t1290 = sext i32 35 to i64
  %t1289 = icmp eq i64 %t1288, %t1290
  %t1291 = zext i1 %t1289 to i64
  %t1292 = icmp ne i64 %t1291, 0
  br i1 %t1292, label %L307, label %L308
L307:
  %t1293 = load i64, ptr %t1079
  %t1294 = sext i32 %t1293 to i64
  %t1295 = icmp ne i64 %t1294, 0
  %t1296 = zext i1 %t1295 to i64
  br label %L309
L308:
  br label %L309
L309:
  %t1297 = phi i64 [ %t1296, %L307 ], [ 0, %L308 ]
  %t1298 = icmp ne i64 %t1297, 0
  br i1 %t1298, label %L310, label %L311
L310:
  %t1299 = alloca i64
  %t1300 = call i32 @new_reg(ptr %t0)
  %t1301 = sext i32 %t1300 to i64
  store i64 %t1301, ptr %t1299
  %t1302 = getelementptr i8, ptr %t0, i64 0
  %t1303 = load i64, ptr %t1302
  %t1304 = getelementptr [34 x i8], ptr @.str136, i64 0, i64 0
  %t1305 = load i64, ptr %t1299
  %t1306 = load ptr, ptr %t1094
  call void (ptr, ...) @__c0c_emit(i64 %t1303, ptr %t1304, i64 %t1305, ptr %t1306)
  %t1308 = getelementptr i8, ptr %t0, i64 0
  %t1309 = load i64, ptr %t1308
  %t1310 = getelementptr [47 x i8], ptr @.str137, i64 0, i64 0
  %t1311 = load i64, ptr %t1063
  %t1312 = load i64, ptr %t1299
  %t1313 = load ptr, ptr %t1095
  call void (ptr, ...) @__c0c_emit(i64 %t1309, ptr %t1310, i64 %t1311, i64 %t1312, ptr %t1313)
  br label %L312
L311:
  %t1315 = load i64, ptr %t1104
  %t1317 = sext i32 %t1315 to i64
  %t1316 = icmp ne i64 %t1317, 0
  br i1 %t1316, label %L313, label %L314
L313:
  %t1318 = getelementptr i8, ptr %t0, i64 0
  %t1319 = load i64, ptr %t1318
  %t1320 = getelementptr [24 x i8], ptr @.str138, i64 0, i64 0
  %t1321 = load i64, ptr %t1063
  %t1322 = load ptr, ptr %t1174
  %t1323 = load ptr, ptr %t1083
  %t1324 = load ptr, ptr %t1094
  %t1325 = load ptr, ptr %t1095
  call void (ptr, ...) @__c0c_emit(i64 %t1319, ptr %t1320, i64 %t1321, ptr %t1322, ptr %t1323, ptr %t1324, ptr %t1325)
  %t1327 = alloca i64
  %t1328 = call i32 @new_reg(ptr %t0)
  %t1329 = sext i32 %t1328 to i64
  store i64 %t1329, ptr %t1327
  %t1330 = getelementptr i8, ptr %t0, i64 0
  %t1331 = load i64, ptr %t1330
  %t1332 = getelementptr [32 x i8], ptr @.str139, i64 0, i64 0
  %t1333 = load i64, ptr %t1327
  %t1334 = load i64, ptr %t1063
  call void (ptr, ...) @__c0c_emit(i64 %t1331, ptr %t1332, i64 %t1333, i64 %t1334)
  %t1336 = alloca ptr
  %t1337 = load ptr, ptr %t1336
  %t1338 = getelementptr [6 x i8], ptr @.str140, i64 0, i64 0
  %t1339 = load i64, ptr %t1327
  %t1340 = call i32 (ptr, ...) @snprintf(ptr %t1337, i64 8, ptr %t1338, i64 %t1339)
  %t1341 = sext i32 %t1340 to i64
  %t1342 = load ptr, ptr %t1336
  %t1343 = call ptr @default_i64_type()
  %t1344 = call i64 @make_val(ptr %t1342, ptr %t1343)
  ret i64 %t1344
L316:
  br label %L315
L314:
  %t1345 = getelementptr i8, ptr %t0, i64 0
  %t1346 = load i64, ptr %t1345
  %t1347 = getelementptr [24 x i8], ptr @.str141, i64 0, i64 0
  %t1348 = load i64, ptr %t1063
  %t1349 = load ptr, ptr %t1174
  %t1350 = load ptr, ptr %t1083
  %t1351 = load ptr, ptr %t1094
  %t1352 = load ptr, ptr %t1095
  call void (ptr, ...) @__c0c_emit(i64 %t1346, ptr %t1347, i64 %t1348, ptr %t1349, ptr %t1350, ptr %t1351, ptr %t1352)
  br label %L315
L315:
  br label %L312
L312:
  %t1354 = alloca ptr
  %t1355 = load ptr, ptr %t1354
  %t1356 = getelementptr [6 x i8], ptr @.str142, i64 0, i64 0
  %t1357 = load i64, ptr %t1063
  %t1358 = call i32 (ptr, ...) @snprintf(ptr %t1355, i64 8, ptr %t1356, i64 %t1357)
  %t1359 = sext i32 %t1358 to i64
  %t1360 = getelementptr i8, ptr %t1, i64 0
  %t1361 = load i64, ptr %t1360
  %t1363 = sext i32 35 to i64
  %t1362 = icmp eq i64 %t1361, %t1363
  %t1364 = zext i1 %t1362 to i64
  %t1365 = icmp ne i64 %t1364, 0
  br i1 %t1365, label %L317, label %L318
L317:
  %t1366 = load i64, ptr %t1079
  %t1367 = sext i32 %t1366 to i64
  %t1368 = icmp ne i64 %t1367, 0
  %t1369 = zext i1 %t1368 to i64
  br label %L319
L318:
  br label %L319
L319:
  %t1370 = phi i64 [ %t1369, %L317 ], [ 0, %L318 ]
  %t1371 = icmp ne i64 %t1370, 0
  br i1 %t1371, label %L320, label %L322
L320:
  %t1372 = load ptr, ptr %t1354
  %t1373 = call ptr @default_ptr_type()
  %t1374 = call i64 @make_val(ptr %t1372, ptr %t1373)
  ret i64 %t1374
L323:
  br label %L322
L322:
  %t1375 = load i64, ptr %t1079
  %t1377 = sext i32 %t1375 to i64
  %t1376 = icmp ne i64 %t1377, 0
  br i1 %t1376, label %L324, label %L326
L324:
  %t1378 = load ptr, ptr %t1354
  %t1379 = call ptr @default_i64_type()
  %t1380 = call i64 @make_val(ptr %t1378, ptr %t1379)
  ret i64 %t1380
L327:
  br label %L326
L326:
  %t1381 = load i64, ptr %t1066
  %t1383 = sext i32 %t1381 to i64
  %t1382 = icmp ne i64 %t1383, 0
  br i1 %t1382, label %L328, label %L330
L328:
  %t1384 = load ptr, ptr %t1354
  %t1385 = call ptr @default_fp_type()
  %t1386 = call i64 @make_val(ptr %t1384, ptr %t1385)
  ret i64 %t1386
L331:
  br label %L330
L330:
  %t1387 = load ptr, ptr %t1354
  %t1388 = call ptr @default_i64_type()
  %t1389 = call i64 @make_val(ptr %t1387, ptr %t1388)
  ret i64 %t1389
L332:
  br label %L12
L12:
  %t1390 = alloca i64
  %t1391 = getelementptr i8, ptr %t1, i64 0
  %t1392 = load i64, ptr %t1391
  %t1393 = inttoptr i64 %t1392 to ptr
  %t1394 = sext i32 0 to i64
  %t1395 = getelementptr ptr, ptr %t1393, i64 %t1394
  %t1396 = load ptr, ptr %t1395
  %t1397 = call i64 @emit_expr(ptr %t0, ptr %t1396)
  store i64 %t1397, ptr %t1390
  %t1398 = alloca i64
  %t1399 = call i32 @new_reg(ptr %t0)
  %t1400 = sext i32 %t1399 to i64
  store i64 %t1400, ptr %t1398
  %t1401 = alloca i64
  %t1402 = getelementptr i8, ptr %t1390, i64 0
  %t1403 = load i64, ptr %t1402
  %t1404 = call i32 @type_is_fp(i64 %t1403)
  %t1405 = sext i32 %t1404 to i64
  store i64 %t1405, ptr %t1401
  %t1406 = alloca ptr
  %t1407 = load i64, ptr %t1401
  %t1409 = sext i32 %t1407 to i64
  %t1410 = icmp eq i64 %t1409, 0
  %t1408 = zext i1 %t1410 to i64
  %t1411 = icmp ne i64 %t1408, 0
  br i1 %t1411, label %L333, label %L335
L333:
  %t1412 = load i64, ptr %t1390
  %t1413 = load ptr, ptr %t1406
  %t1414 = call i32 @promote_to_i64(ptr %t0, i64 %t1412, ptr %t1413, i64 64)
  %t1415 = sext i32 %t1414 to i64
  br label %L335
L335:
  %t1416 = getelementptr i8, ptr %t1, i64 0
  %t1417 = load i64, ptr %t1416
  %t1418 = add i64 %t1417, 0
  switch i64 %t1418, label %L341 [
    i64 36, label %L337
    i64 54, label %L338
    i64 43, label %L339
    i64 35, label %L340
  ]
L337:
  %t1419 = load i64, ptr %t1401
  %t1421 = sext i32 %t1419 to i64
  %t1420 = icmp ne i64 %t1421, 0
  br i1 %t1420, label %L342, label %L343
L342:
  %t1422 = getelementptr i8, ptr %t0, i64 0
  %t1423 = load i64, ptr %t1422
  %t1424 = getelementptr [26 x i8], ptr @.str143, i64 0, i64 0
  %t1425 = load i64, ptr %t1398
  %t1426 = getelementptr i8, ptr %t1390, i64 0
  %t1427 = load i64, ptr %t1426
  call void (ptr, ...) @__c0c_emit(i64 %t1423, ptr %t1424, i64 %t1425, i64 %t1427)
  br label %L344
L343:
  %t1429 = getelementptr i8, ptr %t0, i64 0
  %t1430 = load i64, ptr %t1429
  %t1431 = getelementptr [25 x i8], ptr @.str144, i64 0, i64 0
  %t1432 = load i64, ptr %t1398
  %t1433 = load ptr, ptr %t1406
  call void (ptr, ...) @__c0c_emit(i64 %t1430, ptr %t1431, i64 %t1432, ptr %t1433)
  br label %L344
L344:
  br label %L336
L345:
  br label %L338
L338:
  %t1435 = alloca i64
  %t1436 = call i32 @new_reg(ptr %t0)
  %t1437 = sext i32 %t1436 to i64
  store i64 %t1437, ptr %t1435
  %t1438 = getelementptr i8, ptr %t0, i64 0
  %t1439 = load i64, ptr %t1438
  %t1440 = getelementptr [29 x i8], ptr @.str145, i64 0, i64 0
  %t1441 = load i64, ptr %t1435
  %t1442 = load ptr, ptr %t1406
  call void (ptr, ...) @__c0c_emit(i64 %t1439, ptr %t1440, i64 %t1441, ptr %t1442)
  %t1444 = getelementptr i8, ptr %t0, i64 0
  %t1445 = load i64, ptr %t1444
  %t1446 = getelementptr [32 x i8], ptr @.str146, i64 0, i64 0
  %t1447 = load i64, ptr %t1398
  %t1448 = load i64, ptr %t1435
  call void (ptr, ...) @__c0c_emit(i64 %t1445, ptr %t1446, i64 %t1447, i64 %t1448)
  br label %L336
L346:
  br label %L339
L339:
  %t1450 = getelementptr i8, ptr %t0, i64 0
  %t1451 = load i64, ptr %t1450
  %t1452 = getelementptr [26 x i8], ptr @.str147, i64 0, i64 0
  %t1453 = load i64, ptr %t1398
  %t1454 = load ptr, ptr %t1406
  call void (ptr, ...) @__c0c_emit(i64 %t1451, ptr %t1452, i64 %t1453, ptr %t1454)
  br label %L336
L347:
  br label %L340
L340:
  %t1456 = load i64, ptr %t1390
  %t1457 = sext i32 %t1456 to i64
  ret i64 %t1457
L348:
  br label %L336
L341:
  %t1458 = getelementptr i8, ptr %t0, i64 0
  %t1459 = load i64, ptr %t1458
  %t1460 = getelementptr [25 x i8], ptr @.str148, i64 0, i64 0
  %t1461 = load i64, ptr %t1398
  %t1462 = load ptr, ptr %t1406
  call void (ptr, ...) @__c0c_emit(i64 %t1459, ptr %t1460, i64 %t1461, ptr %t1462)
  br label %L336
L336:
  %t1464 = alloca ptr
  %t1465 = load ptr, ptr %t1464
  %t1466 = getelementptr [6 x i8], ptr @.str149, i64 0, i64 0
  %t1467 = load i64, ptr %t1398
  %t1468 = call i32 (ptr, ...) @snprintf(ptr %t1465, i64 8, ptr %t1466, i64 %t1467)
  %t1469 = sext i32 %t1468 to i64
  %t1470 = load ptr, ptr %t1464
  %t1471 = load i64, ptr %t1401
  %t1473 = sext i32 %t1471 to i64
  %t1472 = icmp ne i64 %t1473, 0
  br i1 %t1472, label %L349, label %L350
L349:
  %t1474 = getelementptr i8, ptr %t1390, i64 0
  %t1475 = load i64, ptr %t1474
  br label %L351
L350:
  %t1476 = call ptr @default_i64_type()
  %t1477 = ptrtoint ptr %t1476 to i64
  br label %L351
L351:
  %t1478 = phi i64 [ %t1475, %L349 ], [ %t1477, %L350 ]
  %t1479 = call i64 @make_val(ptr %t1470, i64 %t1478)
  ret i64 %t1479
L352:
  br label %L13
L13:
  %t1480 = alloca i64
  %t1481 = getelementptr i8, ptr %t1, i64 0
  %t1482 = load i64, ptr %t1481
  %t1483 = inttoptr i64 %t1482 to ptr
  %t1484 = sext i32 1 to i64
  %t1485 = getelementptr ptr, ptr %t1483, i64 %t1484
  %t1486 = load ptr, ptr %t1485
  %t1487 = call i64 @emit_expr(ptr %t0, ptr %t1486)
  store i64 %t1487, ptr %t1480
  %t1488 = alloca ptr
  %t1489 = getelementptr i8, ptr %t1, i64 0
  %t1490 = load i64, ptr %t1489
  %t1491 = inttoptr i64 %t1490 to ptr
  %t1492 = sext i32 0 to i64
  %t1493 = getelementptr ptr, ptr %t1491, i64 %t1492
  %t1494 = load ptr, ptr %t1493
  %t1495 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t1494)
  store ptr %t1495, ptr %t1488
  %t1496 = load ptr, ptr %t1488
  %t1497 = icmp ne ptr %t1496, null
  br i1 %t1497, label %L353, label %L355
L353:
  %t1498 = alloca ptr
  %t1499 = load i64, ptr %t1480
  %t1500 = call i32 @val_is_ptr(i64 %t1499)
  %t1501 = sext i32 %t1500 to i64
  %t1502 = icmp ne i64 %t1501, 0
  br i1 %t1502, label %L356, label %L357
L356:
  %t1503 = getelementptr [4 x i8], ptr @.str150, i64 0, i64 0
  store ptr %t1503, ptr %t1498
  br label %L358
L357:
  %t1504 = getelementptr i8, ptr %t1480, i64 0
  %t1505 = load i64, ptr %t1504
  %t1506 = call i32 @type_is_fp(i64 %t1505)
  %t1507 = sext i32 %t1506 to i64
  %t1508 = icmp ne i64 %t1507, 0
  br i1 %t1508, label %L359, label %L360
L359:
  %t1509 = getelementptr i8, ptr %t1480, i64 0
  %t1510 = load i64, ptr %t1509
  %t1511 = call ptr @llvm_type(i64 %t1510)
  store ptr %t1511, ptr %t1498
  br label %L361
L360:
  %t1512 = getelementptr [4 x i8], ptr @.str151, i64 0, i64 0
  store ptr %t1512, ptr %t1498
  br label %L361
L361:
  br label %L358
L358:
  %t1513 = alloca ptr
  %t1514 = load i64, ptr %t1480
  %t1515 = call i32 @val_is_ptr(i64 %t1514)
  %t1516 = sext i32 %t1515 to i64
  %t1518 = icmp eq i64 %t1516, 0
  %t1517 = zext i1 %t1518 to i64
  %t1519 = icmp ne i64 %t1517, 0
  br i1 %t1519, label %L362, label %L363
L362:
  %t1520 = load i64, ptr %t1480
  %t1521 = call i32 @val_is_64bit(i64 %t1520)
  %t1522 = sext i32 %t1521 to i64
  %t1524 = icmp eq i64 %t1522, 0
  %t1523 = zext i1 %t1524 to i64
  %t1525 = icmp ne i64 %t1523, 0
  %t1526 = zext i1 %t1525 to i64
  br label %L364
L363:
  br label %L364
L364:
  %t1527 = phi i64 [ %t1526, %L362 ], [ 0, %L363 ]
  %t1528 = icmp ne i64 %t1527, 0
  br i1 %t1528, label %L365, label %L366
L365:
  %t1529 = getelementptr i8, ptr %t1480, i64 0
  %t1530 = load i64, ptr %t1529
  %t1531 = call i32 @type_is_fp(i64 %t1530)
  %t1532 = sext i32 %t1531 to i64
  %t1534 = icmp eq i64 %t1532, 0
  %t1533 = zext i1 %t1534 to i64
  %t1535 = icmp ne i64 %t1533, 0
  %t1536 = zext i1 %t1535 to i64
  br label %L367
L366:
  br label %L367
L367:
  %t1537 = phi i64 [ %t1536, %L365 ], [ 0, %L366 ]
  %t1538 = icmp ne i64 %t1537, 0
  br i1 %t1538, label %L368, label %L369
L368:
  %t1539 = alloca i64
  %t1540 = call i32 @new_reg(ptr %t0)
  %t1541 = sext i32 %t1540 to i64
  store i64 %t1541, ptr %t1539
  %t1542 = getelementptr i8, ptr %t0, i64 0
  %t1543 = load i64, ptr %t1542
  %t1544 = getelementptr [30 x i8], ptr @.str152, i64 0, i64 0
  %t1545 = load i64, ptr %t1539
  %t1546 = getelementptr i8, ptr %t1480, i64 0
  %t1547 = load i64, ptr %t1546
  call void (ptr, ...) @__c0c_emit(i64 %t1543, ptr %t1544, i64 %t1545, i64 %t1547)
  %t1549 = load ptr, ptr %t1513
  %t1550 = getelementptr [6 x i8], ptr @.str153, i64 0, i64 0
  %t1551 = load i64, ptr %t1539
  %t1552 = call i32 (ptr, ...) @snprintf(ptr %t1549, i64 64, ptr %t1550, i64 %t1551)
  %t1553 = sext i32 %t1552 to i64
  br label %L370
L369:
  %t1554 = load ptr, ptr %t1513
  %t1555 = getelementptr i8, ptr %t1480, i64 0
  %t1556 = load i64, ptr %t1555
  %t1557 = call ptr @strncpy(ptr %t1554, i64 %t1556, i64 63)
  %t1558 = load ptr, ptr %t1513
  %t1560 = sext i32 63 to i64
  %t1559 = getelementptr ptr, ptr %t1558, i64 %t1560
  %t1561 = sext i32 0 to i64
  store i64 %t1561, ptr %t1559
  br label %L370
L370:
  %t1562 = getelementptr i8, ptr %t0, i64 0
  %t1563 = load i64, ptr %t1562
  %t1564 = getelementptr [23 x i8], ptr @.str154, i64 0, i64 0
  %t1565 = load ptr, ptr %t1498
  %t1566 = load ptr, ptr %t1513
  %t1567 = load ptr, ptr %t1488
  call void (ptr, ...) @__c0c_emit(i64 %t1563, ptr %t1564, ptr %t1565, ptr %t1566, ptr %t1567)
  %t1569 = load ptr, ptr %t1488
  call void @free(ptr %t1569)
  br label %L355
L355:
  %t1571 = load i64, ptr %t1480
  %t1572 = sext i32 %t1571 to i64
  ret i64 %t1572
L371:
  br label %L14
L14:
  %t1573 = alloca i64
  %t1574 = getelementptr i8, ptr %t1, i64 0
  %t1575 = load i64, ptr %t1574
  %t1576 = inttoptr i64 %t1575 to ptr
  %t1577 = sext i32 0 to i64
  %t1578 = getelementptr ptr, ptr %t1576, i64 %t1577
  %t1579 = load ptr, ptr %t1578
  %t1580 = call i64 @emit_expr(ptr %t0, ptr %t1579)
  store i64 %t1580, ptr %t1573
  %t1581 = alloca i64
  %t1582 = getelementptr i8, ptr %t1, i64 0
  %t1583 = load i64, ptr %t1582
  %t1584 = inttoptr i64 %t1583 to ptr
  %t1585 = sext i32 1 to i64
  %t1586 = getelementptr ptr, ptr %t1584, i64 %t1585
  %t1587 = load ptr, ptr %t1586
  %t1588 = call i64 @emit_expr(ptr %t0, ptr %t1587)
  store i64 %t1588, ptr %t1581
  %t1589 = alloca i64
  %t1590 = call i32 @new_reg(ptr %t0)
  %t1591 = sext i32 %t1590 to i64
  store i64 %t1591, ptr %t1589
  %t1592 = alloca i64
  %t1593 = getelementptr i8, ptr %t1573, i64 0
  %t1594 = load i64, ptr %t1593
  %t1595 = call i32 @type_is_fp(i64 %t1594)
  %t1596 = sext i32 %t1595 to i64
  %t1597 = icmp ne i64 %t1596, 0
  br i1 %t1597, label %L372, label %L373
L372:
  br label %L374
L373:
  %t1598 = getelementptr i8, ptr %t1581, i64 0
  %t1599 = load i64, ptr %t1598
  %t1600 = call i32 @type_is_fp(i64 %t1599)
  %t1601 = sext i32 %t1600 to i64
  %t1602 = icmp ne i64 %t1601, 0
  %t1603 = zext i1 %t1602 to i64
  br label %L374
L374:
  %t1604 = phi i64 [ 1, %L372 ], [ %t1603, %L373 ]
  store i64 %t1604, ptr %t1592
  %t1605 = alloca ptr
  %t1606 = load i64, ptr %t1592
  %t1608 = sext i32 %t1606 to i64
  %t1607 = icmp ne i64 %t1608, 0
  br i1 %t1607, label %L375, label %L376
L375:
  %t1609 = getelementptr [7 x i8], ptr @.str155, i64 0, i64 0
  %t1610 = ptrtoint ptr %t1609 to i64
  br label %L377
L376:
  %t1611 = getelementptr [4 x i8], ptr @.str156, i64 0, i64 0
  %t1612 = ptrtoint ptr %t1611 to i64
  br label %L377
L377:
  %t1613 = phi i64 [ %t1610, %L375 ], [ %t1612, %L376 ]
  store i64 %t1613, ptr %t1605
  %t1614 = alloca ptr
  %t1615 = alloca ptr
  %t1616 = load i64, ptr %t1592
  %t1618 = sext i32 %t1616 to i64
  %t1619 = icmp eq i64 %t1618, 0
  %t1617 = zext i1 %t1619 to i64
  %t1620 = icmp ne i64 %t1617, 0
  br i1 %t1620, label %L378, label %L379
L378:
  %t1621 = load i64, ptr %t1573
  %t1622 = load ptr, ptr %t1614
  %t1623 = call i32 @promote_to_i64(ptr %t0, i64 %t1621, ptr %t1622, i64 64)
  %t1624 = sext i32 %t1623 to i64
  %t1625 = load i64, ptr %t1581
  %t1626 = load ptr, ptr %t1615
  %t1627 = call i32 @promote_to_i64(ptr %t0, i64 %t1625, ptr %t1626, i64 64)
  %t1628 = sext i32 %t1627 to i64
  br label %L380
L379:
  %t1629 = load ptr, ptr %t1614
  %t1630 = getelementptr i8, ptr %t1573, i64 0
  %t1631 = load i64, ptr %t1630
  %t1632 = call ptr @strncpy(ptr %t1629, i64 %t1631, i64 63)
  %t1633 = load ptr, ptr %t1614
  %t1635 = sext i32 63 to i64
  %t1634 = getelementptr ptr, ptr %t1633, i64 %t1635
  %t1636 = sext i32 0 to i64
  store i64 %t1636, ptr %t1634
  %t1637 = load ptr, ptr %t1615
  %t1638 = getelementptr i8, ptr %t1581, i64 0
  %t1639 = load i64, ptr %t1638
  %t1640 = call ptr @strncpy(ptr %t1637, i64 %t1639, i64 63)
  %t1641 = load ptr, ptr %t1615
  %t1643 = sext i32 63 to i64
  %t1642 = getelementptr ptr, ptr %t1641, i64 %t1643
  %t1644 = sext i32 0 to i64
  store i64 %t1644, ptr %t1642
  br label %L380
L380:
  %t1645 = alloca ptr
  %t1646 = getelementptr i8, ptr %t1, i64 0
  %t1647 = load i64, ptr %t1646
  %t1648 = add i64 %t1647, 0
  switch i64 %t1648, label %L392 [
    i64 56, label %L382
    i64 57, label %L383
    i64 58, label %L384
    i64 59, label %L385
    i64 65, label %L386
    i64 60, label %L387
    i64 61, label %L388
    i64 62, label %L389
    i64 63, label %L390
    i64 64, label %L391
  ]
L382:
  %t1649 = load i64, ptr %t1592
  %t1651 = sext i32 %t1649 to i64
  %t1650 = icmp ne i64 %t1651, 0
  br i1 %t1650, label %L393, label %L394
L393:
  %t1652 = getelementptr [5 x i8], ptr @.str157, i64 0, i64 0
  %t1653 = ptrtoint ptr %t1652 to i64
  br label %L395
L394:
  %t1654 = getelementptr [4 x i8], ptr @.str158, i64 0, i64 0
  %t1655 = ptrtoint ptr %t1654 to i64
  br label %L395
L395:
  %t1656 = phi i64 [ %t1653, %L393 ], [ %t1655, %L394 ]
  store i64 %t1656, ptr %t1645
  br label %L381
L396:
  br label %L383
L383:
  %t1657 = load i64, ptr %t1592
  %t1659 = sext i32 %t1657 to i64
  %t1658 = icmp ne i64 %t1659, 0
  br i1 %t1658, label %L397, label %L398
L397:
  %t1660 = getelementptr [5 x i8], ptr @.str159, i64 0, i64 0
  %t1661 = ptrtoint ptr %t1660 to i64
  br label %L399
L398:
  %t1662 = getelementptr [4 x i8], ptr @.str160, i64 0, i64 0
  %t1663 = ptrtoint ptr %t1662 to i64
  br label %L399
L399:
  %t1664 = phi i64 [ %t1661, %L397 ], [ %t1663, %L398 ]
  store i64 %t1664, ptr %t1645
  br label %L381
L400:
  br label %L384
L384:
  %t1665 = load i64, ptr %t1592
  %t1667 = sext i32 %t1665 to i64
  %t1666 = icmp ne i64 %t1667, 0
  br i1 %t1666, label %L401, label %L402
L401:
  %t1668 = getelementptr [5 x i8], ptr @.str161, i64 0, i64 0
  %t1669 = ptrtoint ptr %t1668 to i64
  br label %L403
L402:
  %t1670 = getelementptr [4 x i8], ptr @.str162, i64 0, i64 0
  %t1671 = ptrtoint ptr %t1670 to i64
  br label %L403
L403:
  %t1672 = phi i64 [ %t1669, %L401 ], [ %t1671, %L402 ]
  store i64 %t1672, ptr %t1645
  br label %L381
L404:
  br label %L385
L385:
  %t1673 = load i64, ptr %t1592
  %t1675 = sext i32 %t1673 to i64
  %t1674 = icmp ne i64 %t1675, 0
  br i1 %t1674, label %L405, label %L406
L405:
  %t1676 = getelementptr [5 x i8], ptr @.str163, i64 0, i64 0
  %t1677 = ptrtoint ptr %t1676 to i64
  br label %L407
L406:
  %t1678 = getelementptr [5 x i8], ptr @.str164, i64 0, i64 0
  %t1679 = ptrtoint ptr %t1678 to i64
  br label %L407
L407:
  %t1680 = phi i64 [ %t1677, %L405 ], [ %t1679, %L406 ]
  store i64 %t1680, ptr %t1645
  br label %L381
L408:
  br label %L386
L386:
  %t1681 = getelementptr [5 x i8], ptr @.str165, i64 0, i64 0
  store ptr %t1681, ptr %t1645
  br label %L381
L409:
  br label %L387
L387:
  %t1682 = getelementptr [4 x i8], ptr @.str166, i64 0, i64 0
  store ptr %t1682, ptr %t1645
  br label %L381
L410:
  br label %L388
L388:
  %t1683 = getelementptr [3 x i8], ptr @.str167, i64 0, i64 0
  store ptr %t1683, ptr %t1645
  br label %L381
L411:
  br label %L389
L389:
  %t1684 = getelementptr [4 x i8], ptr @.str168, i64 0, i64 0
  store ptr %t1684, ptr %t1645
  br label %L381
L412:
  br label %L390
L390:
  %t1685 = getelementptr [4 x i8], ptr @.str169, i64 0, i64 0
  store ptr %t1685, ptr %t1645
  br label %L381
L413:
  br label %L391
L391:
  %t1686 = getelementptr [5 x i8], ptr @.str170, i64 0, i64 0
  store ptr %t1686, ptr %t1645
  br label %L381
L414:
  br label %L381
L392:
  %t1687 = getelementptr [4 x i8], ptr @.str171, i64 0, i64 0
  store ptr %t1687, ptr %t1645
  br label %L381
L381:
  %t1688 = getelementptr i8, ptr %t0, i64 0
  %t1689 = load i64, ptr %t1688
  %t1690 = getelementptr [24 x i8], ptr @.str172, i64 0, i64 0
  %t1691 = load i64, ptr %t1589
  %t1692 = load ptr, ptr %t1645
  %t1693 = load ptr, ptr %t1605
  %t1694 = load ptr, ptr %t1614
  %t1695 = load ptr, ptr %t1615
  call void (ptr, ...) @__c0c_emit(i64 %t1689, ptr %t1690, i64 %t1691, ptr %t1692, ptr %t1693, ptr %t1694, ptr %t1695)
  %t1697 = alloca ptr
  %t1698 = getelementptr i8, ptr %t1, i64 0
  %t1699 = load i64, ptr %t1698
  %t1700 = inttoptr i64 %t1699 to ptr
  %t1701 = sext i32 0 to i64
  %t1702 = getelementptr ptr, ptr %t1700, i64 %t1701
  %t1703 = load ptr, ptr %t1702
  %t1704 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t1703)
  store ptr %t1704, ptr %t1697
  %t1705 = load ptr, ptr %t1697
  %t1706 = icmp ne ptr %t1705, null
  br i1 %t1706, label %L415, label %L417
L415:
  %t1707 = getelementptr i8, ptr %t0, i64 0
  %t1708 = load i64, ptr %t1707
  %t1709 = getelementptr [26 x i8], ptr @.str173, i64 0, i64 0
  %t1710 = load ptr, ptr %t1605
  %t1711 = load i64, ptr %t1589
  %t1712 = load ptr, ptr %t1697
  call void (ptr, ...) @__c0c_emit(i64 %t1708, ptr %t1709, ptr %t1710, i64 %t1711, ptr %t1712)
  %t1714 = load ptr, ptr %t1697
  call void @free(ptr %t1714)
  br label %L417
L417:
  %t1716 = alloca ptr
  %t1717 = load ptr, ptr %t1716
  %t1718 = getelementptr [6 x i8], ptr @.str174, i64 0, i64 0
  %t1719 = load i64, ptr %t1589
  %t1720 = call i32 (ptr, ...) @snprintf(ptr %t1717, i64 8, ptr %t1718, i64 %t1719)
  %t1721 = sext i32 %t1720 to i64
  %t1722 = load ptr, ptr %t1716
  %t1723 = load i64, ptr %t1592
  %t1725 = sext i32 %t1723 to i64
  %t1724 = icmp ne i64 %t1725, 0
  br i1 %t1724, label %L418, label %L419
L418:
  %t1726 = getelementptr i8, ptr %t1573, i64 0
  %t1727 = load i64, ptr %t1726
  br label %L420
L419:
  %t1728 = call ptr @default_i64_type()
  %t1729 = ptrtoint ptr %t1728 to i64
  br label %L420
L420:
  %t1730 = phi i64 [ %t1727, %L418 ], [ %t1729, %L419 ]
  %t1731 = call i64 @make_val(ptr %t1722, i64 %t1730)
  ret i64 %t1731
L421:
  br label %L15
L15:
  br label %L16
L16:
  %t1732 = alloca i64
  %t1733 = getelementptr i8, ptr %t1, i64 0
  %t1734 = load i64, ptr %t1733
  %t1735 = inttoptr i64 %t1734 to ptr
  %t1736 = sext i32 0 to i64
  %t1737 = getelementptr ptr, ptr %t1735, i64 %t1736
  %t1738 = load ptr, ptr %t1737
  %t1739 = call i64 @emit_expr(ptr %t0, ptr %t1738)
  store i64 %t1739, ptr %t1732
  %t1740 = alloca i64
  %t1741 = call i32 @new_reg(ptr %t0)
  %t1742 = sext i32 %t1741 to i64
  store i64 %t1742, ptr %t1740
  %t1743 = alloca ptr
  %t1744 = getelementptr i8, ptr %t1, i64 0
  %t1745 = load i64, ptr %t1744
  %t1747 = sext i32 38 to i64
  %t1746 = icmp eq i64 %t1745, %t1747
  %t1748 = zext i1 %t1746 to i64
  %t1749 = icmp ne i64 %t1748, 0
  br i1 %t1749, label %L422, label %L423
L422:
  %t1750 = getelementptr [4 x i8], ptr @.str175, i64 0, i64 0
  %t1751 = ptrtoint ptr %t1750 to i64
  br label %L424
L423:
  %t1752 = getelementptr [4 x i8], ptr @.str176, i64 0, i64 0
  %t1753 = ptrtoint ptr %t1752 to i64
  br label %L424
L424:
  %t1754 = phi i64 [ %t1751, %L422 ], [ %t1753, %L423 ]
  store i64 %t1754, ptr %t1743
  %t1755 = alloca ptr
  %t1756 = load i64, ptr %t1732
  %t1757 = load ptr, ptr %t1755
  %t1758 = call i32 @promote_to_i64(ptr %t0, i64 %t1756, ptr %t1757, i64 64)
  %t1759 = sext i32 %t1758 to i64
  %t1760 = getelementptr i8, ptr %t0, i64 0
  %t1761 = load i64, ptr %t1760
  %t1762 = getelementptr [24 x i8], ptr @.str177, i64 0, i64 0
  %t1763 = load i64, ptr %t1740
  %t1764 = load ptr, ptr %t1743
  %t1765 = load ptr, ptr %t1755
  call void (ptr, ...) @__c0c_emit(i64 %t1761, ptr %t1762, i64 %t1763, ptr %t1764, ptr %t1765)
  %t1767 = alloca ptr
  %t1768 = getelementptr i8, ptr %t1, i64 0
  %t1769 = load i64, ptr %t1768
  %t1770 = inttoptr i64 %t1769 to ptr
  %t1771 = sext i32 0 to i64
  %t1772 = getelementptr ptr, ptr %t1770, i64 %t1771
  %t1773 = load ptr, ptr %t1772
  %t1774 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t1773)
  store ptr %t1774, ptr %t1767
  %t1775 = load ptr, ptr %t1767
  %t1776 = icmp ne ptr %t1775, null
  br i1 %t1776, label %L425, label %L427
L425:
  %t1777 = getelementptr i8, ptr %t0, i64 0
  %t1778 = load i64, ptr %t1777
  %t1779 = getelementptr [27 x i8], ptr @.str178, i64 0, i64 0
  %t1780 = load i64, ptr %t1740
  %t1781 = load ptr, ptr %t1767
  call void (ptr, ...) @__c0c_emit(i64 %t1778, ptr %t1779, i64 %t1780, ptr %t1781)
  %t1783 = load ptr, ptr %t1767
  call void @free(ptr %t1783)
  br label %L427
L427:
  %t1785 = alloca ptr
  %t1786 = load ptr, ptr %t1785
  %t1787 = getelementptr [6 x i8], ptr @.str179, i64 0, i64 0
  %t1788 = load i64, ptr %t1740
  %t1789 = call i32 (ptr, ...) @snprintf(ptr %t1786, i64 8, ptr %t1787, i64 %t1788)
  %t1790 = sext i32 %t1789 to i64
  %t1791 = load ptr, ptr %t1785
  %t1792 = call ptr @default_i64_type()
  %t1793 = call i64 @make_val(ptr %t1791, ptr %t1792)
  ret i64 %t1793
L428:
  br label %L17
L17:
  br label %L18
L18:
  %t1794 = alloca i64
  %t1795 = getelementptr i8, ptr %t1, i64 0
  %t1796 = load i64, ptr %t1795
  %t1797 = inttoptr i64 %t1796 to ptr
  %t1798 = sext i32 0 to i64
  %t1799 = getelementptr ptr, ptr %t1797, i64 %t1798
  %t1800 = load ptr, ptr %t1799
  %t1801 = call i64 @emit_expr(ptr %t0, ptr %t1800)
  store i64 %t1801, ptr %t1794
  %t1802 = alloca i64
  %t1803 = call i32 @new_reg(ptr %t0)
  %t1804 = sext i32 %t1803 to i64
  store i64 %t1804, ptr %t1802
  %t1805 = alloca ptr
  %t1806 = getelementptr i8, ptr %t1, i64 0
  %t1807 = load i64, ptr %t1806
  %t1809 = sext i32 40 to i64
  %t1808 = icmp eq i64 %t1807, %t1809
  %t1810 = zext i1 %t1808 to i64
  %t1811 = icmp ne i64 %t1810, 0
  br i1 %t1811, label %L429, label %L430
L429:
  %t1812 = getelementptr [4 x i8], ptr @.str180, i64 0, i64 0
  %t1813 = ptrtoint ptr %t1812 to i64
  br label %L431
L430:
  %t1814 = getelementptr [4 x i8], ptr @.str181, i64 0, i64 0
  %t1815 = ptrtoint ptr %t1814 to i64
  br label %L431
L431:
  %t1816 = phi i64 [ %t1813, %L429 ], [ %t1815, %L430 ]
  store i64 %t1816, ptr %t1805
  %t1817 = alloca ptr
  %t1818 = load i64, ptr %t1794
  %t1819 = load ptr, ptr %t1817
  %t1820 = call i32 @promote_to_i64(ptr %t0, i64 %t1818, ptr %t1819, i64 64)
  %t1821 = sext i32 %t1820 to i64
  %t1822 = getelementptr i8, ptr %t0, i64 0
  %t1823 = load i64, ptr %t1822
  %t1824 = getelementptr [24 x i8], ptr @.str182, i64 0, i64 0
  %t1825 = load i64, ptr %t1802
  %t1826 = load ptr, ptr %t1805
  %t1827 = load ptr, ptr %t1817
  call void (ptr, ...) @__c0c_emit(i64 %t1823, ptr %t1824, i64 %t1825, ptr %t1826, ptr %t1827)
  %t1829 = alloca ptr
  %t1830 = getelementptr i8, ptr %t1, i64 0
  %t1831 = load i64, ptr %t1830
  %t1832 = inttoptr i64 %t1831 to ptr
  %t1833 = sext i32 0 to i64
  %t1834 = getelementptr ptr, ptr %t1832, i64 %t1833
  %t1835 = load ptr, ptr %t1834
  %t1836 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t1835)
  store ptr %t1836, ptr %t1829
  %t1837 = load ptr, ptr %t1829
  %t1838 = icmp ne ptr %t1837, null
  br i1 %t1838, label %L432, label %L434
L432:
  %t1839 = getelementptr i8, ptr %t0, i64 0
  %t1840 = load i64, ptr %t1839
  %t1841 = getelementptr [27 x i8], ptr @.str183, i64 0, i64 0
  %t1842 = load i64, ptr %t1802
  %t1843 = load ptr, ptr %t1829
  call void (ptr, ...) @__c0c_emit(i64 %t1840, ptr %t1841, i64 %t1842, ptr %t1843)
  %t1845 = load ptr, ptr %t1829
  call void @free(ptr %t1845)
  br label %L434
L434:
  %t1847 = load i64, ptr %t1794
  %t1848 = sext i32 %t1847 to i64
  ret i64 %t1848
L435:
  br label %L19
L19:
  %t1849 = alloca ptr
  %t1850 = getelementptr i8, ptr %t1, i64 0
  %t1851 = load i64, ptr %t1850
  %t1852 = inttoptr i64 %t1851 to ptr
  %t1853 = sext i32 0 to i64
  %t1854 = getelementptr ptr, ptr %t1852, i64 %t1853
  %t1855 = load ptr, ptr %t1854
  %t1856 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t1855)
  store ptr %t1856, ptr %t1849
  %t1857 = load ptr, ptr %t1849
  %t1859 = ptrtoint ptr %t1857 to i64
  %t1860 = icmp eq i64 %t1859, 0
  %t1858 = zext i1 %t1860 to i64
  %t1861 = icmp ne i64 %t1858, 0
  br i1 %t1861, label %L436, label %L438
L436:
  %t1862 = getelementptr [5 x i8], ptr @.str184, i64 0, i64 0
  %t1863 = call ptr @default_ptr_type()
  %t1864 = call i64 @make_val(ptr %t1862, ptr %t1863)
  ret i64 %t1864
L439:
  br label %L438
L438:
  %t1865 = alloca i64
  %t1866 = load ptr, ptr %t1849
  %t1867 = call ptr @default_ptr_type()
  %t1868 = call i64 @make_val(ptr %t1866, ptr %t1867)
  store i64 %t1868, ptr %t1865
  %t1869 = load ptr, ptr %t1849
  call void @free(ptr %t1869)
  %t1871 = load i64, ptr %t1865
  %t1872 = sext i32 %t1871 to i64
  ret i64 %t1872
L440:
  br label %L20
L20:
  %t1873 = alloca i64
  %t1874 = getelementptr i8, ptr %t1, i64 0
  %t1875 = load i64, ptr %t1874
  %t1876 = inttoptr i64 %t1875 to ptr
  %t1877 = sext i32 0 to i64
  %t1878 = getelementptr ptr, ptr %t1876, i64 %t1877
  %t1879 = load ptr, ptr %t1878
  %t1880 = call i64 @emit_expr(ptr %t0, ptr %t1879)
  store i64 %t1880, ptr %t1873
  %t1881 = alloca i64
  %t1882 = call i32 @new_reg(ptr %t0)
  %t1883 = sext i32 %t1882 to i64
  store i64 %t1883, ptr %t1881
  %t1884 = alloca ptr
  %t1885 = load i64, ptr %t1873
  %t1886 = call i32 @val_is_ptr(i64 %t1885)
  %t1887 = sext i32 %t1886 to i64
  %t1888 = icmp ne i64 %t1887, 0
  br i1 %t1888, label %L441, label %L442
L441:
  %t1889 = load ptr, ptr %t1884
  %t1890 = getelementptr i8, ptr %t1873, i64 0
  %t1891 = load i64, ptr %t1890
  %t1892 = call ptr @strncpy(ptr %t1889, i64 %t1891, i64 63)
  %t1893 = load ptr, ptr %t1884
  %t1895 = sext i32 63 to i64
  %t1894 = getelementptr ptr, ptr %t1893, i64 %t1895
  %t1896 = sext i32 0 to i64
  store i64 %t1896, ptr %t1894
  br label %L443
L442:
  %t1897 = alloca i64
  %t1898 = call i32 @new_reg(ptr %t0)
  %t1899 = sext i32 %t1898 to i64
  store i64 %t1899, ptr %t1897
  %t1900 = getelementptr i8, ptr %t0, i64 0
  %t1901 = load i64, ptr %t1900
  %t1902 = getelementptr [34 x i8], ptr @.str185, i64 0, i64 0
  %t1903 = load i64, ptr %t1897
  %t1904 = getelementptr i8, ptr %t1873, i64 0
  %t1905 = load i64, ptr %t1904
  call void (ptr, ...) @__c0c_emit(i64 %t1901, ptr %t1902, i64 %t1903, i64 %t1905)
  %t1907 = load ptr, ptr %t1884
  %t1908 = getelementptr [6 x i8], ptr @.str186, i64 0, i64 0
  %t1909 = load i64, ptr %t1897
  %t1910 = call i32 (ptr, ...) @snprintf(ptr %t1907, i64 64, ptr %t1908, i64 %t1909)
  %t1911 = sext i32 %t1910 to i64
  br label %L443
L443:
  %t1912 = alloca ptr
  %t1913 = getelementptr i8, ptr %t1873, i64 0
  %t1914 = load i64, ptr %t1913
  %t1915 = icmp ne i64 %t1914, 0
  br i1 %t1915, label %L444, label %L445
L444:
  %t1916 = getelementptr i8, ptr %t1873, i64 0
  %t1917 = load i64, ptr %t1916
  %t1918 = inttoptr i64 %t1917 to ptr
  %t1919 = getelementptr i8, ptr %t1918, i64 0
  %t1920 = load i64, ptr %t1919
  %t1921 = icmp ne i64 %t1920, 0
  %t1922 = zext i1 %t1921 to i64
  br label %L446
L445:
  br label %L446
L446:
  %t1923 = phi i64 [ %t1922, %L444 ], [ 0, %L445 ]
  %t1924 = icmp ne i64 %t1923, 0
  br i1 %t1924, label %L447, label %L448
L447:
  %t1925 = getelementptr i8, ptr %t1873, i64 0
  %t1926 = load i64, ptr %t1925
  %t1927 = inttoptr i64 %t1926 to ptr
  %t1928 = getelementptr i8, ptr %t1927, i64 0
  %t1929 = load i64, ptr %t1928
  br label %L449
L448:
  %t1930 = call ptr @default_int_type()
  %t1931 = ptrtoint ptr %t1930 to i64
  br label %L449
L449:
  %t1932 = phi i64 [ %t1929, %L447 ], [ %t1931, %L448 ]
  store i64 %t1932, ptr %t1912
  %t1933 = alloca i64
  %t1934 = load ptr, ptr %t1912
  %t1935 = getelementptr i8, ptr %t1934, i64 0
  %t1936 = load i64, ptr %t1935
  %t1938 = sext i32 15 to i64
  %t1937 = icmp eq i64 %t1936, %t1938
  %t1939 = zext i1 %t1937 to i64
  %t1940 = icmp ne i64 %t1939, 0
  br i1 %t1940, label %L450, label %L451
L450:
  br label %L452
L451:
  %t1941 = load ptr, ptr %t1912
  %t1942 = getelementptr i8, ptr %t1941, i64 0
  %t1943 = load i64, ptr %t1942
  %t1945 = sext i32 16 to i64
  %t1944 = icmp eq i64 %t1943, %t1945
  %t1946 = zext i1 %t1944 to i64
  %t1947 = icmp ne i64 %t1946, 0
  %t1948 = zext i1 %t1947 to i64
  br label %L452
L452:
  %t1949 = phi i64 [ 1, %L450 ], [ %t1948, %L451 ]
  store i64 %t1949, ptr %t1933
  %t1950 = alloca ptr
  %t1951 = load i64, ptr %t1933
  %t1953 = sext i32 %t1951 to i64
  %t1952 = icmp ne i64 %t1953, 0
  br i1 %t1952, label %L453, label %L454
L453:
  %t1954 = getelementptr [4 x i8], ptr @.str187, i64 0, i64 0
  %t1955 = ptrtoint ptr %t1954 to i64
  br label %L455
L454:
  %t1956 = getelementptr [4 x i8], ptr @.str188, i64 0, i64 0
  %t1957 = ptrtoint ptr %t1956 to i64
  br label %L455
L455:
  %t1958 = phi i64 [ %t1955, %L453 ], [ %t1957, %L454 ]
  store i64 %t1958, ptr %t1950
  %t1959 = alloca ptr
  %t1960 = load i64, ptr %t1933
  %t1962 = sext i32 %t1960 to i64
  %t1961 = icmp ne i64 %t1962, 0
  br i1 %t1961, label %L456, label %L457
L456:
  %t1963 = call ptr @default_ptr_type()
  %t1964 = ptrtoint ptr %t1963 to i64
  br label %L458
L457:
  %t1965 = call ptr @default_i64_type()
  %t1966 = ptrtoint ptr %t1965 to i64
  br label %L458
L458:
  %t1967 = phi i64 [ %t1964, %L456 ], [ %t1966, %L457 ]
  store i64 %t1967, ptr %t1959
  %t1968 = getelementptr i8, ptr %t0, i64 0
  %t1969 = load i64, ptr %t1968
  %t1970 = getelementptr [27 x i8], ptr @.str189, i64 0, i64 0
  %t1971 = load i64, ptr %t1881
  %t1972 = load ptr, ptr %t1950
  %t1973 = load ptr, ptr %t1884
  call void (ptr, ...) @__c0c_emit(i64 %t1969, ptr %t1970, i64 %t1971, ptr %t1972, ptr %t1973)
  %t1975 = alloca ptr
  %t1976 = load ptr, ptr %t1975
  %t1977 = getelementptr [6 x i8], ptr @.str190, i64 0, i64 0
  %t1978 = load i64, ptr %t1881
  %t1979 = call i32 (ptr, ...) @snprintf(ptr %t1976, i64 8, ptr %t1977, i64 %t1978)
  %t1980 = sext i32 %t1979 to i64
  %t1981 = load ptr, ptr %t1975
  %t1982 = load ptr, ptr %t1959
  %t1983 = call i64 @make_val(ptr %t1981, ptr %t1982)
  ret i64 %t1983
L459:
  br label %L21
L21:
  %t1984 = alloca i64
  %t1985 = getelementptr i8, ptr %t1, i64 0
  %t1986 = load i64, ptr %t1985
  %t1987 = inttoptr i64 %t1986 to ptr
  %t1988 = sext i32 0 to i64
  %t1989 = getelementptr ptr, ptr %t1987, i64 %t1988
  %t1990 = load ptr, ptr %t1989
  %t1991 = call i64 @emit_expr(ptr %t0, ptr %t1990)
  store i64 %t1991, ptr %t1984
  %t1992 = alloca i64
  %t1993 = getelementptr i8, ptr %t1, i64 0
  %t1994 = load i64, ptr %t1993
  %t1995 = inttoptr i64 %t1994 to ptr
  %t1996 = sext i32 1 to i64
  %t1997 = getelementptr ptr, ptr %t1995, i64 %t1996
  %t1998 = load ptr, ptr %t1997
  %t1999 = call i64 @emit_expr(ptr %t0, ptr %t1998)
  store i64 %t1999, ptr %t1992
  %t2000 = alloca ptr
  %t2001 = getelementptr i8, ptr %t1984, i64 0
  %t2002 = load i64, ptr %t2001
  %t2003 = icmp ne i64 %t2002, 0
  br i1 %t2003, label %L460, label %L461
L460:
  %t2004 = getelementptr i8, ptr %t1984, i64 0
  %t2005 = load i64, ptr %t2004
  %t2006 = inttoptr i64 %t2005 to ptr
  %t2007 = getelementptr i8, ptr %t2006, i64 0
  %t2008 = load i64, ptr %t2007
  %t2009 = icmp ne i64 %t2008, 0
  %t2010 = zext i1 %t2009 to i64
  br label %L462
L461:
  br label %L462
L462:
  %t2011 = phi i64 [ %t2010, %L460 ], [ 0, %L461 ]
  %t2012 = icmp ne i64 %t2011, 0
  br i1 %t2012, label %L463, label %L464
L463:
  %t2013 = getelementptr i8, ptr %t1984, i64 0
  %t2014 = load i64, ptr %t2013
  %t2015 = inttoptr i64 %t2014 to ptr
  %t2016 = getelementptr i8, ptr %t2015, i64 0
  %t2017 = load i64, ptr %t2016
  br label %L465
L464:
  %t2019 = sext i32 0 to i64
  %t2018 = inttoptr i64 %t2019 to ptr
  %t2020 = ptrtoint ptr %t2018 to i64
  br label %L465
L465:
  %t2021 = phi i64 [ %t2017, %L463 ], [ %t2020, %L464 ]
  store i64 %t2021, ptr %t2000
  %t2022 = alloca i64
  %t2023 = load ptr, ptr %t2000
  %t2024 = ptrtoint ptr %t2023 to i64
  %t2025 = icmp ne i64 %t2024, 0
  br i1 %t2025, label %L466, label %L467
L466:
  %t2026 = load ptr, ptr %t2000
  %t2027 = getelementptr i8, ptr %t2026, i64 0
  %t2028 = load i64, ptr %t2027
  %t2030 = sext i32 15 to i64
  %t2029 = icmp eq i64 %t2028, %t2030
  %t2031 = zext i1 %t2029 to i64
  %t2032 = icmp ne i64 %t2031, 0
  br i1 %t2032, label %L469, label %L470
L469:
  br label %L471
L470:
  %t2033 = load ptr, ptr %t2000
  %t2034 = getelementptr i8, ptr %t2033, i64 0
  %t2035 = load i64, ptr %t2034
  %t2037 = sext i32 16 to i64
  %t2036 = icmp eq i64 %t2035, %t2037
  %t2038 = zext i1 %t2036 to i64
  %t2039 = icmp ne i64 %t2038, 0
  %t2040 = zext i1 %t2039 to i64
  br label %L471
L471:
  %t2041 = phi i64 [ 1, %L469 ], [ %t2040, %L470 ]
  %t2042 = icmp ne i64 %t2041, 0
  %t2043 = zext i1 %t2042 to i64
  br label %L468
L467:
  br label %L468
L468:
  %t2044 = phi i64 [ %t2043, %L466 ], [ 0, %L467 ]
  store i64 %t2044, ptr %t2022
  %t2045 = alloca i64
  %t2046 = load ptr, ptr %t2000
  %t2047 = ptrtoint ptr %t2046 to i64
  %t2048 = icmp ne i64 %t2047, 0
  br i1 %t2048, label %L472, label %L473
L472:
  %t2049 = load ptr, ptr %t2000
  %t2050 = call i32 @type_is_fp(ptr %t2049)
  %t2051 = sext i32 %t2050 to i64
  %t2052 = icmp ne i64 %t2051, 0
  %t2053 = zext i1 %t2052 to i64
  br label %L474
L473:
  br label %L474
L474:
  %t2054 = phi i64 [ %t2053, %L472 ], [ 0, %L473 ]
  store i64 %t2054, ptr %t2045
  %t2055 = alloca ptr
  %t2056 = alloca ptr
  %t2057 = alloca ptr
  %t2058 = load ptr, ptr %t2000
  %t2060 = ptrtoint ptr %t2058 to i64
  %t2061 = icmp eq i64 %t2060, 0
  %t2059 = zext i1 %t2061 to i64
  %t2062 = icmp ne i64 %t2059, 0
  br i1 %t2062, label %L475, label %L476
L475:
  %t2063 = getelementptr [4 x i8], ptr @.str191, i64 0, i64 0
  store ptr %t2063, ptr %t2055
  %t2064 = getelementptr [4 x i8], ptr @.str192, i64 0, i64 0
  store ptr %t2064, ptr %t2056
  %t2065 = call ptr @default_ptr_type()
  store ptr %t2065, ptr %t2057
  br label %L477
L476:
  %t2066 = load i64, ptr %t2022
  %t2068 = sext i32 %t2066 to i64
  %t2067 = icmp ne i64 %t2068, 0
  br i1 %t2067, label %L478, label %L479
L478:
  %t2069 = getelementptr [4 x i8], ptr @.str193, i64 0, i64 0
  store ptr %t2069, ptr %t2055
  %t2070 = getelementptr [4 x i8], ptr @.str194, i64 0, i64 0
  store ptr %t2070, ptr %t2056
  %t2071 = call ptr @default_ptr_type()
  store ptr %t2071, ptr %t2057
  br label %L480
L479:
  %t2072 = load i64, ptr %t2045
  %t2074 = sext i32 %t2072 to i64
  %t2073 = icmp ne i64 %t2074, 0
  br i1 %t2073, label %L481, label %L482
L481:
  %t2075 = load ptr, ptr %t2000
  %t2076 = call ptr @llvm_type(ptr %t2075)
  store ptr %t2076, ptr %t2055
  %t2077 = load ptr, ptr %t2000
  %t2078 = call ptr @llvm_type(ptr %t2077)
  store ptr %t2078, ptr %t2056
  %t2079 = load ptr, ptr %t2000
  store ptr %t2079, ptr %t2057
  br label %L483
L482:
  %t2080 = getelementptr [4 x i8], ptr @.str195, i64 0, i64 0
  store ptr %t2080, ptr %t2055
  %t2081 = getelementptr [4 x i8], ptr @.str196, i64 0, i64 0
  store ptr %t2081, ptr %t2056
  %t2082 = call ptr @default_i64_type()
  store ptr %t2082, ptr %t2057
  br label %L483
L483:
  br label %L480
L480:
  br label %L477
L477:
  %t2083 = alloca ptr
  %t2084 = alloca ptr
  %t2085 = load i64, ptr %t1984
  %t2086 = call i32 @val_is_ptr(i64 %t2085)
  %t2087 = sext i32 %t2086 to i64
  %t2088 = icmp ne i64 %t2087, 0
  br i1 %t2088, label %L484, label %L485
L484:
  %t2089 = load ptr, ptr %t2083
  %t2090 = getelementptr i8, ptr %t1984, i64 0
  %t2091 = load i64, ptr %t2090
  %t2092 = call ptr @strncpy(ptr %t2089, i64 %t2091, i64 63)
  br label %L486
L485:
  %t2093 = alloca i64
  %t2094 = call i32 @new_reg(ptr %t0)
  %t2095 = sext i32 %t2094 to i64
  store i64 %t2095, ptr %t2093
  %t2096 = getelementptr i8, ptr %t0, i64 0
  %t2097 = load i64, ptr %t2096
  %t2098 = getelementptr [34 x i8], ptr @.str197, i64 0, i64 0
  %t2099 = load i64, ptr %t2093
  %t2100 = getelementptr i8, ptr %t1984, i64 0
  %t2101 = load i64, ptr %t2100
  call void (ptr, ...) @__c0c_emit(i64 %t2097, ptr %t2098, i64 %t2099, i64 %t2101)
  %t2103 = load ptr, ptr %t2083
  %t2104 = getelementptr [6 x i8], ptr @.str198, i64 0, i64 0
  %t2105 = load i64, ptr %t2093
  %t2106 = call i32 (ptr, ...) @snprintf(ptr %t2103, i64 64, ptr %t2104, i64 %t2105)
  %t2107 = sext i32 %t2106 to i64
  br label %L486
L486:
  %t2108 = load i64, ptr %t1992
  %t2109 = load ptr, ptr %t2084
  %t2110 = call i32 @promote_to_i64(ptr %t0, i64 %t2108, ptr %t2109, i64 64)
  %t2111 = sext i32 %t2110 to i64
  %t2112 = load ptr, ptr %t2083
  %t2114 = sext i32 63 to i64
  %t2113 = getelementptr ptr, ptr %t2112, i64 %t2114
  %t2115 = sext i32 0 to i64
  store i64 %t2115, ptr %t2113
  %t2116 = alloca i64
  %t2117 = call i32 @new_reg(ptr %t0)
  %t2118 = sext i32 %t2117 to i64
  store i64 %t2118, ptr %t2116
  %t2119 = getelementptr i8, ptr %t0, i64 0
  %t2120 = load i64, ptr %t2119
  %t2121 = getelementptr [44 x i8], ptr @.str199, i64 0, i64 0
  %t2122 = load i64, ptr %t2116
  %t2123 = load ptr, ptr %t2055
  %t2124 = load ptr, ptr %t2083
  %t2125 = load ptr, ptr %t2084
  call void (ptr, ...) @__c0c_emit(i64 %t2120, ptr %t2121, i64 %t2122, ptr %t2123, ptr %t2124, ptr %t2125)
  %t2127 = alloca i64
  %t2128 = call i32 @new_reg(ptr %t0)
  %t2129 = sext i32 %t2128 to i64
  store i64 %t2129, ptr %t2127
  %t2130 = getelementptr i8, ptr %t0, i64 0
  %t2131 = load i64, ptr %t2130
  %t2132 = getelementptr [30 x i8], ptr @.str200, i64 0, i64 0
  %t2133 = load i64, ptr %t2127
  %t2134 = load ptr, ptr %t2056
  %t2135 = load i64, ptr %t2116
  call void (ptr, ...) @__c0c_emit(i64 %t2131, ptr %t2132, i64 %t2133, ptr %t2134, i64 %t2135)
  %t2137 = alloca ptr
  %t2138 = load ptr, ptr %t2137
  %t2139 = getelementptr [6 x i8], ptr @.str201, i64 0, i64 0
  %t2140 = load i64, ptr %t2127
  %t2141 = call i32 (ptr, ...) @snprintf(ptr %t2138, i64 8, ptr %t2139, i64 %t2140)
  %t2142 = sext i32 %t2141 to i64
  %t2143 = load ptr, ptr %t2137
  %t2144 = load ptr, ptr %t2057
  %t2145 = call i64 @make_val(ptr %t2143, ptr %t2144)
  ret i64 %t2145
L487:
  br label %L22
L22:
  %t2146 = alloca i64
  %t2147 = getelementptr i8, ptr %t1, i64 0
  %t2148 = load i64, ptr %t2147
  %t2149 = call i64 @emit_expr(ptr %t0, i64 %t2148)
  store i64 %t2149, ptr %t2146
  %t2150 = alloca ptr
  %t2151 = getelementptr i8, ptr %t1, i64 0
  %t2152 = load i64, ptr %t2151
  store i64 %t2152, ptr %t2150
  %t2153 = load ptr, ptr %t2150
  %t2155 = ptrtoint ptr %t2153 to i64
  %t2156 = icmp eq i64 %t2155, 0
  %t2154 = zext i1 %t2156 to i64
  %t2157 = icmp ne i64 %t2154, 0
  br i1 %t2157, label %L488, label %L490
L488:
  %t2158 = load i64, ptr %t2146
  %t2159 = sext i32 %t2158 to i64
  ret i64 %t2159
L491:
  br label %L490
L490:
  %t2160 = alloca i64
  %t2161 = call i32 @new_reg(ptr %t0)
  %t2162 = sext i32 %t2161 to i64
  store i64 %t2162, ptr %t2160
  %t2163 = alloca i64
  %t2164 = getelementptr i8, ptr %t2146, i64 0
  %t2165 = load i64, ptr %t2164
  %t2166 = call i32 @type_is_fp(i64 %t2165)
  %t2167 = sext i32 %t2166 to i64
  store i64 %t2167, ptr %t2163
  %t2168 = alloca i64
  %t2169 = load ptr, ptr %t2150
  %t2170 = call i32 @type_is_fp(ptr %t2169)
  %t2171 = sext i32 %t2170 to i64
  store i64 %t2171, ptr %t2168
  %t2172 = alloca i64
  %t2173 = load ptr, ptr %t2150
  %t2174 = getelementptr i8, ptr %t2173, i64 0
  %t2175 = load i64, ptr %t2174
  %t2177 = sext i32 15 to i64
  %t2176 = icmp eq i64 %t2175, %t2177
  %t2178 = zext i1 %t2176 to i64
  %t2179 = icmp ne i64 %t2178, 0
  br i1 %t2179, label %L492, label %L493
L492:
  br label %L494
L493:
  %t2180 = load ptr, ptr %t2150
  %t2181 = getelementptr i8, ptr %t2180, i64 0
  %t2182 = load i64, ptr %t2181
  %t2184 = sext i32 16 to i64
  %t2183 = icmp eq i64 %t2182, %t2184
  %t2185 = zext i1 %t2183 to i64
  %t2186 = icmp ne i64 %t2185, 0
  %t2187 = zext i1 %t2186 to i64
  br label %L494
L494:
  %t2188 = phi i64 [ 1, %L492 ], [ %t2187, %L493 ]
  store i64 %t2188, ptr %t2172
  %t2189 = alloca i64
  %t2190 = load i64, ptr %t2146
  %t2191 = call i32 @val_is_ptr(i64 %t2190)
  %t2192 = sext i32 %t2191 to i64
  store i64 %t2192, ptr %t2189
  %t2193 = load i64, ptr %t2163
  %t2194 = sext i32 %t2193 to i64
  %t2195 = icmp ne i64 %t2194, 0
  br i1 %t2195, label %L495, label %L496
L495:
  %t2196 = load i64, ptr %t2168
  %t2197 = sext i32 %t2196 to i64
  %t2198 = icmp ne i64 %t2197, 0
  %t2199 = zext i1 %t2198 to i64
  br label %L497
L496:
  br label %L497
L497:
  %t2200 = phi i64 [ %t2199, %L495 ], [ 0, %L496 ]
  %t2201 = icmp ne i64 %t2200, 0
  br i1 %t2201, label %L498, label %L499
L498:
  %t2202 = alloca i64
  %t2203 = getelementptr i8, ptr %t2146, i64 0
  %t2204 = load i64, ptr %t2203
  %t2205 = call i32 @type_size(i64 %t2204)
  %t2206 = sext i32 %t2205 to i64
  store i64 %t2206, ptr %t2202
  %t2207 = alloca i64
  %t2208 = load ptr, ptr %t2150
  %t2209 = call i32 @type_size(ptr %t2208)
  %t2210 = sext i32 %t2209 to i64
  store i64 %t2210, ptr %t2207
  %t2211 = load i64, ptr %t2207
  %t2212 = load i64, ptr %t2202
  %t2214 = sext i32 %t2211 to i64
  %t2215 = sext i32 %t2212 to i64
  %t2213 = icmp sgt i64 %t2214, %t2215
  %t2216 = zext i1 %t2213 to i64
  %t2217 = icmp ne i64 %t2216, 0
  br i1 %t2217, label %L501, label %L502
L501:
  %t2218 = getelementptr i8, ptr %t0, i64 0
  %t2219 = load i64, ptr %t2218
  %t2220 = getelementptr [36 x i8], ptr @.str202, i64 0, i64 0
  %t2221 = load i64, ptr %t2160
  %t2222 = getelementptr i8, ptr %t2146, i64 0
  %t2223 = load i64, ptr %t2222
  call void (ptr, ...) @__c0c_emit(i64 %t2219, ptr %t2220, i64 %t2221, i64 %t2223)
  br label %L503
L502:
  %t2225 = getelementptr i8, ptr %t0, i64 0
  %t2226 = load i64, ptr %t2225
  %t2227 = getelementptr [38 x i8], ptr @.str203, i64 0, i64 0
  %t2228 = load i64, ptr %t2160
  %t2229 = getelementptr i8, ptr %t2146, i64 0
  %t2230 = load i64, ptr %t2229
  call void (ptr, ...) @__c0c_emit(i64 %t2226, ptr %t2227, i64 %t2228, i64 %t2230)
  br label %L503
L503:
  br label %L500
L499:
  %t2232 = load i64, ptr %t2163
  %t2233 = sext i32 %t2232 to i64
  %t2234 = icmp ne i64 %t2233, 0
  br i1 %t2234, label %L504, label %L505
L504:
  %t2235 = load i64, ptr %t2168
  %t2237 = sext i32 %t2235 to i64
  %t2238 = icmp eq i64 %t2237, 0
  %t2236 = zext i1 %t2238 to i64
  %t2239 = icmp ne i64 %t2236, 0
  %t2240 = zext i1 %t2239 to i64
  br label %L506
L505:
  br label %L506
L506:
  %t2241 = phi i64 [ %t2240, %L504 ], [ 0, %L505 ]
  %t2242 = icmp ne i64 %t2241, 0
  br i1 %t2242, label %L507, label %L508
L507:
  %t2243 = getelementptr i8, ptr %t0, i64 0
  %t2244 = load i64, ptr %t2243
  %t2245 = getelementptr [35 x i8], ptr @.str204, i64 0, i64 0
  %t2246 = load i64, ptr %t2160
  %t2247 = getelementptr i8, ptr %t2146, i64 0
  %t2248 = load i64, ptr %t2247
  call void (ptr, ...) @__c0c_emit(i64 %t2244, ptr %t2245, i64 %t2246, i64 %t2248)
  br label %L509
L508:
  %t2250 = load i64, ptr %t2163
  %t2252 = sext i32 %t2250 to i64
  %t2253 = icmp eq i64 %t2252, 0
  %t2251 = zext i1 %t2253 to i64
  %t2254 = icmp ne i64 %t2251, 0
  br i1 %t2254, label %L510, label %L511
L510:
  %t2255 = load i64, ptr %t2168
  %t2256 = sext i32 %t2255 to i64
  %t2257 = icmp ne i64 %t2256, 0
  %t2258 = zext i1 %t2257 to i64
  br label %L512
L511:
  br label %L512
L512:
  %t2259 = phi i64 [ %t2258, %L510 ], [ 0, %L511 ]
  %t2260 = icmp ne i64 %t2259, 0
  br i1 %t2260, label %L513, label %L514
L513:
  %t2261 = alloca ptr
  %t2262 = load i64, ptr %t2146
  %t2263 = load ptr, ptr %t2261
  %t2264 = call i32 @promote_to_i64(ptr %t0, i64 %t2262, ptr %t2263, i64 64)
  %t2265 = sext i32 %t2264 to i64
  %t2266 = getelementptr i8, ptr %t0, i64 0
  %t2267 = load i64, ptr %t2266
  %t2268 = getelementptr [31 x i8], ptr @.str205, i64 0, i64 0
  %t2269 = load i64, ptr %t2160
  %t2270 = load ptr, ptr %t2261
  %t2271 = load ptr, ptr %t2150
  %t2272 = call ptr @llvm_type(ptr %t2271)
  call void (ptr, ...) @__c0c_emit(i64 %t2267, ptr %t2268, i64 %t2269, ptr %t2270, ptr %t2272)
  br label %L515
L514:
  %t2274 = load i64, ptr %t2172
  %t2275 = sext i32 %t2274 to i64
  %t2276 = icmp ne i64 %t2275, 0
  br i1 %t2276, label %L516, label %L517
L516:
  %t2277 = load i64, ptr %t2189
  %t2279 = sext i32 %t2277 to i64
  %t2280 = icmp eq i64 %t2279, 0
  %t2278 = zext i1 %t2280 to i64
  %t2281 = icmp ne i64 %t2278, 0
  %t2282 = zext i1 %t2281 to i64
  br label %L518
L517:
  br label %L518
L518:
  %t2283 = phi i64 [ %t2282, %L516 ], [ 0, %L517 ]
  %t2284 = icmp ne i64 %t2283, 0
  br i1 %t2284, label %L519, label %L520
L519:
  %t2285 = alloca ptr
  %t2286 = load i64, ptr %t2146
  %t2287 = load ptr, ptr %t2285
  %t2288 = call i32 @promote_to_i64(ptr %t0, i64 %t2286, ptr %t2287, i64 64)
  %t2289 = sext i32 %t2288 to i64
  %t2290 = getelementptr i8, ptr %t0, i64 0
  %t2291 = load i64, ptr %t2290
  %t2292 = getelementptr [34 x i8], ptr @.str206, i64 0, i64 0
  %t2293 = load i64, ptr %t2160
  %t2294 = load ptr, ptr %t2285
  call void (ptr, ...) @__c0c_emit(i64 %t2291, ptr %t2292, i64 %t2293, ptr %t2294)
  br label %L521
L520:
  %t2296 = load i64, ptr %t2172
  %t2298 = sext i32 %t2296 to i64
  %t2299 = icmp eq i64 %t2298, 0
  %t2297 = zext i1 %t2299 to i64
  %t2300 = icmp ne i64 %t2297, 0
  br i1 %t2300, label %L522, label %L523
L522:
  %t2301 = load i64, ptr %t2189
  %t2302 = sext i32 %t2301 to i64
  %t2303 = icmp ne i64 %t2302, 0
  %t2304 = zext i1 %t2303 to i64
  br label %L524
L523:
  br label %L524
L524:
  %t2305 = phi i64 [ %t2304, %L522 ], [ 0, %L523 ]
  %t2306 = icmp ne i64 %t2305, 0
  br i1 %t2306, label %L525, label %L526
L525:
  %t2307 = getelementptr i8, ptr %t0, i64 0
  %t2308 = load i64, ptr %t2307
  %t2309 = getelementptr [34 x i8], ptr @.str207, i64 0, i64 0
  %t2310 = load i64, ptr %t2160
  %t2311 = getelementptr i8, ptr %t2146, i64 0
  %t2312 = load i64, ptr %t2311
  call void (ptr, ...) @__c0c_emit(i64 %t2308, ptr %t2309, i64 %t2310, i64 %t2312)
  br label %L527
L526:
  %t2314 = load i64, ptr %t2172
  %t2315 = sext i32 %t2314 to i64
  %t2316 = icmp ne i64 %t2315, 0
  br i1 %t2316, label %L528, label %L529
L528:
  %t2317 = load i64, ptr %t2189
  %t2318 = sext i32 %t2317 to i64
  %t2319 = icmp ne i64 %t2318, 0
  %t2320 = zext i1 %t2319 to i64
  br label %L530
L529:
  br label %L530
L530:
  %t2321 = phi i64 [ %t2320, %L528 ], [ 0, %L529 ]
  %t2322 = icmp ne i64 %t2321, 0
  br i1 %t2322, label %L531, label %L532
L531:
  %t2323 = getelementptr i8, ptr %t0, i64 0
  %t2324 = load i64, ptr %t2323
  %t2325 = getelementptr [33 x i8], ptr @.str208, i64 0, i64 0
  %t2326 = load i64, ptr %t2160
  %t2327 = getelementptr i8, ptr %t2146, i64 0
  %t2328 = load i64, ptr %t2327
  call void (ptr, ...) @__c0c_emit(i64 %t2324, ptr %t2325, i64 %t2326, i64 %t2328)
  br label %L533
L532:
  %t2330 = alloca ptr
  %t2331 = load i64, ptr %t2146
  %t2332 = load ptr, ptr %t2330
  %t2333 = call i32 @promote_to_i64(ptr %t0, i64 %t2331, ptr %t2332, i64 64)
  %t2334 = sext i32 %t2333 to i64
  %t2335 = getelementptr i8, ptr %t0, i64 0
  %t2336 = load i64, ptr %t2335
  %t2337 = getelementptr [25 x i8], ptr @.str209, i64 0, i64 0
  %t2338 = load i64, ptr %t2160
  %t2339 = load ptr, ptr %t2330
  call void (ptr, ...) @__c0c_emit(i64 %t2336, ptr %t2337, i64 %t2338, ptr %t2339)
  br label %L533
L533:
  br label %L527
L527:
  br label %L521
L521:
  br label %L515
L515:
  br label %L509
L509:
  br label %L500
L500:
  %t2341 = alloca ptr
  %t2342 = load ptr, ptr %t2341
  %t2343 = getelementptr [6 x i8], ptr @.str210, i64 0, i64 0
  %t2344 = load i64, ptr %t2160
  %t2345 = call i32 (ptr, ...) @snprintf(ptr %t2342, i64 8, ptr %t2343, i64 %t2344)
  %t2346 = sext i32 %t2345 to i64
  %t2347 = load i64, ptr %t2172
  %t2349 = sext i32 %t2347 to i64
  %t2348 = icmp ne i64 %t2349, 0
  br i1 %t2348, label %L534, label %L536
L534:
  %t2350 = load ptr, ptr %t2341
  %t2351 = call ptr @default_ptr_type()
  %t2352 = call i64 @make_val(ptr %t2350, ptr %t2351)
  ret i64 %t2352
L537:
  br label %L536
L536:
  %t2353 = load i64, ptr %t2168
  %t2355 = sext i32 %t2353 to i64
  %t2354 = icmp ne i64 %t2355, 0
  br i1 %t2354, label %L538, label %L540
L538:
  %t2356 = load ptr, ptr %t2341
  %t2357 = load ptr, ptr %t2150
  %t2358 = call i64 @make_val(ptr %t2356, ptr %t2357)
  ret i64 %t2358
L541:
  br label %L540
L540:
  %t2359 = load ptr, ptr %t2341
  %t2360 = call ptr @default_i64_type()
  %t2361 = call i64 @make_val(ptr %t2359, ptr %t2360)
  ret i64 %t2361
L542:
  br label %L23
L23:
  %t2362 = alloca i64
  %t2363 = getelementptr i8, ptr %t1, i64 0
  %t2364 = load i64, ptr %t2363
  %t2365 = call i64 @emit_expr(ptr %t0, i64 %t2364)
  store i64 %t2365, ptr %t2362
  %t2366 = alloca i64
  %t2367 = call i32 @new_label(ptr %t0)
  %t2368 = sext i32 %t2367 to i64
  store i64 %t2368, ptr %t2366
  %t2369 = alloca i64
  %t2370 = call i32 @new_label(ptr %t0)
  %t2371 = sext i32 %t2370 to i64
  store i64 %t2371, ptr %t2369
  %t2372 = alloca i64
  %t2373 = call i32 @new_label(ptr %t0)
  %t2374 = sext i32 %t2373 to i64
  store i64 %t2374, ptr %t2372
  %t2375 = alloca i64
  %t2376 = load i64, ptr %t2362
  %t2377 = call i32 @emit_cond(ptr %t0, i64 %t2376)
  %t2378 = sext i32 %t2377 to i64
  store i64 %t2378, ptr %t2375
  %t2379 = getelementptr i8, ptr %t0, i64 0
  %t2380 = load i64, ptr %t2379
  %t2381 = getelementptr [41 x i8], ptr @.str211, i64 0, i64 0
  %t2382 = load i64, ptr %t2375
  %t2383 = load i64, ptr %t2366
  %t2384 = load i64, ptr %t2369
  call void (ptr, ...) @__c0c_emit(i64 %t2380, ptr %t2381, i64 %t2382, i64 %t2383, i64 %t2384)
  %t2386 = getelementptr i8, ptr %t0, i64 0
  %t2387 = load i64, ptr %t2386
  %t2388 = getelementptr [6 x i8], ptr @.str212, i64 0, i64 0
  %t2389 = load i64, ptr %t2366
  call void (ptr, ...) @__c0c_emit(i64 %t2387, ptr %t2388, i64 %t2389)
  %t2391 = alloca i64
  %t2392 = getelementptr i8, ptr %t1, i64 0
  %t2393 = load i64, ptr %t2392
  %t2394 = inttoptr i64 %t2393 to ptr
  %t2395 = sext i32 0 to i64
  %t2396 = getelementptr ptr, ptr %t2394, i64 %t2395
  %t2397 = load ptr, ptr %t2396
  %t2398 = call i64 @emit_expr(ptr %t0, ptr %t2397)
  store i64 %t2398, ptr %t2391
  %t2399 = alloca ptr
  %t2400 = load i64, ptr %t2391
  %t2401 = load ptr, ptr %t2399
  %t2402 = call i32 @promote_to_i64(ptr %t0, i64 %t2400, ptr %t2401, i64 64)
  %t2403 = sext i32 %t2402 to i64
  %t2404 = getelementptr i8, ptr %t0, i64 0
  %t2405 = load i64, ptr %t2404
  %t2406 = getelementptr [18 x i8], ptr @.str213, i64 0, i64 0
  %t2407 = load i64, ptr %t2372
  call void (ptr, ...) @__c0c_emit(i64 %t2405, ptr %t2406, i64 %t2407)
  %t2409 = getelementptr i8, ptr %t0, i64 0
  %t2410 = load i64, ptr %t2409
  %t2411 = getelementptr [6 x i8], ptr @.str214, i64 0, i64 0
  %t2412 = load i64, ptr %t2369
  call void (ptr, ...) @__c0c_emit(i64 %t2410, ptr %t2411, i64 %t2412)
  %t2414 = alloca i64
  %t2415 = getelementptr i8, ptr %t1, i64 0
  %t2416 = load i64, ptr %t2415
  %t2417 = inttoptr i64 %t2416 to ptr
  %t2418 = sext i32 1 to i64
  %t2419 = getelementptr ptr, ptr %t2417, i64 %t2418
  %t2420 = load ptr, ptr %t2419
  %t2421 = call i64 @emit_expr(ptr %t0, ptr %t2420)
  store i64 %t2421, ptr %t2414
  %t2422 = alloca ptr
  %t2423 = load i64, ptr %t2414
  %t2424 = load ptr, ptr %t2422
  %t2425 = call i32 @promote_to_i64(ptr %t0, i64 %t2423, ptr %t2424, i64 64)
  %t2426 = sext i32 %t2425 to i64
  %t2427 = getelementptr i8, ptr %t0, i64 0
  %t2428 = load i64, ptr %t2427
  %t2429 = getelementptr [18 x i8], ptr @.str215, i64 0, i64 0
  %t2430 = load i64, ptr %t2372
  call void (ptr, ...) @__c0c_emit(i64 %t2428, ptr %t2429, i64 %t2430)
  %t2432 = getelementptr i8, ptr %t0, i64 0
  %t2433 = load i64, ptr %t2432
  %t2434 = getelementptr [6 x i8], ptr @.str216, i64 0, i64 0
  %t2435 = load i64, ptr %t2372
  call void (ptr, ...) @__c0c_emit(i64 %t2433, ptr %t2434, i64 %t2435)
  %t2437 = alloca i64
  %t2438 = call i32 @new_reg(ptr %t0)
  %t2439 = sext i32 %t2438 to i64
  store i64 %t2439, ptr %t2437
  %t2440 = getelementptr i8, ptr %t0, i64 0
  %t2441 = load i64, ptr %t2440
  %t2442 = getelementptr [48 x i8], ptr @.str217, i64 0, i64 0
  %t2443 = load i64, ptr %t2437
  %t2444 = load ptr, ptr %t2399
  %t2445 = load i64, ptr %t2366
  %t2446 = load ptr, ptr %t2422
  %t2447 = load i64, ptr %t2369
  call void (ptr, ...) @__c0c_emit(i64 %t2441, ptr %t2442, i64 %t2443, ptr %t2444, i64 %t2445, ptr %t2446, i64 %t2447)
  %t2449 = alloca ptr
  %t2450 = load ptr, ptr %t2449
  %t2451 = getelementptr [6 x i8], ptr @.str218, i64 0, i64 0
  %t2452 = load i64, ptr %t2437
  %t2453 = call i32 (ptr, ...) @snprintf(ptr %t2450, i64 8, ptr %t2451, i64 %t2452)
  %t2454 = sext i32 %t2453 to i64
  %t2455 = load ptr, ptr %t2449
  %t2456 = call ptr @default_i64_type()
  %t2457 = call i64 @make_val(ptr %t2455, ptr %t2456)
  ret i64 %t2457
L543:
  br label %L24
L24:
  %t2458 = alloca i64
  %t2459 = getelementptr i8, ptr %t1, i64 0
  %t2460 = load i64, ptr %t2459
  %t2461 = icmp ne i64 %t2460, 0
  br i1 %t2461, label %L544, label %L545
L544:
  %t2462 = getelementptr i8, ptr %t1, i64 0
  %t2463 = load i64, ptr %t2462
  %t2464 = call i32 @type_size(i64 %t2463)
  %t2465 = sext i32 %t2464 to i64
  br label %L546
L545:
  %t2466 = sext i32 0 to i64
  br label %L546
L546:
  %t2467 = phi i64 [ %t2465, %L544 ], [ %t2466, %L545 ]
  store i64 %t2467, ptr %t2458
  %t2468 = alloca ptr
  %t2469 = load ptr, ptr %t2468
  %t2470 = getelementptr [3 x i8], ptr @.str219, i64 0, i64 0
  %t2471 = load i64, ptr %t2458
  %t2472 = call i32 (ptr, ...) @snprintf(ptr %t2469, i64 8, ptr %t2470, i64 %t2471)
  %t2473 = sext i32 %t2472 to i64
  %t2474 = load ptr, ptr %t2468
  %t2475 = call ptr @default_int_type()
  %t2476 = call i64 @make_val(ptr %t2474, ptr %t2475)
  ret i64 %t2476
L547:
  br label %L25
L25:
  %t2477 = alloca i64
  %t2478 = getelementptr i8, ptr %t1, i64 0
  %t2479 = load i64, ptr %t2478
  %t2480 = inttoptr i64 %t2479 to ptr
  %t2481 = sext i32 0 to i64
  %t2482 = getelementptr ptr, ptr %t2480, i64 %t2481
  %t2483 = load ptr, ptr %t2482
  %t2484 = getelementptr i8, ptr %t2483, i64 0
  %t2485 = load i64, ptr %t2484
  %t2486 = icmp ne i64 %t2485, 0
  br i1 %t2486, label %L548, label %L549
L548:
  %t2487 = getelementptr i8, ptr %t1, i64 0
  %t2488 = load i64, ptr %t2487
  %t2489 = inttoptr i64 %t2488 to ptr
  %t2490 = sext i32 0 to i64
  %t2491 = getelementptr ptr, ptr %t2489, i64 %t2490
  %t2492 = load ptr, ptr %t2491
  %t2493 = getelementptr i8, ptr %t2492, i64 0
  %t2494 = load i64, ptr %t2493
  %t2495 = call i32 @type_size(i64 %t2494)
  %t2496 = sext i32 %t2495 to i64
  br label %L550
L549:
  %t2497 = sext i32 8 to i64
  br label %L550
L550:
  %t2498 = phi i64 [ %t2496, %L548 ], [ %t2497, %L549 ]
  store i64 %t2498, ptr %t2477
  %t2499 = alloca ptr
  %t2500 = load ptr, ptr %t2499
  %t2501 = getelementptr [3 x i8], ptr @.str220, i64 0, i64 0
  %t2502 = load i64, ptr %t2477
  %t2503 = call i32 (ptr, ...) @snprintf(ptr %t2500, i64 8, ptr %t2501, i64 %t2502)
  %t2504 = sext i32 %t2503 to i64
  %t2505 = load ptr, ptr %t2499
  %t2506 = call ptr @default_int_type()
  %t2507 = call i64 @make_val(ptr %t2505, ptr %t2506)
  ret i64 %t2507
L551:
  br label %L26
L26:
  %t2508 = alloca i64
  %t2509 = getelementptr [2 x i8], ptr @.str221, i64 0, i64 0
  %t2510 = call ptr @default_int_type()
  %t2511 = call i64 @make_val(ptr %t2509, ptr %t2510)
  store i64 %t2511, ptr %t2508
  %t2512 = alloca i64
  %t2513 = sext i32 0 to i64
  store i64 %t2513, ptr %t2512
  br label %L552
L552:
  %t2514 = load i64, ptr %t2512
  %t2515 = getelementptr i8, ptr %t1, i64 0
  %t2516 = load i64, ptr %t2515
  %t2518 = sext i32 %t2514 to i64
  %t2517 = icmp slt i64 %t2518, %t2516
  %t2519 = zext i1 %t2517 to i64
  %t2520 = icmp ne i64 %t2519, 0
  br i1 %t2520, label %L553, label %L555
L553:
  %t2521 = getelementptr i8, ptr %t1, i64 0
  %t2522 = load i64, ptr %t2521
  %t2523 = load i64, ptr %t2512
  %t2524 = inttoptr i64 %t2522 to ptr
  %t2525 = sext i32 %t2523 to i64
  %t2526 = getelementptr ptr, ptr %t2524, i64 %t2525
  %t2527 = load ptr, ptr %t2526
  %t2528 = call i64 @emit_expr(ptr %t0, ptr %t2527)
  store i64 %t2528, ptr %t2508
  br label %L554
L554:
  %t2529 = load i64, ptr %t2512
  %t2531 = sext i32 %t2529 to i64
  %t2530 = add i64 %t2531, 1
  store i64 %t2530, ptr %t2512
  br label %L552
L555:
  %t2532 = load i64, ptr %t2508
  %t2533 = sext i32 %t2532 to i64
  ret i64 %t2533
L556:
  br label %L27
L27:
  br label %L28
L28:
  %t2534 = alloca i64
  %t2535 = getelementptr i8, ptr %t1, i64 0
  %t2536 = load i64, ptr %t2535
  %t2538 = sext i32 35 to i64
  %t2537 = icmp eq i64 %t2536, %t2538
  %t2539 = zext i1 %t2537 to i64
  %t2540 = icmp ne i64 %t2539, 0
  br i1 %t2540, label %L557, label %L558
L557:
  %t2541 = getelementptr i8, ptr %t1, i64 0
  %t2542 = load i64, ptr %t2541
  %t2543 = inttoptr i64 %t2542 to ptr
  %t2544 = sext i32 0 to i64
  %t2545 = getelementptr ptr, ptr %t2543, i64 %t2544
  %t2546 = load ptr, ptr %t2545
  %t2547 = call i64 @emit_expr(ptr %t0, ptr %t2546)
  store i64 %t2547, ptr %t2534
  br label %L559
L558:
  %t2548 = alloca ptr
  %t2549 = getelementptr i8, ptr %t1, i64 0
  %t2550 = load i64, ptr %t2549
  %t2551 = inttoptr i64 %t2550 to ptr
  %t2552 = sext i32 0 to i64
  %t2553 = getelementptr ptr, ptr %t2551, i64 %t2552
  %t2554 = load ptr, ptr %t2553
  %t2555 = call ptr @emit_lvalue_addr(ptr %t0, ptr %t2554)
  store ptr %t2555, ptr %t2548
  %t2556 = load ptr, ptr %t2548
  %t2557 = icmp ne ptr %t2556, null
  br i1 %t2557, label %L560, label %L561
L560:
  %t2558 = load ptr, ptr %t2548
  %t2559 = call ptr @default_ptr_type()
  %t2560 = call i64 @make_val(ptr %t2558, ptr %t2559)
  store i64 %t2560, ptr %t2534
  %t2561 = load ptr, ptr %t2548
  call void @free(ptr %t2561)
  br label %L562
L561:
  %t2563 = getelementptr i8, ptr %t1, i64 0
  %t2564 = load i64, ptr %t2563
  %t2565 = inttoptr i64 %t2564 to ptr
  %t2566 = sext i32 0 to i64
  %t2567 = getelementptr ptr, ptr %t2565, i64 %t2566
  %t2568 = load ptr, ptr %t2567
  %t2569 = call i64 @emit_expr(ptr %t0, ptr %t2568)
  store i64 %t2569, ptr %t2534
  br label %L562
L562:
  br label %L559
L559:
  %t2570 = alloca ptr
  %t2571 = load i64, ptr %t2534
  %t2572 = call i32 @val_is_ptr(i64 %t2571)
  %t2573 = sext i32 %t2572 to i64
  %t2574 = icmp ne i64 %t2573, 0
  br i1 %t2574, label %L563, label %L564
L563:
  %t2575 = load ptr, ptr %t2570
  %t2576 = getelementptr i8, ptr %t2534, i64 0
  %t2577 = load i64, ptr %t2576
  %t2578 = call ptr @strncpy(ptr %t2575, i64 %t2577, i64 63)
  %t2579 = load ptr, ptr %t2570
  %t2581 = sext i32 63 to i64
  %t2580 = getelementptr ptr, ptr %t2579, i64 %t2581
  %t2582 = sext i32 0 to i64
  store i64 %t2582, ptr %t2580
  br label %L565
L564:
  %t2583 = alloca i64
  %t2584 = call i32 @new_reg(ptr %t0)
  %t2585 = sext i32 %t2584 to i64
  store i64 %t2585, ptr %t2583
  %t2586 = alloca ptr
  %t2587 = load i64, ptr %t2534
  %t2588 = load ptr, ptr %t2586
  %t2589 = call i32 @promote_to_i64(ptr %t0, i64 %t2587, ptr %t2588, i64 64)
  %t2590 = sext i32 %t2589 to i64
  %t2591 = getelementptr i8, ptr %t0, i64 0
  %t2592 = load i64, ptr %t2591
  %t2593 = getelementptr [34 x i8], ptr @.str222, i64 0, i64 0
  %t2594 = load i64, ptr %t2583
  %t2595 = load ptr, ptr %t2586
  call void (ptr, ...) @__c0c_emit(i64 %t2592, ptr %t2593, i64 %t2594, ptr %t2595)
  %t2597 = load ptr, ptr %t2570
  %t2598 = getelementptr [6 x i8], ptr @.str223, i64 0, i64 0
  %t2599 = load i64, ptr %t2583
  %t2600 = call i32 (ptr, ...) @snprintf(ptr %t2597, i64 64, ptr %t2598, i64 %t2599)
  %t2601 = sext i32 %t2600 to i64
  br label %L565
L565:
  %t2602 = alloca ptr
  %t2603 = getelementptr i8, ptr %t1, i64 0
  %t2604 = load i64, ptr %t2603
  store i64 %t2604, ptr %t2602
  %t2605 = alloca ptr
  %t2606 = getelementptr i8, ptr %t1, i64 0
  %t2607 = load i64, ptr %t2606
  %t2608 = inttoptr i64 %t2607 to ptr
  %t2609 = sext i32 0 to i64
  %t2610 = getelementptr ptr, ptr %t2608, i64 %t2609
  %t2611 = load ptr, ptr %t2610
  %t2612 = icmp ne ptr %t2611, null
  br i1 %t2612, label %L566, label %L567
L566:
  %t2613 = getelementptr i8, ptr %t1, i64 0
  %t2614 = load i64, ptr %t2613
  %t2615 = inttoptr i64 %t2614 to ptr
  %t2616 = sext i32 0 to i64
  %t2617 = getelementptr ptr, ptr %t2615, i64 %t2616
  %t2618 = load ptr, ptr %t2617
  %t2619 = getelementptr i8, ptr %t2618, i64 0
  %t2620 = load i64, ptr %t2619
  br label %L568
L567:
  %t2622 = sext i32 0 to i64
  %t2621 = inttoptr i64 %t2622 to ptr
  %t2623 = ptrtoint ptr %t2621 to i64
  br label %L568
L568:
  %t2624 = phi i64 [ %t2620, %L566 ], [ %t2623, %L567 ]
  store i64 %t2624, ptr %t2605
  %t2625 = alloca i64
  %t2626 = sext i32 0 to i64
  store i64 %t2626, ptr %t2625
  %t2627 = load ptr, ptr %t2605
  %t2628 = ptrtoint ptr %t2627 to i64
  %t2629 = icmp ne i64 %t2628, 0
  br i1 %t2629, label %L569, label %L570
L569:
  %t2630 = load ptr, ptr %t2605
  %t2631 = getelementptr i8, ptr %t2630, i64 0
  %t2632 = load i64, ptr %t2631
  %t2634 = sext i32 18 to i64
  %t2633 = icmp eq i64 %t2632, %t2634
  %t2635 = zext i1 %t2633 to i64
  %t2636 = icmp ne i64 %t2635, 0
  br i1 %t2636, label %L572, label %L573
L572:
  br label %L574
L573:
  %t2637 = load ptr, ptr %t2605
  %t2638 = getelementptr i8, ptr %t2637, i64 0
  %t2639 = load i64, ptr %t2638
  %t2641 = sext i32 19 to i64
  %t2640 = icmp eq i64 %t2639, %t2641
  %t2642 = zext i1 %t2640 to i64
  %t2643 = icmp ne i64 %t2642, 0
  %t2644 = zext i1 %t2643 to i64
  br label %L574
L574:
  %t2645 = phi i64 [ 1, %L572 ], [ %t2644, %L573 ]
  %t2646 = icmp ne i64 %t2645, 0
  %t2647 = zext i1 %t2646 to i64
  br label %L571
L570:
  br label %L571
L571:
  %t2648 = phi i64 [ %t2647, %L569 ], [ 0, %L570 ]
  %t2649 = icmp ne i64 %t2648, 0
  br i1 %t2649, label %L575, label %L576
L575:
  %t2650 = load ptr, ptr %t2605
  %t2651 = getelementptr i8, ptr %t2650, i64 0
  %t2652 = load i64, ptr %t2651
  %t2653 = icmp ne i64 %t2652, 0
  %t2654 = zext i1 %t2653 to i64
  br label %L577
L576:
  br label %L577
L577:
  %t2655 = phi i64 [ %t2654, %L575 ], [ 0, %L576 ]
  %t2656 = icmp ne i64 %t2655, 0
  br i1 %t2656, label %L578, label %L580
L578:
  %t2657 = alloca i64
  %t2658 = sext i32 0 to i64
  store i64 %t2658, ptr %t2657
  br label %L581
L581:
  %t2659 = load i64, ptr %t2657
  %t2660 = load ptr, ptr %t2605
  %t2661 = getelementptr i8, ptr %t2660, i64 0
  %t2662 = load i64, ptr %t2661
  %t2664 = sext i32 %t2659 to i64
  %t2663 = icmp slt i64 %t2664, %t2662
  %t2665 = zext i1 %t2663 to i64
  %t2666 = icmp ne i64 %t2665, 0
  br i1 %t2666, label %L582, label %L584
L582:
  %t2667 = load ptr, ptr %t2605
  %t2668 = getelementptr i8, ptr %t2667, i64 0
  %t2669 = load i64, ptr %t2668
  %t2670 = load i64, ptr %t2657
  %t2672 = inttoptr i64 %t2669 to ptr
  %t2673 = sext i32 %t2670 to i64
  %t2671 = getelementptr ptr, ptr %t2672, i64 %t2673
  %t2674 = getelementptr i8, ptr %t2671, i64 0
  %t2675 = load i64, ptr %t2674
  %t2676 = icmp ne i64 %t2675, 0
  br i1 %t2676, label %L585, label %L586
L585:
  %t2677 = load ptr, ptr %t2605
  %t2678 = getelementptr i8, ptr %t2677, i64 0
  %t2679 = load i64, ptr %t2678
  %t2680 = load i64, ptr %t2657
  %t2682 = inttoptr i64 %t2679 to ptr
  %t2683 = sext i32 %t2680 to i64
  %t2681 = getelementptr ptr, ptr %t2682, i64 %t2683
  %t2684 = getelementptr i8, ptr %t2681, i64 0
  %t2685 = load i64, ptr %t2684
  %t2686 = load ptr, ptr %t2602
  %t2687 = call i32 @strcmp(i64 %t2685, ptr %t2686)
  %t2688 = sext i32 %t2687 to i64
  %t2690 = sext i32 0 to i64
  %t2689 = icmp eq i64 %t2688, %t2690
  %t2691 = zext i1 %t2689 to i64
  %t2692 = icmp ne i64 %t2691, 0
  %t2693 = zext i1 %t2692 to i64
  br label %L587
L586:
  br label %L587
L587:
  %t2694 = phi i64 [ %t2693, %L585 ], [ 0, %L586 ]
  %t2695 = icmp ne i64 %t2694, 0
  br i1 %t2695, label %L588, label %L590
L588:
  br label %L584
L591:
  br label %L590
L590:
  %t2696 = load ptr, ptr %t2605
  %t2697 = getelementptr i8, ptr %t2696, i64 0
  %t2698 = load i64, ptr %t2697
  %t2699 = icmp ne i64 %t2698, 0
  br i1 %t2699, label %L592, label %L593
L592:
  %t2700 = load ptr, ptr %t2605
  %t2701 = getelementptr i8, ptr %t2700, i64 0
  %t2702 = load i64, ptr %t2701
  %t2703 = load i64, ptr %t2657
  %t2704 = inttoptr i64 %t2702 to ptr
  %t2705 = sext i32 %t2703 to i64
  %t2706 = getelementptr ptr, ptr %t2704, i64 %t2705
  %t2707 = load ptr, ptr %t2706
  %t2708 = ptrtoint ptr %t2707 to i64
  %t2709 = icmp ne i64 %t2708, 0
  %t2710 = zext i1 %t2709 to i64
  br label %L594
L593:
  br label %L594
L594:
  %t2711 = phi i64 [ %t2710, %L592 ], [ 0, %L593 ]
  %t2712 = icmp ne i64 %t2711, 0
  br i1 %t2712, label %L595, label %L597
L595:
  %t2713 = load i64, ptr %t2625
  %t2714 = load ptr, ptr %t2605
  %t2715 = getelementptr i8, ptr %t2714, i64 0
  %t2716 = load i64, ptr %t2715
  %t2717 = load i64, ptr %t2657
  %t2718 = inttoptr i64 %t2716 to ptr
  %t2719 = sext i32 %t2717 to i64
  %t2720 = getelementptr ptr, ptr %t2718, i64 %t2719
  %t2721 = load ptr, ptr %t2720
  %t2722 = call i32 @type_size(ptr %t2721)
  %t2723 = sext i32 %t2722 to i64
  %t2724 = add i64 %t2713, %t2723
  store i64 %t2724, ptr %t2625
  br label %L597
L597:
  br label %L583
L583:
  %t2725 = load i64, ptr %t2657
  %t2727 = sext i32 %t2725 to i64
  %t2726 = add i64 %t2727, 1
  store i64 %t2726, ptr %t2657
  br label %L581
L584:
  br label %L580
L580:
  %t2728 = alloca i64
  %t2729 = call i32 @new_reg(ptr %t0)
  %t2730 = sext i32 %t2729 to i64
  store i64 %t2730, ptr %t2728
  %t2731 = load i64, ptr %t2625
  %t2733 = sext i32 0 to i64
  %t2732 = icmp sgt i64 %t2731, %t2733
  %t2734 = zext i1 %t2732 to i64
  %t2735 = icmp ne i64 %t2734, 0
  br i1 %t2735, label %L598, label %L599
L598:
  %t2736 = getelementptr i8, ptr %t0, i64 0
  %t2737 = load i64, ptr %t2736
  %t2738 = getelementptr [45 x i8], ptr @.str224, i64 0, i64 0
  %t2739 = load i64, ptr %t2728
  %t2740 = load ptr, ptr %t2570
  %t2741 = load i64, ptr %t2625
  call void (ptr, ...) @__c0c_emit(i64 %t2737, ptr %t2738, i64 %t2739, ptr %t2740, i64 %t2741)
  br label %L600
L599:
  %t2743 = getelementptr i8, ptr %t0, i64 0
  %t2744 = load i64, ptr %t2743
  %t2745 = getelementptr [43 x i8], ptr @.str225, i64 0, i64 0
  %t2746 = load i64, ptr %t2728
  %t2747 = load ptr, ptr %t2570
  call void (ptr, ...) @__c0c_emit(i64 %t2744, ptr %t2745, i64 %t2746, ptr %t2747)
  br label %L600
L600:
  %t2749 = alloca i64
  %t2750 = call i32 @new_reg(ptr %t0)
  %t2751 = sext i32 %t2750 to i64
  store i64 %t2751, ptr %t2749
  %t2752 = getelementptr i8, ptr %t0, i64 0
  %t2753 = load i64, ptr %t2752
  %t2754 = getelementptr [31 x i8], ptr @.str226, i64 0, i64 0
  %t2755 = load i64, ptr %t2749
  %t2756 = load i64, ptr %t2728
  call void (ptr, ...) @__c0c_emit(i64 %t2753, ptr %t2754, i64 %t2755, i64 %t2756)
  %t2758 = alloca ptr
  %t2759 = load ptr, ptr %t2758
  %t2760 = getelementptr [6 x i8], ptr @.str227, i64 0, i64 0
  %t2761 = load i64, ptr %t2749
  %t2762 = call i32 (ptr, ...) @snprintf(ptr %t2759, i64 8, ptr %t2760, i64 %t2761)
  %t2763 = sext i32 %t2762 to i64
  %t2764 = load ptr, ptr %t2758
  %t2765 = call ptr @default_i64_type()
  %t2766 = call i64 @make_val(ptr %t2764, ptr %t2765)
  ret i64 %t2766
L601:
  br label %L4
L29:
  %t2767 = getelementptr i8, ptr %t0, i64 0
  %t2768 = load i64, ptr %t2767
  %t2769 = getelementptr [28 x i8], ptr @.str228, i64 0, i64 0
  %t2770 = getelementptr i8, ptr %t1, i64 0
  %t2771 = load i64, ptr %t2770
  call void (ptr, ...) @__c0c_emit(i64 %t2768, ptr %t2769, i64 %t2771)
  %t2773 = getelementptr [2 x i8], ptr @.str229, i64 0, i64 0
  %t2774 = call ptr @default_int_type()
  %t2775 = call i64 @make_val(ptr %t2773, ptr %t2774)
  ret i64 %t2775
L602:
  br label %L4
L4:
  ret i64 0
}

define internal void @emit_stmt(ptr %t0, ptr %t1) {
entry:
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
  %t6 = getelementptr i8, ptr %t1, i64 0
  %t7 = load i64, ptr %t6
  %t8 = add i64 %t7, 0
  switch i64 %t8, label %L21 [
    i64 5, label %L5
    i64 2, label %L6
    i64 18, label %L7
    i64 10, label %L8
    i64 6, label %L9
    i64 7, label %L10
    i64 8, label %L11
    i64 9, label %L12
    i64 11, label %L13
    i64 12, label %L14
    i64 13, label %L15
    i64 14, label %L16
    i64 15, label %L17
    i64 16, label %L18
    i64 17, label %L19
    i64 3, label %L20
  ]
L5:
  %t9 = getelementptr i8, ptr %t1, i64 0
  %t10 = load i64, ptr %t9
  %t11 = icmp ne i64 %t10, 0
  br i1 %t11, label %L22, label %L24
L22:
  call void @scope_push(ptr %t0)
  br label %L24
L24:
  %t13 = alloca i64
  %t14 = sext i32 0 to i64
  store i64 %t14, ptr %t13
  br label %L25
L25:
  %t15 = load i64, ptr %t13
  %t16 = getelementptr i8, ptr %t1, i64 0
  %t17 = load i64, ptr %t16
  %t19 = sext i32 %t15 to i64
  %t18 = icmp slt i64 %t19, %t17
  %t20 = zext i1 %t18 to i64
  %t21 = icmp ne i64 %t20, 0
  br i1 %t21, label %L26, label %L28
L26:
  %t22 = getelementptr i8, ptr %t1, i64 0
  %t23 = load i64, ptr %t22
  %t24 = load i64, ptr %t13
  %t25 = inttoptr i64 %t23 to ptr
  %t26 = sext i32 %t24 to i64
  %t27 = getelementptr ptr, ptr %t25, i64 %t26
  %t28 = load ptr, ptr %t27
  call void @emit_stmt(ptr %t0, ptr %t28)
  br label %L27
L27:
  %t30 = load i64, ptr %t13
  %t32 = sext i32 %t30 to i64
  %t31 = add i64 %t32, 1
  store i64 %t31, ptr %t13
  br label %L25
L28:
  %t33 = getelementptr i8, ptr %t1, i64 0
  %t34 = load i64, ptr %t33
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L29, label %L31
L29:
  call void @scope_pop(ptr %t0)
  br label %L31
L31:
  br label %L4
L32:
  br label %L6
L6:
  %t37 = alloca ptr
  %t38 = getelementptr i8, ptr %t1, i64 0
  %t39 = load i64, ptr %t38
  %t40 = icmp ne i64 %t39, 0
  br i1 %t40, label %L33, label %L34
L33:
  %t41 = getelementptr i8, ptr %t1, i64 0
  %t42 = load i64, ptr %t41
  br label %L35
L34:
  %t43 = call ptr @default_int_type()
  %t44 = ptrtoint ptr %t43 to i64
  br label %L35
L35:
  %t45 = phi i64 [ %t42, %L33 ], [ %t44, %L34 ]
  store i64 %t45, ptr %t37
  %t46 = alloca ptr
  %t47 = alloca ptr
  %t48 = load ptr, ptr %t37
  %t49 = getelementptr i8, ptr %t48, i64 0
  %t50 = load i64, ptr %t49
  %t52 = sext i32 16 to i64
  %t51 = icmp eq i64 %t50, %t52
  %t53 = zext i1 %t51 to i64
  %t54 = icmp ne i64 %t53, 0
  br i1 %t54, label %L36, label %L37
L36:
  %t55 = load ptr, ptr %t37
  %t56 = getelementptr i8, ptr %t55, i64 0
  %t57 = load i64, ptr %t56
  %t58 = icmp ne i64 %t57, 0
  %t59 = zext i1 %t58 to i64
  br label %L38
L37:
  br label %L38
L38:
  %t60 = phi i64 [ %t59, %L36 ], [ 0, %L37 ]
  %t61 = icmp ne i64 %t60, 0
  br i1 %t61, label %L39, label %L40
L39:
  %t62 = load ptr, ptr %t37
  %t63 = getelementptr i8, ptr %t62, i64 0
  %t64 = load i64, ptr %t63
  %t65 = inttoptr i64 %t64 to ptr
  %t66 = getelementptr i8, ptr %t65, i64 0
  %t67 = load i64, ptr %t66
  %t69 = sext i32 18 to i64
  %t68 = icmp eq i64 %t67, %t69
  %t70 = zext i1 %t68 to i64
  %t71 = icmp ne i64 %t70, 0
  br i1 %t71, label %L42, label %L43
L42:
  br label %L44
L43:
  %t72 = load ptr, ptr %t37
  %t73 = getelementptr i8, ptr %t72, i64 0
  %t74 = load i64, ptr %t73
  %t75 = inttoptr i64 %t74 to ptr
  %t76 = getelementptr i8, ptr %t75, i64 0
  %t77 = load i64, ptr %t76
  %t79 = sext i32 19 to i64
  %t78 = icmp eq i64 %t77, %t79
  %t80 = zext i1 %t78 to i64
  %t81 = icmp ne i64 %t80, 0
  %t82 = zext i1 %t81 to i64
  br label %L44
L44:
  %t83 = phi i64 [ 1, %L42 ], [ %t82, %L43 ]
  %t84 = icmp ne i64 %t83, 0
  %t85 = zext i1 %t84 to i64
  br label %L41
L40:
  br label %L41
L41:
  %t86 = phi i64 [ %t85, %L39 ], [ 0, %L40 ]
  %t87 = icmp ne i64 %t86, 0
  br i1 %t87, label %L45, label %L46
L45:
  %t88 = alloca i64
  %t89 = load ptr, ptr %t37
  %t90 = getelementptr i8, ptr %t89, i64 0
  %t91 = load i64, ptr %t90
  %t92 = call i32 @type_size(i64 %t91)
  %t93 = sext i32 %t92 to i64
  store i64 %t93, ptr %t88
  %t94 = alloca i64
  %t95 = load ptr, ptr %t37
  %t96 = getelementptr i8, ptr %t95, i64 0
  %t97 = load i64, ptr %t96
  %t98 = add i64 %t97, 0
  %t99 = load i64, ptr %t88
  %t101 = sext i32 %t99 to i64
  %t100 = mul i64 %t98, %t101
  store i64 %t100, ptr %t94
  %t102 = load i64, ptr %t94
  %t104 = sext i32 %t102 to i64
  %t105 = sext i32 0 to i64
  %t103 = icmp sle i64 %t104, %t105
  %t106 = zext i1 %t103 to i64
  %t107 = icmp ne i64 %t106, 0
  br i1 %t107, label %L48, label %L50
L48:
  %t108 = sext i32 8 to i64
  store i64 %t108, ptr %t94
  br label %L50
L50:
  %t109 = alloca ptr
  %t110 = load ptr, ptr %t109
  %t111 = getelementptr [10 x i8], ptr @.str230, i64 0, i64 0
  %t112 = load i64, ptr %t94
  %t113 = call i32 (ptr, ...) @snprintf(ptr %t110, i64 8, ptr %t111, i64 %t112)
  %t114 = sext i32 %t113 to i64
  %t115 = load ptr, ptr %t109
  store ptr %t115, ptr %t46
  %t116 = call ptr @default_ptr_type()
  store ptr %t116, ptr %t47
  br label %L47
L46:
  %t117 = load ptr, ptr %t37
  %t118 = getelementptr i8, ptr %t117, i64 0
  %t119 = load i64, ptr %t118
  %t121 = sext i32 18 to i64
  %t120 = icmp eq i64 %t119, %t121
  %t122 = zext i1 %t120 to i64
  %t123 = icmp ne i64 %t122, 0
  br i1 %t123, label %L51, label %L52
L51:
  br label %L53
L52:
  %t124 = load ptr, ptr %t37
  %t125 = getelementptr i8, ptr %t124, i64 0
  %t126 = load i64, ptr %t125
  %t128 = sext i32 19 to i64
  %t127 = icmp eq i64 %t126, %t128
  %t129 = zext i1 %t127 to i64
  %t130 = icmp ne i64 %t129, 0
  %t131 = zext i1 %t130 to i64
  br label %L53
L53:
  %t132 = phi i64 [ 1, %L51 ], [ %t131, %L52 ]
  %t133 = icmp ne i64 %t132, 0
  br i1 %t133, label %L54, label %L55
L54:
  %t134 = alloca i64
  %t135 = load ptr, ptr %t37
  %t136 = call i32 @type_size(ptr %t135)
  %t137 = sext i32 %t136 to i64
  store i64 %t137, ptr %t134
  %t138 = load i64, ptr %t134
  %t140 = sext i32 %t138 to i64
  %t141 = sext i32 0 to i64
  %t139 = icmp sle i64 %t140, %t141
  %t142 = zext i1 %t139 to i64
  %t143 = icmp ne i64 %t142, 0
  br i1 %t143, label %L57, label %L59
L57:
  %t144 = sext i32 8 to i64
  store i64 %t144, ptr %t134
  br label %L59
L59:
  %t145 = alloca ptr
  %t146 = load ptr, ptr %t145
  %t147 = getelementptr [10 x i8], ptr @.str231, i64 0, i64 0
  %t148 = load i64, ptr %t134
  %t149 = call i32 (ptr, ...) @snprintf(ptr %t146, i64 8, ptr %t147, i64 %t148)
  %t150 = sext i32 %t149 to i64
  %t151 = load ptr, ptr %t145
  store ptr %t151, ptr %t46
  %t152 = load ptr, ptr %t37
  store ptr %t152, ptr %t47
  br label %L56
L55:
  %t153 = load ptr, ptr %t37
  %t154 = getelementptr i8, ptr %t153, i64 0
  %t155 = load i64, ptr %t154
  %t157 = sext i32 15 to i64
  %t156 = icmp eq i64 %t155, %t157
  %t158 = zext i1 %t156 to i64
  %t159 = icmp ne i64 %t158, 0
  br i1 %t159, label %L60, label %L61
L60:
  br label %L62
L61:
  %t160 = load ptr, ptr %t37
  %t161 = getelementptr i8, ptr %t160, i64 0
  %t162 = load i64, ptr %t161
  %t164 = sext i32 16 to i64
  %t163 = icmp eq i64 %t162, %t164
  %t165 = zext i1 %t163 to i64
  %t166 = icmp ne i64 %t165, 0
  %t167 = zext i1 %t166 to i64
  br label %L62
L62:
  %t168 = phi i64 [ 1, %L60 ], [ %t167, %L61 ]
  %t169 = icmp ne i64 %t168, 0
  br i1 %t169, label %L63, label %L64
L63:
  %t170 = getelementptr [4 x i8], ptr @.str232, i64 0, i64 0
  store ptr %t170, ptr %t46
  %t171 = call ptr @default_ptr_type()
  store ptr %t171, ptr %t47
  br label %L65
L64:
  %t172 = load ptr, ptr %t37
  %t173 = call i32 @type_is_fp(ptr %t172)
  %t174 = sext i32 %t173 to i64
  %t175 = icmp ne i64 %t174, 0
  br i1 %t175, label %L66, label %L67
L66:
  %t176 = getelementptr [7 x i8], ptr @.str233, i64 0, i64 0
  store ptr %t176, ptr %t46
  %t177 = call ptr @default_fp_type()
  store ptr %t177, ptr %t47
  br label %L68
L67:
  %t178 = getelementptr [4 x i8], ptr @.str234, i64 0, i64 0
  store ptr %t178, ptr %t46
  %t179 = load ptr, ptr %t37
  store ptr %t179, ptr %t47
  br label %L68
L68:
  br label %L65
L65:
  br label %L56
L56:
  br label %L47
L47:
  %t180 = alloca i64
  %t181 = call i32 @new_reg(ptr %t0)
  %t182 = sext i32 %t181 to i64
  store i64 %t182, ptr %t180
  %t183 = getelementptr i8, ptr %t0, i64 0
  %t184 = load i64, ptr %t183
  %t185 = getelementptr [21 x i8], ptr @.str235, i64 0, i64 0
  %t186 = load i64, ptr %t180
  %t187 = load ptr, ptr %t46
  call void (ptr, ...) @__c0c_emit(i64 %t184, ptr %t185, i64 %t186, ptr %t187)
  %t189 = getelementptr i8, ptr %t0, i64 0
  %t190 = load i64, ptr %t189
  %t192 = sext i32 2048 to i64
  %t191 = icmp sge i64 %t190, %t192
  %t193 = zext i1 %t191 to i64
  %t194 = icmp ne i64 %t193, 0
  br i1 %t194, label %L69, label %L71
L69:
  %t195 = call ptr @__c0c_stderr()
  %t196 = getelementptr [22 x i8], ptr @.str236, i64 0, i64 0
  %t197 = call i32 (ptr, ...) @fprintf(ptr %t195, ptr %t196)
  %t198 = sext i32 %t197 to i64
  call void @exit(i64 1)
  br label %L71
L71:
  %t200 = alloca ptr
  %t201 = getelementptr i8, ptr %t0, i64 0
  %t202 = load i64, ptr %t201
  %t203 = getelementptr i8, ptr %t0, i64 0
  %t204 = load i64, ptr %t203
  %t205 = add i64 %t204, 1
  %t206 = getelementptr i8, ptr %t0, i64 0
  store i64 %t205, ptr %t206
  %t208 = inttoptr i64 %t202 to ptr
  %t207 = getelementptr ptr, ptr %t208, i64 %t204
  store ptr %t207, ptr %t200
  %t209 = getelementptr i8, ptr %t1, i64 0
  %t210 = load i64, ptr %t209
  %t211 = icmp ne i64 %t210, 0
  br i1 %t211, label %L72, label %L73
L72:
  %t212 = getelementptr i8, ptr %t1, i64 0
  %t213 = load i64, ptr %t212
  br label %L74
L73:
  %t214 = getelementptr [7 x i8], ptr @.str237, i64 0, i64 0
  %t215 = ptrtoint ptr %t214 to i64
  br label %L74
L74:
  %t216 = phi i64 [ %t213, %L72 ], [ %t215, %L73 ]
  %t217 = call ptr @strdup(i64 %t216)
  %t218 = load ptr, ptr %t200
  %t219 = getelementptr i8, ptr %t218, i64 0
  store ptr %t217, ptr %t219
  %t220 = call ptr @malloc(i64 32)
  %t221 = load ptr, ptr %t200
  %t222 = getelementptr i8, ptr %t221, i64 0
  store ptr %t220, ptr %t222
  %t223 = load ptr, ptr %t200
  %t224 = getelementptr i8, ptr %t223, i64 0
  %t225 = load i64, ptr %t224
  %t226 = getelementptr [6 x i8], ptr @.str238, i64 0, i64 0
  %t227 = load i64, ptr %t180
  %t228 = call i32 (ptr, ...) @snprintf(i64 %t225, i64 32, ptr %t226, i64 %t227)
  %t229 = sext i32 %t228 to i64
  %t230 = load ptr, ptr %t47
  %t231 = load ptr, ptr %t200
  %t232 = getelementptr i8, ptr %t231, i64 0
  store ptr %t230, ptr %t232
  %t233 = load ptr, ptr %t200
  %t234 = getelementptr i8, ptr %t233, i64 0
  %t235 = sext i32 0 to i64
  store i64 %t235, ptr %t234
  %t236 = getelementptr i8, ptr %t1, i64 0
  %t237 = load i64, ptr %t236
  %t239 = sext i32 0 to i64
  %t238 = icmp sgt i64 %t237, %t239
  %t240 = zext i1 %t238 to i64
  %t241 = icmp ne i64 %t240, 0
  br i1 %t241, label %L75, label %L77
L75:
  %t242 = alloca i64
  %t243 = getelementptr i8, ptr %t1, i64 0
  %t244 = load i64, ptr %t243
  %t245 = inttoptr i64 %t244 to ptr
  %t246 = sext i32 0 to i64
  %t247 = getelementptr ptr, ptr %t245, i64 %t246
  %t248 = load ptr, ptr %t247
  %t249 = call i64 @emit_expr(ptr %t0, ptr %t248)
  store i64 %t249, ptr %t242
  %t250 = alloca ptr
  %t251 = load i64, ptr %t242
  %t252 = call i32 @val_is_ptr(i64 %t251)
  %t253 = sext i32 %t252 to i64
  %t254 = icmp ne i64 %t253, 0
  br i1 %t254, label %L78, label %L79
L78:
  %t255 = getelementptr [4 x i8], ptr @.str239, i64 0, i64 0
  store ptr %t255, ptr %t250
  br label %L80
L79:
  %t256 = getelementptr i8, ptr %t242, i64 0
  %t257 = load i64, ptr %t256
  %t258 = call i32 @type_is_fp(i64 %t257)
  %t259 = sext i32 %t258 to i64
  %t260 = icmp ne i64 %t259, 0
  br i1 %t260, label %L81, label %L82
L81:
  %t261 = getelementptr i8, ptr %t242, i64 0
  %t262 = load i64, ptr %t261
  %t263 = call ptr @llvm_type(i64 %t262)
  store ptr %t263, ptr %t250
  br label %L83
L82:
  %t264 = getelementptr [4 x i8], ptr @.str240, i64 0, i64 0
  store ptr %t264, ptr %t250
  br label %L83
L83:
  br label %L80
L80:
  %t265 = alloca ptr
  %t266 = load i64, ptr %t242
  %t267 = call i32 @val_is_ptr(i64 %t266)
  %t268 = sext i32 %t267 to i64
  %t270 = icmp eq i64 %t268, 0
  %t269 = zext i1 %t270 to i64
  %t271 = icmp ne i64 %t269, 0
  br i1 %t271, label %L84, label %L85
L84:
  %t272 = load i64, ptr %t242
  %t273 = call i32 @val_is_64bit(i64 %t272)
  %t274 = sext i32 %t273 to i64
  %t276 = icmp eq i64 %t274, 0
  %t275 = zext i1 %t276 to i64
  %t277 = icmp ne i64 %t275, 0
  %t278 = zext i1 %t277 to i64
  br label %L86
L85:
  br label %L86
L86:
  %t279 = phi i64 [ %t278, %L84 ], [ 0, %L85 ]
  %t280 = icmp ne i64 %t279, 0
  br i1 %t280, label %L87, label %L88
L87:
  %t281 = getelementptr i8, ptr %t242, i64 0
  %t282 = load i64, ptr %t281
  %t283 = call i32 @type_is_fp(i64 %t282)
  %t284 = sext i32 %t283 to i64
  %t286 = icmp eq i64 %t284, 0
  %t285 = zext i1 %t286 to i64
  %t287 = icmp ne i64 %t285, 0
  %t288 = zext i1 %t287 to i64
  br label %L89
L88:
  br label %L89
L89:
  %t289 = phi i64 [ %t288, %L87 ], [ 0, %L88 ]
  %t290 = icmp ne i64 %t289, 0
  br i1 %t290, label %L90, label %L91
L90:
  %t291 = alloca i64
  %t292 = call i32 @new_reg(ptr %t0)
  %t293 = sext i32 %t292 to i64
  store i64 %t293, ptr %t291
  %t294 = getelementptr i8, ptr %t0, i64 0
  %t295 = load i64, ptr %t294
  %t296 = getelementptr [30 x i8], ptr @.str241, i64 0, i64 0
  %t297 = load i64, ptr %t291
  %t298 = getelementptr i8, ptr %t242, i64 0
  %t299 = load i64, ptr %t298
  call void (ptr, ...) @__c0c_emit(i64 %t295, ptr %t296, i64 %t297, i64 %t299)
  %t301 = load ptr, ptr %t265
  %t302 = getelementptr [6 x i8], ptr @.str242, i64 0, i64 0
  %t303 = load i64, ptr %t291
  %t304 = call i32 (ptr, ...) @snprintf(ptr %t301, i64 64, ptr %t302, i64 %t303)
  %t305 = sext i32 %t304 to i64
  br label %L92
L91:
  %t306 = load ptr, ptr %t265
  %t307 = getelementptr i8, ptr %t242, i64 0
  %t308 = load i64, ptr %t307
  %t309 = call ptr @strncpy(ptr %t306, i64 %t308, i64 63)
  %t310 = load ptr, ptr %t265
  %t312 = sext i32 63 to i64
  %t311 = getelementptr ptr, ptr %t310, i64 %t312
  %t313 = sext i32 0 to i64
  store i64 %t313, ptr %t311
  br label %L92
L92:
  %t314 = getelementptr i8, ptr %t0, i64 0
  %t315 = load i64, ptr %t314
  %t316 = getelementptr [26 x i8], ptr @.str243, i64 0, i64 0
  %t317 = load ptr, ptr %t250
  %t318 = load ptr, ptr %t265
  %t319 = load i64, ptr %t180
  call void (ptr, ...) @__c0c_emit(i64 %t315, ptr %t316, ptr %t317, ptr %t318, i64 %t319)
  br label %L77
L77:
  %t321 = alloca i64
  %t322 = sext i32 1 to i64
  store i64 %t322, ptr %t321
  br label %L93
L93:
  %t323 = load i64, ptr %t321
  %t324 = getelementptr i8, ptr %t1, i64 0
  %t325 = load i64, ptr %t324
  %t327 = sext i32 %t323 to i64
  %t326 = icmp slt i64 %t327, %t325
  %t328 = zext i1 %t326 to i64
  %t329 = icmp ne i64 %t328, 0
  br i1 %t329, label %L94, label %L96
L94:
  %t330 = getelementptr i8, ptr %t1, i64 0
  %t331 = load i64, ptr %t330
  %t332 = load i64, ptr %t321
  %t333 = inttoptr i64 %t331 to ptr
  %t334 = sext i32 %t332 to i64
  %t335 = getelementptr ptr, ptr %t333, i64 %t334
  %t336 = load ptr, ptr %t335
  call void @emit_stmt(ptr %t0, ptr %t336)
  br label %L95
L95:
  %t338 = load i64, ptr %t321
  %t340 = sext i32 %t338 to i64
  %t339 = add i64 %t340, 1
  store i64 %t339, ptr %t321
  br label %L93
L96:
  br label %L4
L97:
  br label %L7
L7:
  %t341 = getelementptr i8, ptr %t1, i64 0
  %t342 = load i64, ptr %t341
  %t344 = sext i32 0 to i64
  %t343 = icmp sgt i64 %t342, %t344
  %t345 = zext i1 %t343 to i64
  %t346 = icmp ne i64 %t345, 0
  br i1 %t346, label %L98, label %L100
L98:
  %t347 = getelementptr i8, ptr %t1, i64 0
  %t348 = load i64, ptr %t347
  %t349 = inttoptr i64 %t348 to ptr
  %t350 = sext i32 0 to i64
  %t351 = getelementptr ptr, ptr %t349, i64 %t350
  %t352 = load ptr, ptr %t351
  %t353 = call i64 @emit_expr(ptr %t0, ptr %t352)
  br label %L100
L100:
  br label %L4
L101:
  br label %L8
L8:
  %t354 = getelementptr i8, ptr %t1, i64 0
  %t355 = load i64, ptr %t354
  %t356 = icmp ne i64 %t355, 0
  br i1 %t356, label %L102, label %L103
L102:
  %t357 = alloca i64
  %t358 = getelementptr i8, ptr %t1, i64 0
  %t359 = load i64, ptr %t358
  %t360 = call i64 @emit_expr(ptr %t0, i64 %t359)
  store i64 %t360, ptr %t357
  %t361 = alloca ptr
  %t362 = getelementptr i8, ptr %t0, i64 0
  %t363 = load i64, ptr %t362
  store i64 %t363, ptr %t361
  %t364 = alloca i64
  %t365 = load ptr, ptr %t361
  %t366 = getelementptr i8, ptr %t365, i64 0
  %t367 = load i64, ptr %t366
  %t369 = sext i32 15 to i64
  %t368 = icmp eq i64 %t367, %t369
  %t370 = zext i1 %t368 to i64
  %t371 = icmp ne i64 %t370, 0
  br i1 %t371, label %L105, label %L106
L105:
  br label %L107
L106:
  %t372 = load ptr, ptr %t361
  %t373 = getelementptr i8, ptr %t372, i64 0
  %t374 = load i64, ptr %t373
  %t376 = sext i32 16 to i64
  %t375 = icmp eq i64 %t374, %t376
  %t377 = zext i1 %t375 to i64
  %t378 = icmp ne i64 %t377, 0
  %t379 = zext i1 %t378 to i64
  br label %L107
L107:
  %t380 = phi i64 [ 1, %L105 ], [ %t379, %L106 ]
  store i64 %t380, ptr %t364
  %t381 = alloca i64
  %t382 = load ptr, ptr %t361
  %t383 = getelementptr i8, ptr %t382, i64 0
  %t384 = load i64, ptr %t383
  %t386 = sext i32 0 to i64
  %t385 = icmp eq i64 %t384, %t386
  %t387 = zext i1 %t385 to i64
  store i64 %t387, ptr %t381
  %t388 = alloca i64
  %t389 = load ptr, ptr %t361
  %t390 = call i32 @type_is_fp(ptr %t389)
  %t391 = sext i32 %t390 to i64
  store i64 %t391, ptr %t388
  %t392 = alloca ptr
  %t393 = load ptr, ptr %t361
  %t394 = call ptr @llvm_type(ptr %t393)
  store ptr %t394, ptr %t392
  %t395 = load i64, ptr %t381
  %t397 = sext i32 %t395 to i64
  %t396 = icmp ne i64 %t397, 0
  br i1 %t396, label %L108, label %L109
L108:
  %t398 = getelementptr i8, ptr %t0, i64 0
  %t399 = load i64, ptr %t398
  %t400 = getelementptr [12 x i8], ptr @.str244, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t399, ptr %t400)
  br label %L110
L109:
  %t402 = load i64, ptr %t388
  %t404 = sext i32 %t402 to i64
  %t403 = icmp ne i64 %t404, 0
  br i1 %t403, label %L111, label %L112
L111:
  %t405 = getelementptr i8, ptr %t0, i64 0
  %t406 = load i64, ptr %t405
  %t407 = getelementptr [13 x i8], ptr @.str245, i64 0, i64 0
  %t408 = load ptr, ptr %t392
  %t409 = getelementptr i8, ptr %t357, i64 0
  %t410 = load i64, ptr %t409
  call void (ptr, ...) @__c0c_emit(i64 %t406, ptr %t407, ptr %t408, i64 %t410)
  br label %L113
L112:
  %t412 = load i64, ptr %t364
  %t414 = sext i32 %t412 to i64
  %t413 = icmp ne i64 %t414, 0
  br i1 %t413, label %L114, label %L115
L114:
  %t415 = load i64, ptr %t357
  %t416 = call i32 @val_is_ptr(i64 %t415)
  %t417 = sext i32 %t416 to i64
  %t418 = icmp ne i64 %t417, 0
  br i1 %t418, label %L117, label %L118
L117:
  %t419 = getelementptr i8, ptr %t0, i64 0
  %t420 = load i64, ptr %t419
  %t421 = getelementptr [14 x i8], ptr @.str246, i64 0, i64 0
  %t422 = getelementptr i8, ptr %t357, i64 0
  %t423 = load i64, ptr %t422
  call void (ptr, ...) @__c0c_emit(i64 %t420, ptr %t421, i64 %t423)
  br label %L119
L118:
  %t425 = alloca i64
  %t426 = call i32 @new_reg(ptr %t0)
  %t427 = sext i32 %t426 to i64
  store i64 %t427, ptr %t425
  %t428 = alloca ptr
  %t429 = load i64, ptr %t357
  %t430 = load ptr, ptr %t428
  %t431 = call i32 @promote_to_i64(ptr %t0, i64 %t429, ptr %t430, i64 64)
  %t432 = sext i32 %t431 to i64
  %t433 = getelementptr i8, ptr %t0, i64 0
  %t434 = load i64, ptr %t433
  %t435 = getelementptr [34 x i8], ptr @.str247, i64 0, i64 0
  %t436 = load i64, ptr %t425
  %t437 = load ptr, ptr %t428
  call void (ptr, ...) @__c0c_emit(i64 %t434, ptr %t435, i64 %t436, ptr %t437)
  %t439 = getelementptr i8, ptr %t0, i64 0
  %t440 = load i64, ptr %t439
  %t441 = getelementptr [17 x i8], ptr @.str248, i64 0, i64 0
  %t442 = load i64, ptr %t425
  call void (ptr, ...) @__c0c_emit(i64 %t440, ptr %t441, i64 %t442)
  br label %L119
L119:
  br label %L116
L115:
  %t444 = alloca ptr
  %t445 = load i64, ptr %t357
  %t446 = load ptr, ptr %t444
  %t447 = call i32 @promote_to_i64(ptr %t0, i64 %t445, ptr %t446, i64 64)
  %t448 = sext i32 %t447 to i64
  %t449 = load ptr, ptr %t392
  %t450 = getelementptr [3 x i8], ptr @.str249, i64 0, i64 0
  %t451 = call i32 @strcmp(ptr %t449, ptr %t450)
  %t452 = sext i32 %t451 to i64
  %t454 = sext i32 0 to i64
  %t453 = icmp eq i64 %t452, %t454
  %t455 = zext i1 %t453 to i64
  %t456 = icmp ne i64 %t455, 0
  br i1 %t456, label %L120, label %L121
L120:
  %t457 = alloca i64
  %t458 = call i32 @new_reg(ptr %t0)
  %t459 = sext i32 %t458 to i64
  store i64 %t459, ptr %t457
  %t460 = getelementptr i8, ptr %t0, i64 0
  %t461 = load i64, ptr %t460
  %t462 = getelementptr [30 x i8], ptr @.str250, i64 0, i64 0
  %t463 = load i64, ptr %t457
  %t464 = load ptr, ptr %t444
  call void (ptr, ...) @__c0c_emit(i64 %t461, ptr %t462, i64 %t463, ptr %t464)
  %t466 = getelementptr i8, ptr %t0, i64 0
  %t467 = load i64, ptr %t466
  %t468 = getelementptr [16 x i8], ptr @.str251, i64 0, i64 0
  %t469 = load i64, ptr %t457
  call void (ptr, ...) @__c0c_emit(i64 %t467, ptr %t468, i64 %t469)
  br label %L122
L121:
  %t471 = load ptr, ptr %t392
  %t472 = getelementptr [4 x i8], ptr @.str252, i64 0, i64 0
  %t473 = call i32 @strcmp(ptr %t471, ptr %t472)
  %t474 = sext i32 %t473 to i64
  %t476 = sext i32 0 to i64
  %t475 = icmp eq i64 %t474, %t476
  %t477 = zext i1 %t475 to i64
  %t478 = icmp ne i64 %t477, 0
  br i1 %t478, label %L123, label %L124
L123:
  %t479 = alloca i64
  %t480 = call i32 @new_reg(ptr %t0)
  %t481 = sext i32 %t480 to i64
  store i64 %t481, ptr %t479
  %t482 = getelementptr i8, ptr %t0, i64 0
  %t483 = load i64, ptr %t482
  %t484 = getelementptr [31 x i8], ptr @.str253, i64 0, i64 0
  %t485 = load i64, ptr %t479
  %t486 = load ptr, ptr %t444
  call void (ptr, ...) @__c0c_emit(i64 %t483, ptr %t484, i64 %t485, ptr %t486)
  %t488 = getelementptr i8, ptr %t0, i64 0
  %t489 = load i64, ptr %t488
  %t490 = getelementptr [17 x i8], ptr @.str254, i64 0, i64 0
  %t491 = load i64, ptr %t479
  call void (ptr, ...) @__c0c_emit(i64 %t489, ptr %t490, i64 %t491)
  br label %L125
L124:
  %t493 = load ptr, ptr %t392
  %t494 = getelementptr [4 x i8], ptr @.str255, i64 0, i64 0
  %t495 = call i32 @strcmp(ptr %t493, ptr %t494)
  %t496 = sext i32 %t495 to i64
  %t498 = sext i32 0 to i64
  %t497 = icmp eq i64 %t496, %t498
  %t499 = zext i1 %t497 to i64
  %t500 = icmp ne i64 %t499, 0
  br i1 %t500, label %L126, label %L127
L126:
  %t501 = alloca i64
  %t502 = call i32 @new_reg(ptr %t0)
  %t503 = sext i32 %t502 to i64
  store i64 %t503, ptr %t501
  %t504 = getelementptr i8, ptr %t0, i64 0
  %t505 = load i64, ptr %t504
  %t506 = getelementptr [31 x i8], ptr @.str256, i64 0, i64 0
  %t507 = load i64, ptr %t501
  %t508 = load ptr, ptr %t444
  call void (ptr, ...) @__c0c_emit(i64 %t505, ptr %t506, i64 %t507, ptr %t508)
  %t510 = getelementptr i8, ptr %t0, i64 0
  %t511 = load i64, ptr %t510
  %t512 = getelementptr [17 x i8], ptr @.str257, i64 0, i64 0
  %t513 = load i64, ptr %t501
  call void (ptr, ...) @__c0c_emit(i64 %t511, ptr %t512, i64 %t513)
  br label %L128
L127:
  %t515 = getelementptr i8, ptr %t0, i64 0
  %t516 = load i64, ptr %t515
  %t517 = getelementptr [14 x i8], ptr @.str258, i64 0, i64 0
  %t518 = load ptr, ptr %t444
  call void (ptr, ...) @__c0c_emit(i64 %t516, ptr %t517, ptr %t518)
  br label %L128
L128:
  br label %L125
L125:
  br label %L122
L122:
  br label %L116
L116:
  br label %L113
L113:
  br label %L110
L110:
  br label %L104
L103:
  %t520 = getelementptr i8, ptr %t0, i64 0
  %t521 = load i64, ptr %t520
  %t522 = getelementptr [12 x i8], ptr @.str259, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t521, ptr %t522)
  br label %L104
L104:
  %t524 = alloca i64
  %t525 = call i32 @new_label(ptr %t0)
  %t526 = sext i32 %t525 to i64
  store i64 %t526, ptr %t524
  %t527 = getelementptr i8, ptr %t0, i64 0
  %t528 = load i64, ptr %t527
  %t529 = getelementptr [6 x i8], ptr @.str260, i64 0, i64 0
  %t530 = load i64, ptr %t524
  call void (ptr, ...) @__c0c_emit(i64 %t528, ptr %t529, i64 %t530)
  br label %L4
L129:
  br label %L9
L9:
  %t532 = alloca i64
  %t533 = getelementptr i8, ptr %t1, i64 0
  %t534 = load i64, ptr %t533
  %t535 = call i64 @emit_expr(ptr %t0, i64 %t534)
  store i64 %t535, ptr %t532
  %t536 = alloca i64
  %t537 = load i64, ptr %t532
  %t538 = call i32 @emit_cond(ptr %t0, i64 %t537)
  %t539 = sext i32 %t538 to i64
  store i64 %t539, ptr %t536
  %t540 = alloca i64
  %t541 = call i32 @new_label(ptr %t0)
  %t542 = sext i32 %t541 to i64
  store i64 %t542, ptr %t540
  %t543 = alloca i64
  %t544 = call i32 @new_label(ptr %t0)
  %t545 = sext i32 %t544 to i64
  store i64 %t545, ptr %t543
  %t546 = alloca i64
  %t547 = call i32 @new_label(ptr %t0)
  %t548 = sext i32 %t547 to i64
  store i64 %t548, ptr %t546
  %t549 = getelementptr i8, ptr %t0, i64 0
  %t550 = load i64, ptr %t549
  %t551 = getelementptr [41 x i8], ptr @.str261, i64 0, i64 0
  %t552 = load i64, ptr %t536
  %t553 = load i64, ptr %t540
  %t554 = getelementptr i8, ptr %t1, i64 0
  %t555 = load i64, ptr %t554
  %t556 = icmp ne i64 %t555, 0
  br i1 %t556, label %L130, label %L131
L130:
  %t557 = load i64, ptr %t543
  %t558 = sext i32 %t557 to i64
  br label %L132
L131:
  %t559 = load i64, ptr %t546
  %t560 = sext i32 %t559 to i64
  br label %L132
L132:
  %t561 = phi i64 [ %t558, %L130 ], [ %t560, %L131 ]
  call void (ptr, ...) @__c0c_emit(i64 %t550, ptr %t551, i64 %t552, i64 %t553, i64 %t561)
  %t563 = getelementptr i8, ptr %t0, i64 0
  %t564 = load i64, ptr %t563
  %t565 = getelementptr [6 x i8], ptr @.str262, i64 0, i64 0
  %t566 = load i64, ptr %t540
  call void (ptr, ...) @__c0c_emit(i64 %t564, ptr %t565, i64 %t566)
  %t568 = getelementptr i8, ptr %t1, i64 0
  %t569 = load i64, ptr %t568
  call void @emit_stmt(ptr %t0, i64 %t569)
  %t571 = getelementptr i8, ptr %t0, i64 0
  %t572 = load i64, ptr %t571
  %t573 = getelementptr [18 x i8], ptr @.str263, i64 0, i64 0
  %t574 = load i64, ptr %t546
  call void (ptr, ...) @__c0c_emit(i64 %t572, ptr %t573, i64 %t574)
  %t576 = getelementptr i8, ptr %t1, i64 0
  %t577 = load i64, ptr %t576
  %t578 = icmp ne i64 %t577, 0
  br i1 %t578, label %L133, label %L135
L133:
  %t579 = getelementptr i8, ptr %t0, i64 0
  %t580 = load i64, ptr %t579
  %t581 = getelementptr [6 x i8], ptr @.str264, i64 0, i64 0
  %t582 = load i64, ptr %t543
  call void (ptr, ...) @__c0c_emit(i64 %t580, ptr %t581, i64 %t582)
  %t584 = getelementptr i8, ptr %t1, i64 0
  %t585 = load i64, ptr %t584
  call void @emit_stmt(ptr %t0, i64 %t585)
  %t587 = getelementptr i8, ptr %t0, i64 0
  %t588 = load i64, ptr %t587
  %t589 = getelementptr [18 x i8], ptr @.str265, i64 0, i64 0
  %t590 = load i64, ptr %t546
  call void (ptr, ...) @__c0c_emit(i64 %t588, ptr %t589, i64 %t590)
  br label %L135
L135:
  %t592 = getelementptr i8, ptr %t0, i64 0
  %t593 = load i64, ptr %t592
  %t594 = getelementptr [6 x i8], ptr @.str266, i64 0, i64 0
  %t595 = load i64, ptr %t546
  call void (ptr, ...) @__c0c_emit(i64 %t593, ptr %t594, i64 %t595)
  br label %L4
L136:
  br label %L10
L10:
  %t597 = alloca i64
  %t598 = call i32 @new_label(ptr %t0)
  %t599 = sext i32 %t598 to i64
  store i64 %t599, ptr %t597
  %t600 = alloca i64
  %t601 = call i32 @new_label(ptr %t0)
  %t602 = sext i32 %t601 to i64
  store i64 %t602, ptr %t600
  %t603 = alloca i64
  %t604 = call i32 @new_label(ptr %t0)
  %t605 = sext i32 %t604 to i64
  store i64 %t605, ptr %t603
  %t606 = alloca ptr
  %t607 = alloca ptr
  %t608 = load ptr, ptr %t606
  %t609 = getelementptr i8, ptr %t0, i64 0
  %t610 = load i64, ptr %t609
  %t611 = call ptr @strcpy(ptr %t608, i64 %t610)
  %t612 = load ptr, ptr %t607
  %t613 = getelementptr i8, ptr %t0, i64 0
  %t614 = load i64, ptr %t613
  %t615 = call ptr @strcpy(ptr %t612, i64 %t614)
  %t616 = getelementptr i8, ptr %t0, i64 0
  %t617 = load i64, ptr %t616
  %t618 = getelementptr [4 x i8], ptr @.str267, i64 0, i64 0
  %t619 = load i64, ptr %t603
  %t620 = call i32 (ptr, ...) @snprintf(i64 %t617, i64 64, ptr %t618, i64 %t619)
  %t621 = sext i32 %t620 to i64
  %t622 = getelementptr i8, ptr %t0, i64 0
  %t623 = load i64, ptr %t622
  %t624 = getelementptr [4 x i8], ptr @.str268, i64 0, i64 0
  %t625 = load i64, ptr %t597
  %t626 = call i32 (ptr, ...) @snprintf(i64 %t623, i64 64, ptr %t624, i64 %t625)
  %t627 = sext i32 %t626 to i64
  %t628 = getelementptr i8, ptr %t0, i64 0
  %t629 = load i64, ptr %t628
  %t630 = getelementptr [18 x i8], ptr @.str269, i64 0, i64 0
  %t631 = load i64, ptr %t597
  call void (ptr, ...) @__c0c_emit(i64 %t629, ptr %t630, i64 %t631)
  %t633 = getelementptr i8, ptr %t0, i64 0
  %t634 = load i64, ptr %t633
  %t635 = getelementptr [6 x i8], ptr @.str270, i64 0, i64 0
  %t636 = load i64, ptr %t597
  call void (ptr, ...) @__c0c_emit(i64 %t634, ptr %t635, i64 %t636)
  %t638 = alloca i64
  %t639 = getelementptr i8, ptr %t1, i64 0
  %t640 = load i64, ptr %t639
  %t641 = call i64 @emit_expr(ptr %t0, i64 %t640)
  store i64 %t641, ptr %t638
  %t642 = alloca i64
  %t643 = load i64, ptr %t638
  %t644 = call i32 @emit_cond(ptr %t0, i64 %t643)
  %t645 = sext i32 %t644 to i64
  store i64 %t645, ptr %t642
  %t646 = getelementptr i8, ptr %t0, i64 0
  %t647 = load i64, ptr %t646
  %t648 = getelementptr [41 x i8], ptr @.str271, i64 0, i64 0
  %t649 = load i64, ptr %t642
  %t650 = load i64, ptr %t600
  %t651 = load i64, ptr %t603
  call void (ptr, ...) @__c0c_emit(i64 %t647, ptr %t648, i64 %t649, i64 %t650, i64 %t651)
  %t653 = getelementptr i8, ptr %t0, i64 0
  %t654 = load i64, ptr %t653
  %t655 = getelementptr [6 x i8], ptr @.str272, i64 0, i64 0
  %t656 = load i64, ptr %t600
  call void (ptr, ...) @__c0c_emit(i64 %t654, ptr %t655, i64 %t656)
  %t658 = getelementptr i8, ptr %t1, i64 0
  %t659 = load i64, ptr %t658
  call void @emit_stmt(ptr %t0, i64 %t659)
  %t661 = getelementptr i8, ptr %t0, i64 0
  %t662 = load i64, ptr %t661
  %t663 = getelementptr [18 x i8], ptr @.str273, i64 0, i64 0
  %t664 = load i64, ptr %t597
  call void (ptr, ...) @__c0c_emit(i64 %t662, ptr %t663, i64 %t664)
  %t666 = getelementptr i8, ptr %t0, i64 0
  %t667 = load i64, ptr %t666
  %t668 = getelementptr [6 x i8], ptr @.str274, i64 0, i64 0
  %t669 = load i64, ptr %t603
  call void (ptr, ...) @__c0c_emit(i64 %t667, ptr %t668, i64 %t669)
  %t671 = getelementptr i8, ptr %t0, i64 0
  %t672 = load i64, ptr %t671
  %t673 = load ptr, ptr %t606
  %t674 = call ptr @strcpy(i64 %t672, ptr %t673)
  %t675 = getelementptr i8, ptr %t0, i64 0
  %t676 = load i64, ptr %t675
  %t677 = load ptr, ptr %t607
  %t678 = call ptr @strcpy(i64 %t676, ptr %t677)
  br label %L4
L137:
  br label %L11
L11:
  %t679 = alloca i64
  %t680 = call i32 @new_label(ptr %t0)
  %t681 = sext i32 %t680 to i64
  store i64 %t681, ptr %t679
  %t682 = alloca i64
  %t683 = call i32 @new_label(ptr %t0)
  %t684 = sext i32 %t683 to i64
  store i64 %t684, ptr %t682
  %t685 = alloca i64
  %t686 = call i32 @new_label(ptr %t0)
  %t687 = sext i32 %t686 to i64
  store i64 %t687, ptr %t685
  %t688 = alloca ptr
  %t689 = alloca ptr
  %t690 = load ptr, ptr %t688
  %t691 = getelementptr i8, ptr %t0, i64 0
  %t692 = load i64, ptr %t691
  %t693 = call ptr @strcpy(ptr %t690, i64 %t692)
  %t694 = load ptr, ptr %t689
  %t695 = getelementptr i8, ptr %t0, i64 0
  %t696 = load i64, ptr %t695
  %t697 = call ptr @strcpy(ptr %t694, i64 %t696)
  %t698 = getelementptr i8, ptr %t0, i64 0
  %t699 = load i64, ptr %t698
  %t700 = getelementptr [4 x i8], ptr @.str275, i64 0, i64 0
  %t701 = load i64, ptr %t685
  %t702 = call i32 (ptr, ...) @snprintf(i64 %t699, i64 64, ptr %t700, i64 %t701)
  %t703 = sext i32 %t702 to i64
  %t704 = getelementptr i8, ptr %t0, i64 0
  %t705 = load i64, ptr %t704
  %t706 = getelementptr [4 x i8], ptr @.str276, i64 0, i64 0
  %t707 = load i64, ptr %t682
  %t708 = call i32 (ptr, ...) @snprintf(i64 %t705, i64 64, ptr %t706, i64 %t707)
  %t709 = sext i32 %t708 to i64
  %t710 = getelementptr i8, ptr %t0, i64 0
  %t711 = load i64, ptr %t710
  %t712 = getelementptr [18 x i8], ptr @.str277, i64 0, i64 0
  %t713 = load i64, ptr %t679
  call void (ptr, ...) @__c0c_emit(i64 %t711, ptr %t712, i64 %t713)
  %t715 = getelementptr i8, ptr %t0, i64 0
  %t716 = load i64, ptr %t715
  %t717 = getelementptr [6 x i8], ptr @.str278, i64 0, i64 0
  %t718 = load i64, ptr %t679
  call void (ptr, ...) @__c0c_emit(i64 %t716, ptr %t717, i64 %t718)
  %t720 = getelementptr i8, ptr %t1, i64 0
  %t721 = load i64, ptr %t720
  call void @emit_stmt(ptr %t0, i64 %t721)
  %t723 = getelementptr i8, ptr %t0, i64 0
  %t724 = load i64, ptr %t723
  %t725 = getelementptr [18 x i8], ptr @.str279, i64 0, i64 0
  %t726 = load i64, ptr %t682
  call void (ptr, ...) @__c0c_emit(i64 %t724, ptr %t725, i64 %t726)
  %t728 = getelementptr i8, ptr %t0, i64 0
  %t729 = load i64, ptr %t728
  %t730 = getelementptr [6 x i8], ptr @.str280, i64 0, i64 0
  %t731 = load i64, ptr %t682
  call void (ptr, ...) @__c0c_emit(i64 %t729, ptr %t730, i64 %t731)
  %t733 = alloca i64
  %t734 = getelementptr i8, ptr %t1, i64 0
  %t735 = load i64, ptr %t734
  %t736 = call i64 @emit_expr(ptr %t0, i64 %t735)
  store i64 %t736, ptr %t733
  %t737 = alloca i64
  %t738 = load i64, ptr %t733
  %t739 = call i32 @emit_cond(ptr %t0, i64 %t738)
  %t740 = sext i32 %t739 to i64
  store i64 %t740, ptr %t737
  %t741 = getelementptr i8, ptr %t0, i64 0
  %t742 = load i64, ptr %t741
  %t743 = getelementptr [41 x i8], ptr @.str281, i64 0, i64 0
  %t744 = load i64, ptr %t737
  %t745 = load i64, ptr %t679
  %t746 = load i64, ptr %t685
  call void (ptr, ...) @__c0c_emit(i64 %t742, ptr %t743, i64 %t744, i64 %t745, i64 %t746)
  %t748 = getelementptr i8, ptr %t0, i64 0
  %t749 = load i64, ptr %t748
  %t750 = getelementptr [6 x i8], ptr @.str282, i64 0, i64 0
  %t751 = load i64, ptr %t685
  call void (ptr, ...) @__c0c_emit(i64 %t749, ptr %t750, i64 %t751)
  %t753 = getelementptr i8, ptr %t0, i64 0
  %t754 = load i64, ptr %t753
  %t755 = load ptr, ptr %t688
  %t756 = call ptr @strcpy(i64 %t754, ptr %t755)
  %t757 = getelementptr i8, ptr %t0, i64 0
  %t758 = load i64, ptr %t757
  %t759 = load ptr, ptr %t689
  %t760 = call ptr @strcpy(i64 %t758, ptr %t759)
  br label %L4
L138:
  br label %L12
L12:
  %t761 = alloca i64
  %t762 = call i32 @new_label(ptr %t0)
  %t763 = sext i32 %t762 to i64
  store i64 %t763, ptr %t761
  %t764 = alloca i64
  %t765 = call i32 @new_label(ptr %t0)
  %t766 = sext i32 %t765 to i64
  store i64 %t766, ptr %t764
  %t767 = alloca i64
  %t768 = call i32 @new_label(ptr %t0)
  %t769 = sext i32 %t768 to i64
  store i64 %t769, ptr %t767
  %t770 = alloca i64
  %t771 = call i32 @new_label(ptr %t0)
  %t772 = sext i32 %t771 to i64
  store i64 %t772, ptr %t770
  %t773 = alloca ptr
  %t774 = alloca ptr
  %t775 = load ptr, ptr %t773
  %t776 = getelementptr i8, ptr %t0, i64 0
  %t777 = load i64, ptr %t776
  %t778 = call ptr @strcpy(ptr %t775, i64 %t777)
  %t779 = load ptr, ptr %t774
  %t780 = getelementptr i8, ptr %t0, i64 0
  %t781 = load i64, ptr %t780
  %t782 = call ptr @strcpy(ptr %t779, i64 %t781)
  %t783 = getelementptr i8, ptr %t0, i64 0
  %t784 = load i64, ptr %t783
  %t785 = getelementptr [4 x i8], ptr @.str283, i64 0, i64 0
  %t786 = load i64, ptr %t770
  %t787 = call i32 (ptr, ...) @snprintf(i64 %t784, i64 64, ptr %t785, i64 %t786)
  %t788 = sext i32 %t787 to i64
  %t789 = getelementptr i8, ptr %t0, i64 0
  %t790 = load i64, ptr %t789
  %t791 = getelementptr [4 x i8], ptr @.str284, i64 0, i64 0
  %t792 = load i64, ptr %t767
  %t793 = call i32 (ptr, ...) @snprintf(i64 %t790, i64 64, ptr %t791, i64 %t792)
  %t794 = sext i32 %t793 to i64
  call void @scope_push(ptr %t0)
  %t796 = getelementptr i8, ptr %t1, i64 0
  %t797 = load i64, ptr %t796
  %t798 = icmp ne i64 %t797, 0
  br i1 %t798, label %L139, label %L141
L139:
  %t799 = getelementptr i8, ptr %t1, i64 0
  %t800 = load i64, ptr %t799
  call void @emit_stmt(ptr %t0, i64 %t800)
  br label %L141
L141:
  %t802 = getelementptr i8, ptr %t0, i64 0
  %t803 = load i64, ptr %t802
  %t804 = getelementptr [18 x i8], ptr @.str285, i64 0, i64 0
  %t805 = load i64, ptr %t761
  call void (ptr, ...) @__c0c_emit(i64 %t803, ptr %t804, i64 %t805)
  %t807 = getelementptr i8, ptr %t0, i64 0
  %t808 = load i64, ptr %t807
  %t809 = getelementptr [6 x i8], ptr @.str286, i64 0, i64 0
  %t810 = load i64, ptr %t761
  call void (ptr, ...) @__c0c_emit(i64 %t808, ptr %t809, i64 %t810)
  %t812 = getelementptr i8, ptr %t1, i64 0
  %t813 = load i64, ptr %t812
  %t814 = icmp ne i64 %t813, 0
  br i1 %t814, label %L142, label %L143
L142:
  %t815 = alloca i64
  %t816 = getelementptr i8, ptr %t1, i64 0
  %t817 = load i64, ptr %t816
  %t818 = call i64 @emit_expr(ptr %t0, i64 %t817)
  store i64 %t818, ptr %t815
  %t819 = alloca i64
  %t820 = load i64, ptr %t815
  %t821 = call i32 @emit_cond(ptr %t0, i64 %t820)
  %t822 = sext i32 %t821 to i64
  store i64 %t822, ptr %t819
  %t823 = getelementptr i8, ptr %t0, i64 0
  %t824 = load i64, ptr %t823
  %t825 = getelementptr [41 x i8], ptr @.str287, i64 0, i64 0
  %t826 = load i64, ptr %t819
  %t827 = load i64, ptr %t764
  %t828 = load i64, ptr %t770
  call void (ptr, ...) @__c0c_emit(i64 %t824, ptr %t825, i64 %t826, i64 %t827, i64 %t828)
  br label %L144
L143:
  %t830 = getelementptr i8, ptr %t0, i64 0
  %t831 = load i64, ptr %t830
  %t832 = getelementptr [18 x i8], ptr @.str288, i64 0, i64 0
  %t833 = load i64, ptr %t764
  call void (ptr, ...) @__c0c_emit(i64 %t831, ptr %t832, i64 %t833)
  br label %L144
L144:
  %t835 = getelementptr i8, ptr %t0, i64 0
  %t836 = load i64, ptr %t835
  %t837 = getelementptr [6 x i8], ptr @.str289, i64 0, i64 0
  %t838 = load i64, ptr %t764
  call void (ptr, ...) @__c0c_emit(i64 %t836, ptr %t837, i64 %t838)
  %t840 = getelementptr i8, ptr %t1, i64 0
  %t841 = load i64, ptr %t840
  call void @emit_stmt(ptr %t0, i64 %t841)
  %t843 = getelementptr i8, ptr %t0, i64 0
  %t844 = load i64, ptr %t843
  %t845 = getelementptr [18 x i8], ptr @.str290, i64 0, i64 0
  %t846 = load i64, ptr %t767
  call void (ptr, ...) @__c0c_emit(i64 %t844, ptr %t845, i64 %t846)
  %t848 = getelementptr i8, ptr %t0, i64 0
  %t849 = load i64, ptr %t848
  %t850 = getelementptr [6 x i8], ptr @.str291, i64 0, i64 0
  %t851 = load i64, ptr %t767
  call void (ptr, ...) @__c0c_emit(i64 %t849, ptr %t850, i64 %t851)
  %t853 = getelementptr i8, ptr %t1, i64 0
  %t854 = load i64, ptr %t853
  %t855 = icmp ne i64 %t854, 0
  br i1 %t855, label %L145, label %L147
L145:
  %t856 = getelementptr i8, ptr %t1, i64 0
  %t857 = load i64, ptr %t856
  %t858 = call i64 @emit_expr(ptr %t0, i64 %t857)
  br label %L147
L147:
  %t859 = getelementptr i8, ptr %t0, i64 0
  %t860 = load i64, ptr %t859
  %t861 = getelementptr [18 x i8], ptr @.str292, i64 0, i64 0
  %t862 = load i64, ptr %t761
  call void (ptr, ...) @__c0c_emit(i64 %t860, ptr %t861, i64 %t862)
  %t864 = getelementptr i8, ptr %t0, i64 0
  %t865 = load i64, ptr %t864
  %t866 = getelementptr [6 x i8], ptr @.str293, i64 0, i64 0
  %t867 = load i64, ptr %t770
  call void (ptr, ...) @__c0c_emit(i64 %t865, ptr %t866, i64 %t867)
  call void @scope_pop(ptr %t0)
  %t870 = getelementptr i8, ptr %t0, i64 0
  %t871 = load i64, ptr %t870
  %t872 = load ptr, ptr %t773
  %t873 = call ptr @strcpy(i64 %t871, ptr %t872)
  %t874 = getelementptr i8, ptr %t0, i64 0
  %t875 = load i64, ptr %t874
  %t876 = load ptr, ptr %t774
  %t877 = call ptr @strcpy(i64 %t875, ptr %t876)
  br label %L4
L148:
  br label %L13
L13:
  %t878 = getelementptr i8, ptr %t0, i64 0
  %t879 = load i64, ptr %t878
  %t880 = getelementptr [17 x i8], ptr @.str294, i64 0, i64 0
  %t881 = getelementptr i8, ptr %t0, i64 0
  %t882 = load i64, ptr %t881
  call void (ptr, ...) @__c0c_emit(i64 %t879, ptr %t880, i64 %t882)
  %t884 = alloca i64
  %t885 = call i32 @new_label(ptr %t0)
  %t886 = sext i32 %t885 to i64
  store i64 %t886, ptr %t884
  %t887 = getelementptr i8, ptr %t0, i64 0
  %t888 = load i64, ptr %t887
  %t889 = getelementptr [6 x i8], ptr @.str295, i64 0, i64 0
  %t890 = load i64, ptr %t884
  call void (ptr, ...) @__c0c_emit(i64 %t888, ptr %t889, i64 %t890)
  br label %L4
L149:
  br label %L14
L14:
  %t892 = getelementptr i8, ptr %t0, i64 0
  %t893 = load i64, ptr %t892
  %t894 = getelementptr [17 x i8], ptr @.str296, i64 0, i64 0
  %t895 = getelementptr i8, ptr %t0, i64 0
  %t896 = load i64, ptr %t895
  call void (ptr, ...) @__c0c_emit(i64 %t893, ptr %t894, i64 %t896)
  %t898 = alloca i64
  %t899 = call i32 @new_label(ptr %t0)
  %t900 = sext i32 %t899 to i64
  store i64 %t900, ptr %t898
  %t901 = getelementptr i8, ptr %t0, i64 0
  %t902 = load i64, ptr %t901
  %t903 = getelementptr [6 x i8], ptr @.str297, i64 0, i64 0
  %t904 = load i64, ptr %t898
  call void (ptr, ...) @__c0c_emit(i64 %t902, ptr %t903, i64 %t904)
  br label %L4
L150:
  br label %L15
L15:
  %t906 = alloca i64
  %t907 = getelementptr i8, ptr %t1, i64 0
  %t908 = load i64, ptr %t907
  %t909 = call i64 @emit_expr(ptr %t0, i64 %t908)
  store i64 %t909, ptr %t906
  %t910 = alloca i64
  %t911 = call i32 @new_label(ptr %t0)
  %t912 = sext i32 %t911 to i64
  store i64 %t912, ptr %t910
  %t913 = alloca ptr
  %t914 = load ptr, ptr %t913
  %t915 = getelementptr i8, ptr %t0, i64 0
  %t916 = load i64, ptr %t915
  %t917 = call ptr @strcpy(ptr %t914, i64 %t916)
  %t918 = getelementptr i8, ptr %t0, i64 0
  %t919 = load i64, ptr %t918
  %t920 = getelementptr [4 x i8], ptr @.str298, i64 0, i64 0
  %t921 = load i64, ptr %t910
  %t922 = call i32 (ptr, ...) @snprintf(i64 %t919, i64 64, ptr %t920, i64 %t921)
  %t923 = sext i32 %t922 to i64
  %t924 = alloca ptr
  %t925 = getelementptr i8, ptr %t1, i64 0
  %t926 = load i64, ptr %t925
  store i64 %t926, ptr %t924
  %t927 = alloca i64
  %t928 = sext i32 0 to i64
  store i64 %t928, ptr %t927
  %t929 = alloca ptr
  %t930 = alloca ptr
  %t931 = alloca i64
  %t932 = load i64, ptr %t910
  %t933 = sext i32 %t932 to i64
  store i64 %t933, ptr %t931
  %t934 = alloca i64
  %t935 = sext i32 0 to i64
  store i64 %t935, ptr %t934
  br label %L151
L151:
  %t936 = load i64, ptr %t934
  %t937 = load ptr, ptr %t924
  %t938 = getelementptr i8, ptr %t937, i64 0
  %t939 = load i64, ptr %t938
  %t941 = sext i32 %t936 to i64
  %t940 = icmp slt i64 %t941, %t939
  %t942 = zext i1 %t940 to i64
  %t943 = icmp ne i64 %t942, 0
  br i1 %t943, label %L155, label %L156
L155:
  %t944 = load i64, ptr %t927
  %t946 = sext i32 %t944 to i64
  %t947 = sext i32 256 to i64
  %t945 = icmp slt i64 %t946, %t947
  %t948 = zext i1 %t945 to i64
  %t949 = icmp ne i64 %t948, 0
  %t950 = zext i1 %t949 to i64
  br label %L157
L156:
  br label %L157
L157:
  %t951 = phi i64 [ %t950, %L155 ], [ 0, %L156 ]
  %t952 = icmp ne i64 %t951, 0
  br i1 %t952, label %L152, label %L154
L152:
  %t953 = alloca ptr
  %t954 = load ptr, ptr %t924
  %t955 = getelementptr i8, ptr %t954, i64 0
  %t956 = load i64, ptr %t955
  %t957 = load i64, ptr %t934
  %t958 = inttoptr i64 %t956 to ptr
  %t959 = sext i32 %t957 to i64
  %t960 = getelementptr ptr, ptr %t958, i64 %t959
  %t961 = load ptr, ptr %t960
  store ptr %t961, ptr %t953
  %t962 = load ptr, ptr %t953
  %t963 = getelementptr i8, ptr %t962, i64 0
  %t964 = load i64, ptr %t963
  %t966 = sext i32 14 to i64
  %t965 = icmp eq i64 %t964, %t966
  %t967 = zext i1 %t965 to i64
  %t968 = icmp ne i64 %t967, 0
  br i1 %t968, label %L158, label %L159
L158:
  %t969 = call i32 @new_label(ptr %t0)
  %t970 = sext i32 %t969 to i64
  %t971 = load ptr, ptr %t929
  %t972 = load i64, ptr %t927
  %t974 = sext i32 %t972 to i64
  %t973 = getelementptr ptr, ptr %t971, i64 %t974
  store i64 %t970, ptr %t973
  %t975 = load ptr, ptr %t953
  %t976 = getelementptr i8, ptr %t975, i64 0
  %t977 = load i64, ptr %t976
  %t978 = icmp ne i64 %t977, 0
  br i1 %t978, label %L161, label %L162
L161:
  %t979 = load ptr, ptr %t953
  %t980 = getelementptr i8, ptr %t979, i64 0
  %t981 = load i64, ptr %t980
  %t982 = inttoptr i64 %t981 to ptr
  %t983 = getelementptr i8, ptr %t982, i64 0
  %t984 = load i64, ptr %t983
  br label %L163
L162:
  %t985 = sext i32 0 to i64
  br label %L163
L163:
  %t986 = phi i64 [ %t984, %L161 ], [ %t985, %L162 ]
  %t987 = load ptr, ptr %t930
  %t988 = load i64, ptr %t927
  %t990 = sext i32 %t988 to i64
  %t989 = getelementptr ptr, ptr %t987, i64 %t990
  store i64 %t986, ptr %t989
  %t991 = load i64, ptr %t927
  %t993 = sext i32 %t991 to i64
  %t992 = add i64 %t993, 1
  store i64 %t992, ptr %t927
  br label %L160
L159:
  %t994 = load ptr, ptr %t953
  %t995 = getelementptr i8, ptr %t994, i64 0
  %t996 = load i64, ptr %t995
  %t998 = sext i32 15 to i64
  %t997 = icmp eq i64 %t996, %t998
  %t999 = zext i1 %t997 to i64
  %t1000 = icmp ne i64 %t999, 0
  br i1 %t1000, label %L164, label %L166
L164:
  %t1001 = call i32 @new_label(ptr %t0)
  %t1002 = sext i32 %t1001 to i64
  store i64 %t1002, ptr %t931
  br label %L166
L166:
  br label %L160
L160:
  br label %L153
L153:
  %t1003 = load i64, ptr %t934
  %t1005 = sext i32 %t1003 to i64
  %t1004 = add i64 %t1005, 1
  store i64 %t1004, ptr %t934
  br label %L151
L154:
  %t1006 = alloca ptr
  %t1007 = load i64, ptr %t906
  %t1008 = load ptr, ptr %t1006
  %t1009 = call i32 @promote_to_i64(ptr %t0, i64 %t1007, ptr %t1008, i64 64)
  %t1010 = sext i32 %t1009 to i64
  %t1011 = alloca i64
  %t1012 = call i32 @new_reg(ptr %t0)
  %t1013 = sext i32 %t1012 to i64
  store i64 %t1013, ptr %t1011
  %t1014 = getelementptr i8, ptr %t0, i64 0
  %t1015 = load i64, ptr %t1014
  %t1016 = getelementptr [25 x i8], ptr @.str299, i64 0, i64 0
  %t1017 = load i64, ptr %t1011
  %t1018 = load ptr, ptr %t1006
  call void (ptr, ...) @__c0c_emit(i64 %t1015, ptr %t1016, i64 %t1017, ptr %t1018)
  %t1020 = getelementptr i8, ptr %t0, i64 0
  %t1021 = load i64, ptr %t1020
  %t1022 = getelementptr [35 x i8], ptr @.str300, i64 0, i64 0
  %t1023 = load i64, ptr %t1011
  %t1024 = load i64, ptr %t931
  call void (ptr, ...) @__c0c_emit(i64 %t1021, ptr %t1022, i64 %t1023, i64 %t1024)
  %t1026 = alloca i64
  %t1027 = sext i32 0 to i64
  store i64 %t1027, ptr %t1026
  %t1028 = alloca i64
  %t1029 = sext i32 0 to i64
  store i64 %t1029, ptr %t1028
  br label %L167
L167:
  %t1030 = load i64, ptr %t1028
  %t1031 = load ptr, ptr %t924
  %t1032 = getelementptr i8, ptr %t1031, i64 0
  %t1033 = load i64, ptr %t1032
  %t1035 = sext i32 %t1030 to i64
  %t1034 = icmp slt i64 %t1035, %t1033
  %t1036 = zext i1 %t1034 to i64
  %t1037 = icmp ne i64 %t1036, 0
  br i1 %t1037, label %L168, label %L170
L168:
  %t1038 = alloca ptr
  %t1039 = load ptr, ptr %t924
  %t1040 = getelementptr i8, ptr %t1039, i64 0
  %t1041 = load i64, ptr %t1040
  %t1042 = load i64, ptr %t1028
  %t1043 = inttoptr i64 %t1041 to ptr
  %t1044 = sext i32 %t1042 to i64
  %t1045 = getelementptr ptr, ptr %t1043, i64 %t1044
  %t1046 = load ptr, ptr %t1045
  store ptr %t1046, ptr %t1038
  %t1047 = load ptr, ptr %t1038
  %t1048 = getelementptr i8, ptr %t1047, i64 0
  %t1049 = load i64, ptr %t1048
  %t1051 = sext i32 14 to i64
  %t1050 = icmp eq i64 %t1049, %t1051
  %t1052 = zext i1 %t1050 to i64
  %t1053 = icmp ne i64 %t1052, 0
  br i1 %t1053, label %L171, label %L172
L171:
  %t1054 = load i64, ptr %t1026
  %t1055 = load i64, ptr %t927
  %t1057 = sext i32 %t1054 to i64
  %t1058 = sext i32 %t1055 to i64
  %t1056 = icmp slt i64 %t1057, %t1058
  %t1059 = zext i1 %t1056 to i64
  %t1060 = icmp ne i64 %t1059, 0
  %t1061 = zext i1 %t1060 to i64
  br label %L173
L172:
  br label %L173
L173:
  %t1062 = phi i64 [ %t1061, %L171 ], [ 0, %L172 ]
  %t1063 = icmp ne i64 %t1062, 0
  br i1 %t1063, label %L174, label %L176
L174:
  %t1064 = getelementptr i8, ptr %t0, i64 0
  %t1065 = load i64, ptr %t1064
  %t1066 = getelementptr [27 x i8], ptr @.str301, i64 0, i64 0
  %t1067 = load ptr, ptr %t930
  %t1068 = load i64, ptr %t1026
  %t1069 = sext i32 %t1068 to i64
  %t1070 = getelementptr ptr, ptr %t1067, i64 %t1069
  %t1071 = load ptr, ptr %t1070
  %t1072 = load ptr, ptr %t929
  %t1073 = load i64, ptr %t1026
  %t1074 = sext i32 %t1073 to i64
  %t1075 = getelementptr ptr, ptr %t1072, i64 %t1074
  %t1076 = load ptr, ptr %t1075
  call void (ptr, ...) @__c0c_emit(i64 %t1065, ptr %t1066, ptr %t1071, ptr %t1076)
  %t1078 = load i64, ptr %t1026
  %t1080 = sext i32 %t1078 to i64
  %t1079 = add i64 %t1080, 1
  store i64 %t1079, ptr %t1026
  br label %L176
L176:
  br label %L169
L169:
  %t1081 = load i64, ptr %t1028
  %t1083 = sext i32 %t1081 to i64
  %t1082 = add i64 %t1083, 1
  store i64 %t1082, ptr %t1028
  br label %L167
L170:
  %t1084 = getelementptr i8, ptr %t0, i64 0
  %t1085 = load i64, ptr %t1084
  %t1086 = getelementptr [5 x i8], ptr @.str302, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t1085, ptr %t1086)
  %t1088 = sext i32 0 to i64
  store i64 %t1088, ptr %t1026
  %t1089 = alloca i64
  %t1090 = sext i32 0 to i64
  store i64 %t1090, ptr %t1089
  %t1091 = alloca i64
  %t1092 = sext i32 0 to i64
  store i64 %t1092, ptr %t1091
  br label %L177
L177:
  %t1093 = load i64, ptr %t1091
  %t1094 = load ptr, ptr %t924
  %t1095 = getelementptr i8, ptr %t1094, i64 0
  %t1096 = load i64, ptr %t1095
  %t1098 = sext i32 %t1093 to i64
  %t1097 = icmp slt i64 %t1098, %t1096
  %t1099 = zext i1 %t1097 to i64
  %t1100 = icmp ne i64 %t1099, 0
  br i1 %t1100, label %L178, label %L180
L178:
  %t1101 = alloca ptr
  %t1102 = load ptr, ptr %t924
  %t1103 = getelementptr i8, ptr %t1102, i64 0
  %t1104 = load i64, ptr %t1103
  %t1105 = load i64, ptr %t1091
  %t1106 = inttoptr i64 %t1104 to ptr
  %t1107 = sext i32 %t1105 to i64
  %t1108 = getelementptr ptr, ptr %t1106, i64 %t1107
  %t1109 = load ptr, ptr %t1108
  store ptr %t1109, ptr %t1101
  %t1110 = load ptr, ptr %t1101
  %t1111 = getelementptr i8, ptr %t1110, i64 0
  %t1112 = load i64, ptr %t1111
  %t1114 = sext i32 14 to i64
  %t1113 = icmp eq i64 %t1112, %t1114
  %t1115 = zext i1 %t1113 to i64
  %t1116 = icmp ne i64 %t1115, 0
  br i1 %t1116, label %L181, label %L182
L181:
  %t1117 = load i64, ptr %t1026
  %t1118 = load i64, ptr %t927
  %t1120 = sext i32 %t1117 to i64
  %t1121 = sext i32 %t1118 to i64
  %t1119 = icmp slt i64 %t1120, %t1121
  %t1122 = zext i1 %t1119 to i64
  %t1123 = icmp ne i64 %t1122, 0
  %t1124 = zext i1 %t1123 to i64
  br label %L183
L182:
  br label %L183
L183:
  %t1125 = phi i64 [ %t1124, %L181 ], [ 0, %L182 ]
  %t1126 = icmp ne i64 %t1125, 0
  br i1 %t1126, label %L184, label %L185
L184:
  %t1127 = getelementptr i8, ptr %t0, i64 0
  %t1128 = load i64, ptr %t1127
  %t1129 = getelementptr [6 x i8], ptr @.str303, i64 0, i64 0
  %t1130 = load ptr, ptr %t929
  %t1131 = load i64, ptr %t1026
  %t1133 = sext i32 %t1131 to i64
  %t1132 = add i64 %t1133, 1
  store i64 %t1132, ptr %t1026
  %t1134 = sext i32 %t1131 to i64
  %t1135 = getelementptr ptr, ptr %t1130, i64 %t1134
  %t1136 = load ptr, ptr %t1135
  call void (ptr, ...) @__c0c_emit(i64 %t1128, ptr %t1129, ptr %t1136)
  %t1138 = load ptr, ptr %t1101
  %t1139 = getelementptr i8, ptr %t1138, i64 0
  %t1140 = load i64, ptr %t1139
  %t1142 = sext i32 0 to i64
  %t1141 = icmp sgt i64 %t1140, %t1142
  %t1143 = zext i1 %t1141 to i64
  %t1144 = icmp ne i64 %t1143, 0
  br i1 %t1144, label %L187, label %L189
L187:
  %t1145 = load ptr, ptr %t1101
  %t1146 = getelementptr i8, ptr %t1145, i64 0
  %t1147 = load i64, ptr %t1146
  %t1148 = inttoptr i64 %t1147 to ptr
  %t1149 = sext i32 0 to i64
  %t1150 = getelementptr ptr, ptr %t1148, i64 %t1149
  %t1151 = load ptr, ptr %t1150
  call void @emit_stmt(ptr %t0, ptr %t1151)
  br label %L189
L189:
  %t1153 = alloca i64
  %t1154 = load i64, ptr %t1026
  %t1155 = load i64, ptr %t927
  %t1157 = sext i32 %t1154 to i64
  %t1158 = sext i32 %t1155 to i64
  %t1156 = icmp slt i64 %t1157, %t1158
  %t1159 = zext i1 %t1156 to i64
  %t1160 = icmp ne i64 %t1159, 0
  br i1 %t1160, label %L190, label %L191
L190:
  %t1161 = load ptr, ptr %t929
  %t1162 = load i64, ptr %t1026
  %t1163 = sext i32 %t1162 to i64
  %t1164 = getelementptr ptr, ptr %t1161, i64 %t1163
  %t1165 = load ptr, ptr %t1164
  %t1166 = ptrtoint ptr %t1165 to i64
  br label %L192
L191:
  %t1167 = load i64, ptr %t910
  %t1168 = sext i32 %t1167 to i64
  br label %L192
L192:
  %t1169 = phi i64 [ %t1166, %L190 ], [ %t1168, %L191 ]
  store i64 %t1169, ptr %t1153
  %t1170 = getelementptr i8, ptr %t0, i64 0
  %t1171 = load i64, ptr %t1170
  %t1172 = getelementptr [18 x i8], ptr @.str304, i64 0, i64 0
  %t1173 = load i64, ptr %t1153
  call void (ptr, ...) @__c0c_emit(i64 %t1171, ptr %t1172, i64 %t1173)
  br label %L186
L185:
  %t1175 = load ptr, ptr %t1101
  %t1176 = getelementptr i8, ptr %t1175, i64 0
  %t1177 = load i64, ptr %t1176
  %t1179 = sext i32 15 to i64
  %t1178 = icmp eq i64 %t1177, %t1179
  %t1180 = zext i1 %t1178 to i64
  %t1181 = icmp ne i64 %t1180, 0
  br i1 %t1181, label %L193, label %L194
L193:
  %t1182 = getelementptr i8, ptr %t0, i64 0
  %t1183 = load i64, ptr %t1182
  %t1184 = getelementptr [6 x i8], ptr @.str305, i64 0, i64 0
  %t1185 = load i64, ptr %t931
  call void (ptr, ...) @__c0c_emit(i64 %t1183, ptr %t1184, i64 %t1185)
  %t1187 = load ptr, ptr %t1101
  %t1188 = getelementptr i8, ptr %t1187, i64 0
  %t1189 = load i64, ptr %t1188
  %t1191 = sext i32 0 to i64
  %t1190 = icmp sgt i64 %t1189, %t1191
  %t1192 = zext i1 %t1190 to i64
  %t1193 = icmp ne i64 %t1192, 0
  br i1 %t1193, label %L196, label %L198
L196:
  %t1194 = load ptr, ptr %t1101
  %t1195 = getelementptr i8, ptr %t1194, i64 0
  %t1196 = load i64, ptr %t1195
  %t1197 = inttoptr i64 %t1196 to ptr
  %t1198 = sext i32 0 to i64
  %t1199 = getelementptr ptr, ptr %t1197, i64 %t1198
  %t1200 = load ptr, ptr %t1199
  call void @emit_stmt(ptr %t0, ptr %t1200)
  br label %L198
L198:
  %t1202 = getelementptr i8, ptr %t0, i64 0
  %t1203 = load i64, ptr %t1202
  %t1204 = getelementptr [18 x i8], ptr @.str306, i64 0, i64 0
  %t1205 = load i64, ptr %t910
  call void (ptr, ...) @__c0c_emit(i64 %t1203, ptr %t1204, i64 %t1205)
  %t1207 = load i64, ptr %t1089
  %t1209 = sext i32 %t1207 to i64
  %t1208 = add i64 %t1209, 1
  store i64 %t1208, ptr %t1089
  br label %L195
L194:
  %t1210 = load ptr, ptr %t1101
  call void @emit_stmt(ptr %t0, ptr %t1210)
  br label %L195
L195:
  br label %L186
L186:
  br label %L179
L179:
  %t1212 = load i64, ptr %t1091
  %t1214 = sext i32 %t1212 to i64
  %t1213 = add i64 %t1214, 1
  store i64 %t1213, ptr %t1091
  br label %L177
L180:
  %t1215 = getelementptr i8, ptr %t0, i64 0
  %t1216 = load i64, ptr %t1215
  %t1217 = getelementptr [6 x i8], ptr @.str307, i64 0, i64 0
  %t1218 = load i64, ptr %t910
  call void (ptr, ...) @__c0c_emit(i64 %t1216, ptr %t1217, i64 %t1218)
  %t1220 = getelementptr i8, ptr %t0, i64 0
  %t1221 = load i64, ptr %t1220
  %t1222 = load ptr, ptr %t913
  %t1223 = call ptr @strcpy(i64 %t1221, ptr %t1222)
  br label %L4
L199:
  br label %L16
L16:
  %t1224 = getelementptr i8, ptr %t1, i64 0
  %t1225 = load i64, ptr %t1224
  %t1227 = sext i32 0 to i64
  %t1226 = icmp sgt i64 %t1225, %t1227
  %t1228 = zext i1 %t1226 to i64
  %t1229 = icmp ne i64 %t1228, 0
  br i1 %t1229, label %L200, label %L202
L200:
  %t1230 = getelementptr i8, ptr %t1, i64 0
  %t1231 = load i64, ptr %t1230
  %t1232 = inttoptr i64 %t1231 to ptr
  %t1233 = sext i32 0 to i64
  %t1234 = getelementptr ptr, ptr %t1232, i64 %t1233
  %t1235 = load ptr, ptr %t1234
  call void @emit_stmt(ptr %t0, ptr %t1235)
  br label %L202
L202:
  br label %L4
L203:
  br label %L17
L17:
  %t1237 = getelementptr i8, ptr %t1, i64 0
  %t1238 = load i64, ptr %t1237
  %t1240 = sext i32 0 to i64
  %t1239 = icmp sgt i64 %t1238, %t1240
  %t1241 = zext i1 %t1239 to i64
  %t1242 = icmp ne i64 %t1241, 0
  br i1 %t1242, label %L204, label %L206
L204:
  %t1243 = getelementptr i8, ptr %t1, i64 0
  %t1244 = load i64, ptr %t1243
  %t1245 = inttoptr i64 %t1244 to ptr
  %t1246 = sext i32 0 to i64
  %t1247 = getelementptr ptr, ptr %t1245, i64 %t1246
  %t1248 = load ptr, ptr %t1247
  call void @emit_stmt(ptr %t0, ptr %t1248)
  br label %L206
L206:
  br label %L4
L207:
  br label %L18
L18:
  %t1250 = getelementptr i8, ptr %t0, i64 0
  %t1251 = load i64, ptr %t1250
  %t1252 = getelementptr [17 x i8], ptr @.str308, i64 0, i64 0
  %t1253 = getelementptr i8, ptr %t1, i64 0
  %t1254 = load i64, ptr %t1253
  call void (ptr, ...) @__c0c_emit(i64 %t1251, ptr %t1252, i64 %t1254)
  %t1256 = getelementptr i8, ptr %t0, i64 0
  %t1257 = load i64, ptr %t1256
  %t1258 = getelementptr [5 x i8], ptr @.str309, i64 0, i64 0
  %t1259 = getelementptr i8, ptr %t1, i64 0
  %t1260 = load i64, ptr %t1259
  call void (ptr, ...) @__c0c_emit(i64 %t1257, ptr %t1258, i64 %t1260)
  %t1262 = getelementptr i8, ptr %t1, i64 0
  %t1263 = load i64, ptr %t1262
  %t1265 = sext i32 0 to i64
  %t1264 = icmp sgt i64 %t1263, %t1265
  %t1266 = zext i1 %t1264 to i64
  %t1267 = icmp ne i64 %t1266, 0
  br i1 %t1267, label %L208, label %L210
L208:
  %t1268 = getelementptr i8, ptr %t1, i64 0
  %t1269 = load i64, ptr %t1268
  %t1270 = inttoptr i64 %t1269 to ptr
  %t1271 = sext i32 0 to i64
  %t1272 = getelementptr ptr, ptr %t1270, i64 %t1271
  %t1273 = load ptr, ptr %t1272
  call void @emit_stmt(ptr %t0, ptr %t1273)
  br label %L210
L210:
  br label %L4
L211:
  br label %L19
L19:
  %t1275 = getelementptr i8, ptr %t0, i64 0
  %t1276 = load i64, ptr %t1275
  %t1277 = getelementptr [17 x i8], ptr @.str310, i64 0, i64 0
  %t1278 = getelementptr i8, ptr %t1, i64 0
  %t1279 = load i64, ptr %t1278
  call void (ptr, ...) @__c0c_emit(i64 %t1276, ptr %t1277, i64 %t1279)
  %t1281 = alloca i64
  %t1282 = call i32 @new_label(ptr %t0)
  %t1283 = sext i32 %t1282 to i64
  store i64 %t1283, ptr %t1281
  %t1284 = getelementptr i8, ptr %t0, i64 0
  %t1285 = load i64, ptr %t1284
  %t1286 = getelementptr [6 x i8], ptr @.str311, i64 0, i64 0
  %t1287 = load i64, ptr %t1281
  call void (ptr, ...) @__c0c_emit(i64 %t1285, ptr %t1286, i64 %t1287)
  br label %L4
L212:
  br label %L20
L20:
  br label %L4
L213:
  br label %L4
L21:
  %t1289 = call i64 @emit_expr(ptr %t0, ptr %t1)
  br label %L4
L214:
  br label %L4
L4:
  ret void
}

define internal void @emit_func_def(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t3 = getelementptr i8, ptr %t1, i64 0
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t2
  %t5 = load ptr, ptr %t2
  %t7 = ptrtoint ptr %t5 to i64
  %t8 = icmp eq i64 %t7, 0
  %t6 = zext i1 %t8 to i64
  %t9 = icmp ne i64 %t6, 0
  br i1 %t9, label %L0, label %L1
L0:
  br label %L2
L1:
  %t10 = load ptr, ptr %t2
  %t11 = getelementptr i8, ptr %t10, i64 0
  %t12 = load i64, ptr %t11
  %t14 = sext i32 17 to i64
  %t13 = icmp ne i64 %t12, %t14
  %t15 = zext i1 %t13 to i64
  %t16 = icmp ne i64 %t15, 0
  %t17 = zext i1 %t16 to i64
  br label %L2
L2:
  %t18 = phi i64 [ 1, %L0 ], [ %t17, %L1 ]
  %t19 = icmp ne i64 %t18, 0
  br i1 %t19, label %L3, label %L5
L3:
  ret void
L6:
  br label %L5
L5:
  %t20 = getelementptr i8, ptr %t0, i64 0
  %t21 = sext i32 0 to i64
  store i64 %t21, ptr %t20
  %t22 = getelementptr i8, ptr %t0, i64 0
  %t23 = sext i32 0 to i64
  store i64 %t23, ptr %t22
  %t24 = getelementptr i8, ptr %t0, i64 0
  %t25 = sext i32 0 to i64
  store i64 %t25, ptr %t24
  %t26 = load ptr, ptr %t2
  %t27 = getelementptr i8, ptr %t26, i64 0
  %t28 = load i64, ptr %t27
  %t29 = icmp ne i64 %t28, 0
  br i1 %t29, label %L7, label %L8
L7:
  %t30 = load ptr, ptr %t2
  %t31 = getelementptr i8, ptr %t30, i64 0
  %t32 = load i64, ptr %t31
  br label %L9
L8:
  %t33 = call ptr @default_int_type()
  %t34 = ptrtoint ptr %t33 to i64
  br label %L9
L9:
  %t35 = phi i64 [ %t32, %L7 ], [ %t34, %L8 ]
  %t36 = getelementptr i8, ptr %t0, i64 0
  store i64 %t35, ptr %t36
  %t37 = getelementptr i8, ptr %t0, i64 0
  %t38 = load i64, ptr %t37
  %t39 = getelementptr i8, ptr %t1, i64 0
  %t40 = load i64, ptr %t39
  %t41 = icmp ne i64 %t40, 0
  br i1 %t41, label %L10, label %L11
L10:
  %t42 = getelementptr i8, ptr %t1, i64 0
  %t43 = load i64, ptr %t42
  br label %L12
L11:
  %t44 = getelementptr [5 x i8], ptr @.str312, i64 0, i64 0
  %t45 = ptrtoint ptr %t44 to i64
  br label %L12
L12:
  %t46 = phi i64 [ %t43, %L10 ], [ %t45, %L11 ]
  %t47 = call ptr @strncpy(i64 %t38, i64 %t46, i64 127)
  %t48 = alloca ptr
  %t49 = getelementptr i8, ptr %t1, i64 0
  %t50 = load i64, ptr %t49
  %t51 = icmp ne i64 %t50, 0
  br i1 %t51, label %L13, label %L14
L13:
  %t52 = getelementptr [9 x i8], ptr @.str313, i64 0, i64 0
  %t53 = ptrtoint ptr %t52 to i64
  br label %L15
L14:
  %t54 = getelementptr [10 x i8], ptr @.str314, i64 0, i64 0
  %t55 = ptrtoint ptr %t54 to i64
  br label %L15
L15:
  %t56 = phi i64 [ %t53, %L13 ], [ %t55, %L14 ]
  store i64 %t56, ptr %t48
  %t57 = getelementptr i8, ptr %t0, i64 0
  %t58 = load i64, ptr %t57
  %t59 = getelementptr [18 x i8], ptr @.str315, i64 0, i64 0
  %t60 = load ptr, ptr %t48
  %t61 = load ptr, ptr %t2
  %t62 = call ptr @llvm_ret_type(ptr %t61)
  %t63 = getelementptr i8, ptr %t1, i64 0
  %t64 = load i64, ptr %t63
  %t65 = icmp ne i64 %t64, 0
  br i1 %t65, label %L16, label %L17
L16:
  %t66 = getelementptr i8, ptr %t1, i64 0
  %t67 = load i64, ptr %t66
  br label %L18
L17:
  %t68 = getelementptr [5 x i8], ptr @.str316, i64 0, i64 0
  %t69 = ptrtoint ptr %t68 to i64
  br label %L18
L18:
  %t70 = phi i64 [ %t67, %L16 ], [ %t69, %L17 ]
  call void (ptr, ...) @__c0c_emit(i64 %t58, ptr %t59, ptr %t60, ptr %t62, i64 %t70)
  call void @scope_push(ptr %t0)
  %t73 = alloca i64
  %t74 = sext i32 0 to i64
  store i64 %t74, ptr %t73
  %t75 = alloca i64
  %t76 = sext i32 0 to i64
  store i64 %t76, ptr %t75
  br label %L19
L19:
  %t77 = load i64, ptr %t75
  %t78 = load ptr, ptr %t2
  %t79 = getelementptr i8, ptr %t78, i64 0
  %t80 = load i64, ptr %t79
  %t82 = sext i32 %t77 to i64
  %t81 = icmp slt i64 %t82, %t80
  %t83 = zext i1 %t81 to i64
  %t84 = icmp ne i64 %t83, 0
  br i1 %t84, label %L20, label %L22
L20:
  %t85 = alloca ptr
  %t86 = load ptr, ptr %t2
  %t87 = getelementptr i8, ptr %t86, i64 0
  %t88 = load i64, ptr %t87
  %t89 = load i64, ptr %t75
  %t91 = inttoptr i64 %t88 to ptr
  %t92 = sext i32 %t89 to i64
  %t90 = getelementptr ptr, ptr %t91, i64 %t92
  %t93 = getelementptr i8, ptr %t90, i64 0
  %t94 = load i64, ptr %t93
  store i64 %t94, ptr %t85
  %t95 = load ptr, ptr %t85
  %t96 = ptrtoint ptr %t95 to i64
  %t97 = icmp ne i64 %t96, 0
  br i1 %t97, label %L23, label %L24
L23:
  %t98 = load ptr, ptr %t85
  %t99 = getelementptr i8, ptr %t98, i64 0
  %t100 = load i64, ptr %t99
  %t102 = sext i32 0 to i64
  %t101 = icmp eq i64 %t100, %t102
  %t103 = zext i1 %t101 to i64
  %t104 = icmp ne i64 %t103, 0
  %t105 = zext i1 %t104 to i64
  br label %L25
L24:
  br label %L25
L25:
  %t106 = phi i64 [ %t105, %L23 ], [ 0, %L24 ]
  %t107 = icmp ne i64 %t106, 0
  br i1 %t107, label %L26, label %L27
L26:
  %t108 = load ptr, ptr %t2
  %t109 = getelementptr i8, ptr %t108, i64 0
  %t110 = load i64, ptr %t109
  %t112 = sext i32 1 to i64
  %t111 = icmp eq i64 %t110, %t112
  %t113 = zext i1 %t111 to i64
  %t114 = icmp ne i64 %t113, 0
  %t115 = zext i1 %t114 to i64
  br label %L28
L27:
  br label %L28
L28:
  %t116 = phi i64 [ %t115, %L26 ], [ 0, %L27 ]
  %t117 = icmp ne i64 %t116, 0
  br i1 %t117, label %L29, label %L31
L29:
  br label %L22
L32:
  br label %L31
L31:
  %t118 = load i64, ptr %t73
  %t120 = sext i32 %t118 to i64
  %t119 = icmp ne i64 %t120, 0
  br i1 %t119, label %L33, label %L35
L33:
  %t121 = getelementptr i8, ptr %t0, i64 0
  %t122 = load i64, ptr %t121
  %t123 = getelementptr [3 x i8], ptr @.str317, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t122, ptr %t123)
  br label %L35
L35:
  %t125 = alloca ptr
  %t126 = alloca ptr
  %t127 = load ptr, ptr %t85
  %t129 = ptrtoint ptr %t127 to i64
  %t130 = icmp eq i64 %t129, 0
  %t128 = zext i1 %t130 to i64
  %t131 = icmp ne i64 %t128, 0
  br i1 %t131, label %L36, label %L37
L36:
  br label %L38
L37:
  %t132 = load ptr, ptr %t85
  %t133 = call i32 @type_is_fp(ptr %t132)
  %t134 = sext i32 %t133 to i64
  %t135 = icmp ne i64 %t134, 0
  %t136 = zext i1 %t135 to i64
  br label %L38
L38:
  %t137 = phi i64 [ 1, %L36 ], [ %t136, %L37 ]
  %t138 = icmp ne i64 %t137, 0
  br i1 %t138, label %L39, label %L40
L39:
  %t139 = load ptr, ptr %t85
  %t140 = icmp ne ptr %t139, null
  br i1 %t140, label %L42, label %L43
L42:
  %t141 = load ptr, ptr %t85
  %t142 = call ptr @llvm_type(ptr %t141)
  %t143 = ptrtoint ptr %t142 to i64
  br label %L44
L43:
  %t144 = getelementptr [4 x i8], ptr @.str318, i64 0, i64 0
  %t145 = ptrtoint ptr %t144 to i64
  br label %L44
L44:
  %t146 = phi i64 [ %t143, %L42 ], [ %t145, %L43 ]
  store i64 %t146, ptr %t125
  %t147 = load ptr, ptr %t85
  store ptr %t147, ptr %t126
  br label %L41
L40:
  %t148 = load ptr, ptr %t85
  %t149 = getelementptr i8, ptr %t148, i64 0
  %t150 = load i64, ptr %t149
  %t152 = sext i32 15 to i64
  %t151 = icmp eq i64 %t150, %t152
  %t153 = zext i1 %t151 to i64
  %t154 = icmp ne i64 %t153, 0
  br i1 %t154, label %L45, label %L46
L45:
  br label %L47
L46:
  %t155 = load ptr, ptr %t85
  %t156 = getelementptr i8, ptr %t155, i64 0
  %t157 = load i64, ptr %t156
  %t159 = sext i32 16 to i64
  %t158 = icmp eq i64 %t157, %t159
  %t160 = zext i1 %t158 to i64
  %t161 = icmp ne i64 %t160, 0
  %t162 = zext i1 %t161 to i64
  br label %L47
L47:
  %t163 = phi i64 [ 1, %L45 ], [ %t162, %L46 ]
  %t164 = icmp ne i64 %t163, 0
  br i1 %t164, label %L48, label %L49
L48:
  %t165 = getelementptr [4 x i8], ptr @.str319, i64 0, i64 0
  store ptr %t165, ptr %t125
  %t166 = call ptr @default_ptr_type()
  store ptr %t166, ptr %t126
  br label %L50
L49:
  %t167 = load ptr, ptr %t85
  %t168 = getelementptr i8, ptr %t167, i64 0
  %t169 = load i64, ptr %t168
  %t171 = sext i32 18 to i64
  %t170 = icmp eq i64 %t169, %t171
  %t172 = zext i1 %t170 to i64
  %t173 = icmp ne i64 %t172, 0
  br i1 %t173, label %L51, label %L52
L51:
  br label %L53
L52:
  %t174 = load ptr, ptr %t85
  %t175 = getelementptr i8, ptr %t174, i64 0
  %t176 = load i64, ptr %t175
  %t178 = sext i32 19 to i64
  %t177 = icmp eq i64 %t176, %t178
  %t179 = zext i1 %t177 to i64
  %t180 = icmp ne i64 %t179, 0
  %t181 = zext i1 %t180 to i64
  br label %L53
L53:
  %t182 = phi i64 [ 1, %L51 ], [ %t181, %L52 ]
  %t183 = icmp ne i64 %t182, 0
  br i1 %t183, label %L54, label %L55
L54:
  br label %L56
L55:
  %t184 = load ptr, ptr %t85
  %t185 = getelementptr i8, ptr %t184, i64 0
  %t186 = load i64, ptr %t185
  %t188 = sext i32 21 to i64
  %t187 = icmp eq i64 %t186, %t188
  %t189 = zext i1 %t187 to i64
  %t190 = icmp ne i64 %t189, 0
  %t191 = zext i1 %t190 to i64
  br label %L56
L56:
  %t192 = phi i64 [ 1, %L54 ], [ %t191, %L55 ]
  %t193 = icmp ne i64 %t192, 0
  br i1 %t193, label %L57, label %L58
L57:
  %t194 = getelementptr [4 x i8], ptr @.str320, i64 0, i64 0
  store ptr %t194, ptr %t125
  %t195 = call ptr @default_ptr_type()
  store ptr %t195, ptr %t126
  br label %L59
L58:
  %t196 = getelementptr [4 x i8], ptr @.str321, i64 0, i64 0
  store ptr %t196, ptr %t125
  %t197 = call ptr @default_i64_type()
  store ptr %t197, ptr %t126
  br label %L59
L59:
  br label %L50
L50:
  br label %L41
L41:
  %t198 = alloca i64
  %t199 = call i32 @new_reg(ptr %t0)
  %t200 = sext i32 %t199 to i64
  store i64 %t200, ptr %t198
  %t201 = getelementptr i8, ptr %t0, i64 0
  %t202 = load i64, ptr %t201
  %t203 = getelementptr [9 x i8], ptr @.str322, i64 0, i64 0
  %t204 = load ptr, ptr %t125
  %t205 = load i64, ptr %t198
  call void (ptr, ...) @__c0c_emit(i64 %t202, ptr %t203, ptr %t204, i64 %t205)
  %t207 = load i64, ptr %t73
  %t209 = sext i32 %t207 to i64
  %t208 = add i64 %t209, 1
  store i64 %t208, ptr %t73
  %t210 = getelementptr i8, ptr %t1, i64 0
  %t211 = load i64, ptr %t210
  %t212 = icmp ne i64 %t211, 0
  br i1 %t212, label %L60, label %L61
L60:
  %t213 = getelementptr i8, ptr %t1, i64 0
  %t214 = load i64, ptr %t213
  %t215 = load i64, ptr %t75
  %t216 = inttoptr i64 %t214 to ptr
  %t217 = sext i32 %t215 to i64
  %t218 = getelementptr ptr, ptr %t216, i64 %t217
  %t219 = load ptr, ptr %t218
  %t220 = ptrtoint ptr %t219 to i64
  %t221 = icmp ne i64 %t220, 0
  %t222 = zext i1 %t221 to i64
  br label %L62
L61:
  br label %L62
L62:
  %t223 = phi i64 [ %t222, %L60 ], [ 0, %L61 ]
  %t224 = icmp ne i64 %t223, 0
  br i1 %t224, label %L63, label %L65
L63:
  %t225 = getelementptr i8, ptr %t0, i64 0
  %t226 = load i64, ptr %t225
  %t228 = sext i32 2048 to i64
  %t227 = icmp sge i64 %t226, %t228
  %t229 = zext i1 %t227 to i64
  %t230 = icmp ne i64 %t229, 0
  br i1 %t230, label %L66, label %L68
L66:
  %t231 = call ptr @__c0c_stderr()
  %t232 = getelementptr [22 x i8], ptr @.str323, i64 0, i64 0
  %t233 = call i32 (ptr, ...) @fprintf(ptr %t231, ptr %t232)
  %t234 = sext i32 %t233 to i64
  call void @exit(i64 1)
  br label %L68
L68:
  %t236 = alloca ptr
  %t237 = getelementptr i8, ptr %t0, i64 0
  %t238 = load i64, ptr %t237
  %t239 = getelementptr i8, ptr %t0, i64 0
  %t240 = load i64, ptr %t239
  %t241 = add i64 %t240, 1
  %t242 = getelementptr i8, ptr %t0, i64 0
  store i64 %t241, ptr %t242
  %t244 = inttoptr i64 %t238 to ptr
  %t243 = getelementptr ptr, ptr %t244, i64 %t240
  store ptr %t243, ptr %t236
  %t245 = getelementptr i8, ptr %t1, i64 0
  %t246 = load i64, ptr %t245
  %t247 = load i64, ptr %t75
  %t248 = inttoptr i64 %t246 to ptr
  %t249 = sext i32 %t247 to i64
  %t250 = getelementptr ptr, ptr %t248, i64 %t249
  %t251 = load ptr, ptr %t250
  %t252 = call ptr @strdup(ptr %t251)
  %t253 = load ptr, ptr %t236
  %t254 = getelementptr i8, ptr %t253, i64 0
  store ptr %t252, ptr %t254
  %t255 = call ptr @malloc(i64 32)
  %t256 = load ptr, ptr %t236
  %t257 = getelementptr i8, ptr %t256, i64 0
  store ptr %t255, ptr %t257
  %t258 = load ptr, ptr %t236
  %t259 = getelementptr i8, ptr %t258, i64 0
  %t260 = load i64, ptr %t259
  %t261 = getelementptr [6 x i8], ptr @.str324, i64 0, i64 0
  %t262 = load i64, ptr %t198
  %t263 = call i32 (ptr, ...) @snprintf(i64 %t260, i64 32, ptr %t261, i64 %t262)
  %t264 = sext i32 %t263 to i64
  %t265 = load ptr, ptr %t126
  %t266 = load ptr, ptr %t236
  %t267 = getelementptr i8, ptr %t266, i64 0
  store ptr %t265, ptr %t267
  %t268 = load ptr, ptr %t236
  %t269 = getelementptr i8, ptr %t268, i64 0
  %t270 = sext i32 1 to i64
  store i64 %t270, ptr %t269
  br label %L65
L65:
  br label %L21
L21:
  %t271 = load i64, ptr %t75
  %t273 = sext i32 %t271 to i64
  %t272 = add i64 %t273, 1
  store i64 %t272, ptr %t75
  br label %L19
L22:
  %t274 = load ptr, ptr %t2
  %t275 = getelementptr i8, ptr %t274, i64 0
  %t276 = load i64, ptr %t275
  %t277 = icmp ne i64 %t276, 0
  br i1 %t277, label %L69, label %L71
L69:
  %t278 = load i64, ptr %t73
  %t280 = sext i32 %t278 to i64
  %t279 = icmp ne i64 %t280, 0
  br i1 %t279, label %L72, label %L74
L72:
  %t281 = getelementptr i8, ptr %t0, i64 0
  %t282 = load i64, ptr %t281
  %t283 = getelementptr [3 x i8], ptr @.str325, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t282, ptr %t283)
  br label %L74
L74:
  %t285 = getelementptr i8, ptr %t0, i64 0
  %t286 = load i64, ptr %t285
  %t287 = getelementptr [4 x i8], ptr @.str326, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t286, ptr %t287)
  br label %L71
L71:
  %t289 = getelementptr i8, ptr %t0, i64 0
  %t290 = load i64, ptr %t289
  %t291 = getelementptr [5 x i8], ptr @.str327, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t290, ptr %t291)
  %t293 = getelementptr i8, ptr %t0, i64 0
  %t294 = load i64, ptr %t293
  %t295 = getelementptr [8 x i8], ptr @.str328, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t294, ptr %t295)
  %t297 = getelementptr i8, ptr %t1, i64 0
  %t298 = load i64, ptr %t297
  call void @emit_stmt(ptr %t0, i64 %t298)
  %t300 = load ptr, ptr %t2
  %t301 = getelementptr i8, ptr %t300, i64 0
  %t302 = load i64, ptr %t301
  %t304 = icmp eq i64 %t302, 0
  %t303 = zext i1 %t304 to i64
  %t305 = icmp ne i64 %t303, 0
  br i1 %t305, label %L75, label %L76
L75:
  br label %L77
L76:
  %t306 = load ptr, ptr %t2
  %t307 = getelementptr i8, ptr %t306, i64 0
  %t308 = load i64, ptr %t307
  %t309 = inttoptr i64 %t308 to ptr
  %t310 = getelementptr i8, ptr %t309, i64 0
  %t311 = load i64, ptr %t310
  %t313 = sext i32 0 to i64
  %t312 = icmp eq i64 %t311, %t313
  %t314 = zext i1 %t312 to i64
  %t315 = icmp ne i64 %t314, 0
  %t316 = zext i1 %t315 to i64
  br label %L77
L77:
  %t317 = phi i64 [ 1, %L75 ], [ %t316, %L76 ]
  %t318 = icmp ne i64 %t317, 0
  br i1 %t318, label %L78, label %L79
L78:
  %t319 = getelementptr i8, ptr %t0, i64 0
  %t320 = load i64, ptr %t319
  %t321 = getelementptr [12 x i8], ptr @.str329, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t320, ptr %t321)
  br label %L80
L79:
  %t323 = load ptr, ptr %t2
  %t324 = getelementptr i8, ptr %t323, i64 0
  %t325 = load i64, ptr %t324
  %t326 = inttoptr i64 %t325 to ptr
  %t327 = getelementptr i8, ptr %t326, i64 0
  %t328 = load i64, ptr %t327
  %t330 = sext i32 15 to i64
  %t329 = icmp eq i64 %t328, %t330
  %t331 = zext i1 %t329 to i64
  %t332 = icmp ne i64 %t331, 0
  br i1 %t332, label %L81, label %L82
L81:
  br label %L83
L82:
  %t333 = load ptr, ptr %t2
  %t334 = getelementptr i8, ptr %t333, i64 0
  %t335 = load i64, ptr %t334
  %t336 = inttoptr i64 %t335 to ptr
  %t337 = getelementptr i8, ptr %t336, i64 0
  %t338 = load i64, ptr %t337
  %t340 = sext i32 16 to i64
  %t339 = icmp eq i64 %t338, %t340
  %t341 = zext i1 %t339 to i64
  %t342 = icmp ne i64 %t341, 0
  %t343 = zext i1 %t342 to i64
  br label %L83
L83:
  %t344 = phi i64 [ 1, %L81 ], [ %t343, %L82 ]
  %t345 = icmp ne i64 %t344, 0
  br i1 %t345, label %L84, label %L85
L84:
  %t346 = getelementptr i8, ptr %t0, i64 0
  %t347 = load i64, ptr %t346
  %t348 = getelementptr [16 x i8], ptr @.str330, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t347, ptr %t348)
  br label %L86
L85:
  %t350 = load ptr, ptr %t2
  %t351 = getelementptr i8, ptr %t350, i64 0
  %t352 = load i64, ptr %t351
  %t353 = call i32 @type_is_fp(i64 %t352)
  %t354 = sext i32 %t353 to i64
  %t355 = icmp ne i64 %t354, 0
  br i1 %t355, label %L87, label %L88
L87:
  %t356 = getelementptr i8, ptr %t0, i64 0
  %t357 = load i64, ptr %t356
  %t358 = getelementptr [14 x i8], ptr @.str331, i64 0, i64 0
  %t359 = load ptr, ptr %t2
  %t360 = getelementptr i8, ptr %t359, i64 0
  %t361 = load i64, ptr %t360
  %t362 = call ptr @llvm_type(i64 %t361)
  call void (ptr, ...) @__c0c_emit(i64 %t357, ptr %t358, ptr %t362)
  br label %L89
L88:
  %t364 = alloca ptr
  %t365 = load ptr, ptr %t2
  %t366 = getelementptr i8, ptr %t365, i64 0
  %t367 = load i64, ptr %t366
  %t368 = call ptr @llvm_type(i64 %t367)
  store ptr %t368, ptr %t364
  %t369 = load ptr, ptr %t364
  %t370 = getelementptr [3 x i8], ptr @.str332, i64 0, i64 0
  %t371 = call i32 @strcmp(ptr %t369, ptr %t370)
  %t372 = sext i32 %t371 to i64
  %t374 = sext i32 0 to i64
  %t373 = icmp eq i64 %t372, %t374
  %t375 = zext i1 %t373 to i64
  %t376 = icmp ne i64 %t375, 0
  br i1 %t376, label %L90, label %L91
L90:
  %t377 = getelementptr i8, ptr %t0, i64 0
  %t378 = load i64, ptr %t377
  %t379 = getelementptr [12 x i8], ptr @.str333, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t378, ptr %t379)
  br label %L92
L91:
  %t381 = load ptr, ptr %t364
  %t382 = getelementptr [4 x i8], ptr @.str334, i64 0, i64 0
  %t383 = call i32 @strcmp(ptr %t381, ptr %t382)
  %t384 = sext i32 %t383 to i64
  %t386 = sext i32 0 to i64
  %t385 = icmp eq i64 %t384, %t386
  %t387 = zext i1 %t385 to i64
  %t388 = icmp ne i64 %t387, 0
  br i1 %t388, label %L93, label %L94
L93:
  %t389 = getelementptr i8, ptr %t0, i64 0
  %t390 = load i64, ptr %t389
  %t391 = getelementptr [13 x i8], ptr @.str335, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t390, ptr %t391)
  br label %L95
L94:
  %t393 = load ptr, ptr %t364
  %t394 = getelementptr [4 x i8], ptr @.str336, i64 0, i64 0
  %t395 = call i32 @strcmp(ptr %t393, ptr %t394)
  %t396 = sext i32 %t395 to i64
  %t398 = sext i32 0 to i64
  %t397 = icmp eq i64 %t396, %t398
  %t399 = zext i1 %t397 to i64
  %t400 = icmp ne i64 %t399, 0
  br i1 %t400, label %L96, label %L97
L96:
  %t401 = getelementptr i8, ptr %t0, i64 0
  %t402 = load i64, ptr %t401
  %t403 = getelementptr [13 x i8], ptr @.str337, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t402, ptr %t403)
  br label %L98
L97:
  %t405 = getelementptr i8, ptr %t0, i64 0
  %t406 = load i64, ptr %t405
  %t407 = getelementptr [13 x i8], ptr @.str338, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t406, ptr %t407)
  br label %L98
L98:
  br label %L95
L95:
  br label %L92
L92:
  br label %L89
L89:
  br label %L86
L86:
  br label %L80
L80:
  %t409 = getelementptr i8, ptr %t0, i64 0
  %t410 = load i64, ptr %t409
  %t411 = getelementptr [4 x i8], ptr @.str339, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t410, ptr %t411)
  call void @scope_pop(ptr %t0)
  ret void
}

define internal void @emit_global_var(ptr %t0, ptr %t1) {
entry:
  %t2 = getelementptr i8, ptr %t1, i64 0
  %t3 = load i64, ptr %t2
  %t5 = icmp eq i64 %t3, 0
  %t4 = zext i1 %t5 to i64
  %t6 = icmp ne i64 %t4, 0
  br i1 %t6, label %L0, label %L2
L0:
  ret void
L3:
  br label %L2
L2:
  %t7 = alloca ptr
  %t8 = getelementptr i8, ptr %t1, i64 0
  %t9 = load i64, ptr %t8
  store i64 %t9, ptr %t7
  %t10 = load ptr, ptr %t7
  %t11 = ptrtoint ptr %t10 to i64
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L4, label %L5
L4:
  %t13 = load ptr, ptr %t7
  %t14 = getelementptr i8, ptr %t13, i64 0
  %t15 = load i64, ptr %t14
  %t17 = sext i32 17 to i64
  %t16 = icmp eq i64 %t15, %t17
  %t18 = zext i1 %t16 to i64
  %t19 = icmp ne i64 %t18, 0
  %t20 = zext i1 %t19 to i64
  br label %L6
L5:
  br label %L6
L6:
  %t21 = phi i64 [ %t20, %L4 ], [ 0, %L5 ]
  %t22 = icmp ne i64 %t21, 0
  br i1 %t22, label %L7, label %L9
L7:
  %t23 = alloca i64
  %t24 = sext i32 0 to i64
  store i64 %t24, ptr %t23
  %t25 = alloca i64
  %t26 = sext i32 0 to i64
  store i64 %t26, ptr %t25
  br label %L10
L10:
  %t27 = load i64, ptr %t25
  %t28 = getelementptr i8, ptr %t0, i64 0
  %t29 = load i64, ptr %t28
  %t31 = sext i32 %t27 to i64
  %t30 = icmp slt i64 %t31, %t29
  %t32 = zext i1 %t30 to i64
  %t33 = icmp ne i64 %t32, 0
  br i1 %t33, label %L11, label %L13
L11:
  %t34 = getelementptr i8, ptr %t0, i64 0
  %t35 = load i64, ptr %t34
  %t36 = load i64, ptr %t25
  %t38 = inttoptr i64 %t35 to ptr
  %t39 = sext i32 %t36 to i64
  %t37 = getelementptr ptr, ptr %t38, i64 %t39
  %t40 = getelementptr i8, ptr %t37, i64 0
  %t41 = load i64, ptr %t40
  %t42 = getelementptr i8, ptr %t1, i64 0
  %t43 = load i64, ptr %t42
  %t44 = call i32 @strcmp(i64 %t41, i64 %t43)
  %t45 = sext i32 %t44 to i64
  %t47 = sext i32 0 to i64
  %t46 = icmp eq i64 %t45, %t47
  %t48 = zext i1 %t46 to i64
  %t49 = icmp ne i64 %t48, 0
  br i1 %t49, label %L14, label %L16
L14:
  %t50 = sext i32 1 to i64
  store i64 %t50, ptr %t23
  br label %L13
L17:
  br label %L16
L16:
  br label %L12
L12:
  %t51 = load i64, ptr %t25
  %t53 = sext i32 %t51 to i64
  %t52 = add i64 %t53, 1
  store i64 %t52, ptr %t25
  br label %L10
L13:
  %t54 = load i64, ptr %t23
  %t56 = sext i32 %t54 to i64
  %t55 = icmp ne i64 %t56, 0
  br i1 %t55, label %L18, label %L20
L18:
  ret void
L21:
  br label %L20
L20:
  %t57 = getelementptr i8, ptr %t0, i64 0
  %t58 = load i64, ptr %t57
  %t60 = sext i32 2048 to i64
  %t59 = icmp slt i64 %t58, %t60
  %t61 = zext i1 %t59 to i64
  %t62 = icmp ne i64 %t61, 0
  br i1 %t62, label %L22, label %L24
L22:
  %t63 = getelementptr i8, ptr %t1, i64 0
  %t64 = load i64, ptr %t63
  %t65 = call ptr @strdup(i64 %t64)
  %t66 = getelementptr i8, ptr %t0, i64 0
  %t67 = load i64, ptr %t66
  %t68 = getelementptr i8, ptr %t0, i64 0
  %t69 = load i64, ptr %t68
  %t71 = inttoptr i64 %t67 to ptr
  %t70 = getelementptr ptr, ptr %t71, i64 %t69
  %t72 = getelementptr i8, ptr %t70, i64 0
  store ptr %t65, ptr %t72
  %t73 = load ptr, ptr %t7
  %t74 = getelementptr i8, ptr %t0, i64 0
  %t75 = load i64, ptr %t74
  %t76 = getelementptr i8, ptr %t0, i64 0
  %t77 = load i64, ptr %t76
  %t79 = inttoptr i64 %t75 to ptr
  %t78 = getelementptr ptr, ptr %t79, i64 %t77
  %t80 = getelementptr i8, ptr %t78, i64 0
  store ptr %t73, ptr %t80
  %t81 = getelementptr i8, ptr %t0, i64 0
  %t82 = load i64, ptr %t81
  %t83 = getelementptr i8, ptr %t0, i64 0
  %t84 = load i64, ptr %t83
  %t86 = inttoptr i64 %t82 to ptr
  %t85 = getelementptr ptr, ptr %t86, i64 %t84
  %t87 = getelementptr i8, ptr %t85, i64 0
  %t88 = sext i32 1 to i64
  store i64 %t88, ptr %t87
  %t89 = getelementptr i8, ptr %t0, i64 0
  %t90 = load i64, ptr %t89
  %t91 = add i64 %t90, 1
  %t92 = getelementptr i8, ptr %t0, i64 0
  store i64 %t91, ptr %t92
  br label %L24
L24:
  %t93 = alloca ptr
  %t94 = sext i32 0 to i64
  store i64 %t94, ptr %t93
  %t95 = alloca i64
  %t96 = sext i32 0 to i64
  store i64 %t96, ptr %t95
  %t97 = alloca i64
  %t98 = sext i32 0 to i64
  store i64 %t98, ptr %t97
  br label %L25
L25:
  %t99 = load i64, ptr %t97
  %t100 = load ptr, ptr %t7
  %t101 = getelementptr i8, ptr %t100, i64 0
  %t102 = load i64, ptr %t101
  %t104 = sext i32 %t99 to i64
  %t103 = icmp slt i64 %t104, %t102
  %t105 = zext i1 %t103 to i64
  %t106 = icmp ne i64 %t105, 0
  br i1 %t106, label %L29, label %L30
L29:
  %t107 = load i64, ptr %t95
  %t109 = sext i32 %t107 to i64
  %t110 = sext i32 480 to i64
  %t108 = icmp slt i64 %t109, %t110
  %t111 = zext i1 %t108 to i64
  %t112 = icmp ne i64 %t111, 0
  %t113 = zext i1 %t112 to i64
  br label %L31
L30:
  br label %L31
L31:
  %t114 = phi i64 [ %t113, %L29 ], [ 0, %L30 ]
  %t115 = icmp ne i64 %t114, 0
  br i1 %t115, label %L26, label %L28
L26:
  %t116 = alloca ptr
  %t117 = load ptr, ptr %t7
  %t118 = getelementptr i8, ptr %t117, i64 0
  %t119 = load i64, ptr %t118
  %t120 = load i64, ptr %t97
  %t122 = inttoptr i64 %t119 to ptr
  %t123 = sext i32 %t120 to i64
  %t121 = getelementptr ptr, ptr %t122, i64 %t123
  %t124 = getelementptr i8, ptr %t121, i64 0
  %t125 = load i64, ptr %t124
  store i64 %t125, ptr %t116
  %t126 = load ptr, ptr %t116
  %t127 = ptrtoint ptr %t126 to i64
  %t128 = icmp ne i64 %t127, 0
  br i1 %t128, label %L32, label %L33
L32:
  %t129 = load ptr, ptr %t116
  %t130 = getelementptr i8, ptr %t129, i64 0
  %t131 = load i64, ptr %t130
  %t133 = sext i32 0 to i64
  %t132 = icmp eq i64 %t131, %t133
  %t134 = zext i1 %t132 to i64
  %t135 = icmp ne i64 %t134, 0
  %t136 = zext i1 %t135 to i64
  br label %L34
L33:
  br label %L34
L34:
  %t137 = phi i64 [ %t136, %L32 ], [ 0, %L33 ]
  %t138 = icmp ne i64 %t137, 0
  br i1 %t138, label %L35, label %L36
L35:
  %t139 = load ptr, ptr %t7
  %t140 = getelementptr i8, ptr %t139, i64 0
  %t141 = load i64, ptr %t140
  %t143 = sext i32 1 to i64
  %t142 = icmp eq i64 %t141, %t143
  %t144 = zext i1 %t142 to i64
  %t145 = icmp ne i64 %t144, 0
  %t146 = zext i1 %t145 to i64
  br label %L37
L36:
  br label %L37
L37:
  %t147 = phi i64 [ %t146, %L35 ], [ 0, %L36 ]
  %t148 = icmp ne i64 %t147, 0
  br i1 %t148, label %L38, label %L40
L38:
  br label %L28
L41:
  br label %L40
L40:
  %t149 = load i64, ptr %t97
  %t151 = sext i32 %t149 to i64
  %t150 = icmp ne i64 %t151, 0
  br i1 %t150, label %L42, label %L44
L42:
  %t152 = load i64, ptr %t95
  %t153 = load ptr, ptr %t93
  %t154 = load i64, ptr %t95
  %t156 = ptrtoint ptr %t153 to i64
  %t157 = sext i32 %t154 to i64
  %t158 = inttoptr i64 %t156 to ptr
  %t155 = getelementptr i8, ptr %t158, i64 %t157
  %t159 = load i64, ptr %t95
  %t161 = sext i32 512 to i64
  %t162 = sext i32 %t159 to i64
  %t160 = sub i64 %t161, %t162
  %t163 = getelementptr [3 x i8], ptr @.str340, i64 0, i64 0
  %t164 = call i32 (ptr, ...) @snprintf(ptr %t155, i64 %t160, ptr %t163)
  %t165 = sext i32 %t164 to i64
  %t167 = sext i32 %t152 to i64
  %t166 = add i64 %t167, %t165
  store i64 %t166, ptr %t95
  br label %L44
L44:
  %t168 = alloca ptr
  %t169 = load ptr, ptr %t116
  %t171 = ptrtoint ptr %t169 to i64
  %t172 = icmp eq i64 %t171, 0
  %t170 = zext i1 %t172 to i64
  %t173 = icmp ne i64 %t170, 0
  br i1 %t173, label %L45, label %L46
L45:
  br label %L47
L46:
  %t174 = load ptr, ptr %t116
  %t175 = call i32 @type_is_fp(ptr %t174)
  %t176 = sext i32 %t175 to i64
  %t177 = icmp ne i64 %t176, 0
  %t178 = zext i1 %t177 to i64
  br label %L47
L47:
  %t179 = phi i64 [ 1, %L45 ], [ %t178, %L46 ]
  %t180 = icmp ne i64 %t179, 0
  br i1 %t180, label %L48, label %L49
L48:
  %t181 = load ptr, ptr %t116
  %t182 = icmp ne ptr %t181, null
  br i1 %t182, label %L51, label %L52
L51:
  %t183 = load ptr, ptr %t116
  %t184 = call ptr @llvm_type(ptr %t183)
  %t185 = ptrtoint ptr %t184 to i64
  br label %L53
L52:
  %t186 = getelementptr [4 x i8], ptr @.str341, i64 0, i64 0
  %t187 = ptrtoint ptr %t186 to i64
  br label %L53
L53:
  %t188 = phi i64 [ %t185, %L51 ], [ %t187, %L52 ]
  store i64 %t188, ptr %t168
  br label %L50
L49:
  %t189 = load ptr, ptr %t116
  %t190 = getelementptr i8, ptr %t189, i64 0
  %t191 = load i64, ptr %t190
  %t193 = sext i32 15 to i64
  %t192 = icmp eq i64 %t191, %t193
  %t194 = zext i1 %t192 to i64
  %t195 = icmp ne i64 %t194, 0
  br i1 %t195, label %L54, label %L55
L54:
  br label %L56
L55:
  %t196 = load ptr, ptr %t116
  %t197 = getelementptr i8, ptr %t196, i64 0
  %t198 = load i64, ptr %t197
  %t200 = sext i32 16 to i64
  %t199 = icmp eq i64 %t198, %t200
  %t201 = zext i1 %t199 to i64
  %t202 = icmp ne i64 %t201, 0
  %t203 = zext i1 %t202 to i64
  br label %L56
L56:
  %t204 = phi i64 [ 1, %L54 ], [ %t203, %L55 ]
  %t205 = icmp ne i64 %t204, 0
  br i1 %t205, label %L57, label %L58
L57:
  %t206 = getelementptr [4 x i8], ptr @.str342, i64 0, i64 0
  store ptr %t206, ptr %t168
  br label %L59
L58:
  %t207 = load ptr, ptr %t116
  %t208 = getelementptr i8, ptr %t207, i64 0
  %t209 = load i64, ptr %t208
  %t211 = sext i32 18 to i64
  %t210 = icmp eq i64 %t209, %t211
  %t212 = zext i1 %t210 to i64
  %t213 = icmp ne i64 %t212, 0
  br i1 %t213, label %L60, label %L61
L60:
  br label %L62
L61:
  %t214 = load ptr, ptr %t116
  %t215 = getelementptr i8, ptr %t214, i64 0
  %t216 = load i64, ptr %t215
  %t218 = sext i32 19 to i64
  %t217 = icmp eq i64 %t216, %t218
  %t219 = zext i1 %t217 to i64
  %t220 = icmp ne i64 %t219, 0
  %t221 = zext i1 %t220 to i64
  br label %L62
L62:
  %t222 = phi i64 [ 1, %L60 ], [ %t221, %L61 ]
  %t223 = icmp ne i64 %t222, 0
  br i1 %t223, label %L63, label %L64
L63:
  br label %L65
L64:
  %t224 = load ptr, ptr %t116
  %t225 = getelementptr i8, ptr %t224, i64 0
  %t226 = load i64, ptr %t225
  %t228 = sext i32 21 to i64
  %t227 = icmp eq i64 %t226, %t228
  %t229 = zext i1 %t227 to i64
  %t230 = icmp ne i64 %t229, 0
  %t231 = zext i1 %t230 to i64
  br label %L65
L65:
  %t232 = phi i64 [ 1, %L63 ], [ %t231, %L64 ]
  %t233 = icmp ne i64 %t232, 0
  br i1 %t233, label %L66, label %L67
L66:
  %t234 = getelementptr [4 x i8], ptr @.str343, i64 0, i64 0
  store ptr %t234, ptr %t168
  br label %L68
L67:
  %t235 = getelementptr [4 x i8], ptr @.str344, i64 0, i64 0
  store ptr %t235, ptr %t168
  br label %L68
L68:
  br label %L59
L59:
  br label %L50
L50:
  %t236 = load i64, ptr %t95
  %t237 = load ptr, ptr %t93
  %t238 = load i64, ptr %t95
  %t240 = ptrtoint ptr %t237 to i64
  %t241 = sext i32 %t238 to i64
  %t242 = inttoptr i64 %t240 to ptr
  %t239 = getelementptr i8, ptr %t242, i64 %t241
  %t243 = load i64, ptr %t95
  %t245 = sext i32 512 to i64
  %t246 = sext i32 %t243 to i64
  %t244 = sub i64 %t245, %t246
  %t247 = getelementptr [3 x i8], ptr @.str345, i64 0, i64 0
  %t248 = load ptr, ptr %t168
  %t249 = call i32 (ptr, ...) @snprintf(ptr %t239, i64 %t244, ptr %t247, ptr %t248)
  %t250 = sext i32 %t249 to i64
  %t252 = sext i32 %t236 to i64
  %t251 = add i64 %t252, %t250
  store i64 %t251, ptr %t95
  br label %L27
L27:
  %t253 = load i64, ptr %t97
  %t255 = sext i32 %t253 to i64
  %t254 = add i64 %t255, 1
  store i64 %t254, ptr %t97
  br label %L25
L28:
  %t256 = load ptr, ptr %t7
  %t257 = getelementptr i8, ptr %t256, i64 0
  %t258 = load i64, ptr %t257
  %t259 = icmp ne i64 %t258, 0
  br i1 %t259, label %L69, label %L71
L69:
  %t260 = load ptr, ptr %t7
  %t261 = getelementptr i8, ptr %t260, i64 0
  %t262 = load i64, ptr %t261
  %t263 = icmp ne i64 %t262, 0
  br i1 %t263, label %L72, label %L74
L72:
  %t264 = load i64, ptr %t95
  %t265 = load ptr, ptr %t93
  %t266 = load i64, ptr %t95
  %t268 = ptrtoint ptr %t265 to i64
  %t269 = sext i32 %t266 to i64
  %t270 = inttoptr i64 %t268 to ptr
  %t267 = getelementptr i8, ptr %t270, i64 %t269
  %t271 = load i64, ptr %t95
  %t273 = sext i32 512 to i64
  %t274 = sext i32 %t271 to i64
  %t272 = sub i64 %t273, %t274
  %t275 = getelementptr [3 x i8], ptr @.str346, i64 0, i64 0
  %t276 = call i32 (ptr, ...) @snprintf(ptr %t267, i64 %t272, ptr %t275)
  %t277 = sext i32 %t276 to i64
  %t279 = sext i32 %t264 to i64
  %t278 = add i64 %t279, %t277
  store i64 %t278, ptr %t95
  br label %L74
L74:
  %t280 = load i64, ptr %t95
  %t281 = load ptr, ptr %t93
  %t282 = load i64, ptr %t95
  %t284 = ptrtoint ptr %t281 to i64
  %t285 = sext i32 %t282 to i64
  %t286 = inttoptr i64 %t284 to ptr
  %t283 = getelementptr i8, ptr %t286, i64 %t285
  %t287 = load i64, ptr %t95
  %t289 = sext i32 512 to i64
  %t290 = sext i32 %t287 to i64
  %t288 = sub i64 %t289, %t290
  %t291 = getelementptr [4 x i8], ptr @.str347, i64 0, i64 0
  %t292 = call i32 (ptr, ...) @snprintf(ptr %t283, i64 %t288, ptr %t291)
  %t293 = sext i32 %t292 to i64
  %t295 = sext i32 %t280 to i64
  %t294 = add i64 %t295, %t293
  store i64 %t294, ptr %t95
  br label %L71
L71:
  %t296 = getelementptr i8, ptr %t0, i64 0
  %t297 = load i64, ptr %t296
  %t298 = getelementptr [20 x i8], ptr @.str348, i64 0, i64 0
  %t299 = load ptr, ptr %t7
  %t300 = call ptr @llvm_ret_type(ptr %t299)
  %t301 = getelementptr i8, ptr %t1, i64 0
  %t302 = load i64, ptr %t301
  %t303 = load ptr, ptr %t93
  call void (ptr, ...) @__c0c_emit(i64 %t297, ptr %t298, ptr %t300, i64 %t302, ptr %t303)
  ret void
L75:
  br label %L9
L9:
  %t305 = alloca i64
  %t306 = sext i32 0 to i64
  store i64 %t306, ptr %t305
  %t307 = alloca i64
  %t308 = sext i32 0 to i64
  store i64 %t308, ptr %t307
  br label %L76
L76:
  %t309 = load i64, ptr %t307
  %t310 = getelementptr i8, ptr %t0, i64 0
  %t311 = load i64, ptr %t310
  %t313 = sext i32 %t309 to i64
  %t312 = icmp slt i64 %t313, %t311
  %t314 = zext i1 %t312 to i64
  %t315 = icmp ne i64 %t314, 0
  br i1 %t315, label %L77, label %L79
L77:
  %t316 = getelementptr i8, ptr %t0, i64 0
  %t317 = load i64, ptr %t316
  %t318 = load i64, ptr %t307
  %t320 = inttoptr i64 %t317 to ptr
  %t321 = sext i32 %t318 to i64
  %t319 = getelementptr ptr, ptr %t320, i64 %t321
  %t322 = getelementptr i8, ptr %t319, i64 0
  %t323 = load i64, ptr %t322
  %t324 = getelementptr i8, ptr %t1, i64 0
  %t325 = load i64, ptr %t324
  %t326 = call i32 @strcmp(i64 %t323, i64 %t325)
  %t327 = sext i32 %t326 to i64
  %t329 = sext i32 0 to i64
  %t328 = icmp eq i64 %t327, %t329
  %t330 = zext i1 %t328 to i64
  %t331 = icmp ne i64 %t330, 0
  br i1 %t331, label %L80, label %L82
L80:
  %t332 = sext i32 1 to i64
  store i64 %t332, ptr %t305
  br label %L79
L83:
  br label %L82
L82:
  br label %L78
L78:
  %t333 = load i64, ptr %t307
  %t335 = sext i32 %t333 to i64
  %t334 = add i64 %t335, 1
  store i64 %t334, ptr %t307
  br label %L76
L79:
  %t336 = load i64, ptr %t305
  %t338 = sext i32 %t336 to i64
  %t339 = icmp eq i64 %t338, 0
  %t337 = zext i1 %t339 to i64
  %t340 = icmp ne i64 %t337, 0
  br i1 %t340, label %L84, label %L85
L84:
  %t341 = getelementptr i8, ptr %t0, i64 0
  %t342 = load i64, ptr %t341
  %t344 = sext i32 2048 to i64
  %t343 = icmp slt i64 %t342, %t344
  %t345 = zext i1 %t343 to i64
  %t346 = icmp ne i64 %t345, 0
  %t347 = zext i1 %t346 to i64
  br label %L86
L85:
  br label %L86
L86:
  %t348 = phi i64 [ %t347, %L84 ], [ 0, %L85 ]
  %t349 = icmp ne i64 %t348, 0
  br i1 %t349, label %L87, label %L89
L87:
  %t350 = getelementptr i8, ptr %t1, i64 0
  %t351 = load i64, ptr %t350
  %t352 = call ptr @strdup(i64 %t351)
  %t353 = getelementptr i8, ptr %t0, i64 0
  %t354 = load i64, ptr %t353
  %t355 = getelementptr i8, ptr %t0, i64 0
  %t356 = load i64, ptr %t355
  %t358 = inttoptr i64 %t354 to ptr
  %t357 = getelementptr ptr, ptr %t358, i64 %t356
  %t359 = getelementptr i8, ptr %t357, i64 0
  store ptr %t352, ptr %t359
  %t360 = load ptr, ptr %t7
  %t361 = getelementptr i8, ptr %t0, i64 0
  %t362 = load i64, ptr %t361
  %t363 = getelementptr i8, ptr %t0, i64 0
  %t364 = load i64, ptr %t363
  %t366 = inttoptr i64 %t362 to ptr
  %t365 = getelementptr ptr, ptr %t366, i64 %t364
  %t367 = getelementptr i8, ptr %t365, i64 0
  store ptr %t360, ptr %t367
  %t368 = getelementptr i8, ptr %t1, i64 0
  %t369 = load i64, ptr %t368
  %t370 = getelementptr i8, ptr %t0, i64 0
  %t371 = load i64, ptr %t370
  %t372 = getelementptr i8, ptr %t0, i64 0
  %t373 = load i64, ptr %t372
  %t375 = inttoptr i64 %t371 to ptr
  %t374 = getelementptr ptr, ptr %t375, i64 %t373
  %t376 = getelementptr i8, ptr %t374, i64 0
  store i64 %t369, ptr %t376
  %t377 = getelementptr i8, ptr %t0, i64 0
  %t378 = load i64, ptr %t377
  %t379 = add i64 %t378, 1
  %t380 = getelementptr i8, ptr %t0, i64 0
  store i64 %t379, ptr %t380
  br label %L89
L89:
  %t381 = getelementptr i8, ptr %t1, i64 0
  %t382 = load i64, ptr %t381
  %t383 = icmp ne i64 %t382, 0
  br i1 %t383, label %L90, label %L92
L90:
  %t384 = getelementptr i8, ptr %t0, i64 0
  %t385 = load i64, ptr %t384
  %t386 = getelementptr [26 x i8], ptr @.str349, i64 0, i64 0
  %t387 = getelementptr i8, ptr %t1, i64 0
  %t388 = load i64, ptr %t387
  %t389 = load ptr, ptr %t7
  %t390 = call ptr @llvm_type(ptr %t389)
  call void (ptr, ...) @__c0c_emit(i64 %t385, ptr %t386, i64 %t388, ptr %t390)
  ret void
L93:
  br label %L92
L92:
  %t392 = alloca ptr
  %t393 = getelementptr i8, ptr %t1, i64 0
  %t394 = load i64, ptr %t393
  %t395 = icmp ne i64 %t394, 0
  br i1 %t395, label %L94, label %L95
L94:
  %t396 = getelementptr [9 x i8], ptr @.str350, i64 0, i64 0
  %t397 = ptrtoint ptr %t396 to i64
  br label %L96
L95:
  %t398 = getelementptr [10 x i8], ptr @.str351, i64 0, i64 0
  %t399 = ptrtoint ptr %t398 to i64
  br label %L96
L96:
  %t400 = phi i64 [ %t397, %L94 ], [ %t399, %L95 ]
  store i64 %t400, ptr %t392
  %t401 = alloca ptr
  %t402 = load ptr, ptr %t7
  %t403 = call ptr @llvm_type(ptr %t402)
  store ptr %t403, ptr %t401
  %t404 = getelementptr i8, ptr %t0, i64 0
  %t405 = load i64, ptr %t404
  %t406 = getelementptr [36 x i8], ptr @.str352, i64 0, i64 0
  %t407 = getelementptr i8, ptr %t1, i64 0
  %t408 = load i64, ptr %t407
  %t409 = load ptr, ptr %t392
  %t410 = load ptr, ptr %t401
  call void (ptr, ...) @__c0c_emit(i64 %t405, ptr %t406, i64 %t408, ptr %t409, ptr %t410)
  ret void
}

define dso_local ptr @codegen_new(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t3 = call ptr @calloc(i64 1, i64 0)
  store ptr %t3, ptr %t2
  %t4 = load ptr, ptr %t2
  %t6 = ptrtoint ptr %t4 to i64
  %t7 = icmp eq i64 %t6, 0
  %t5 = zext i1 %t7 to i64
  %t8 = icmp ne i64 %t5, 0
  br i1 %t8, label %L0, label %L2
L0:
  %t9 = getelementptr [7 x i8], ptr @.str353, i64 0, i64 0
  call void @perror(ptr %t9)
  call void @exit(i64 1)
  br label %L2
L2:
  %t12 = load ptr, ptr %t2
  %t13 = getelementptr i8, ptr %t12, i64 0
  store ptr %t0, ptr %t13
  %t14 = load ptr, ptr %t2
  %t15 = getelementptr i8, ptr %t14, i64 0
  store ptr %t1, ptr %t15
  %t16 = load ptr, ptr %t2
  ret ptr %t16
L3:
  ret ptr null
}

define dso_local void @codegen_free(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = sext i32 0 to i64
  store i64 %t2, ptr %t1
  br label %L0
L0:
  %t3 = load i64, ptr %t1
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t7 = sext i32 %t3 to i64
  %t6 = icmp slt i64 %t7, %t5
  %t8 = zext i1 %t6 to i64
  %t9 = icmp ne i64 %t8, 0
  br i1 %t9, label %L1, label %L3
L1:
  %t10 = getelementptr i8, ptr %t0, i64 0
  %t11 = load i64, ptr %t10
  %t12 = load i64, ptr %t1
  %t14 = inttoptr i64 %t11 to ptr
  %t15 = sext i32 %t12 to i64
  %t13 = getelementptr ptr, ptr %t14, i64 %t15
  %t16 = getelementptr i8, ptr %t13, i64 0
  %t17 = load i64, ptr %t16
  call void @free(i64 %t17)
  br label %L2
L2:
  %t19 = load i64, ptr %t1
  %t21 = sext i32 %t19 to i64
  %t20 = add i64 %t21, 1
  store i64 %t20, ptr %t1
  br label %L0
L3:
  %t22 = alloca i64
  %t23 = sext i32 0 to i64
  store i64 %t23, ptr %t22
  br label %L4
L4:
  %t24 = load i64, ptr %t22
  %t25 = getelementptr i8, ptr %t0, i64 0
  %t26 = load i64, ptr %t25
  %t28 = sext i32 %t24 to i64
  %t27 = icmp slt i64 %t28, %t26
  %t29 = zext i1 %t27 to i64
  %t30 = icmp ne i64 %t29, 0
  br i1 %t30, label %L5, label %L7
L5:
  %t31 = getelementptr i8, ptr %t0, i64 0
  %t32 = load i64, ptr %t31
  %t33 = load i64, ptr %t22
  %t34 = inttoptr i64 %t32 to ptr
  %t35 = sext i32 %t33 to i64
  %t36 = getelementptr ptr, ptr %t34, i64 %t35
  %t37 = load ptr, ptr %t36
  call void @free(ptr %t37)
  br label %L6
L6:
  %t39 = load i64, ptr %t22
  %t41 = sext i32 %t39 to i64
  %t40 = add i64 %t41, 1
  store i64 %t40, ptr %t22
  br label %L4
L7:
  call void @free(ptr %t0)
  ret void
}

define dso_local void @codegen_emit(ptr %t0, ptr %t1) {
entry:
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
  %t6 = getelementptr i8, ptr %t0, i64 0
  %t7 = load i64, ptr %t6
  %t8 = getelementptr [19 x i8], ptr @.str354, i64 0, i64 0
  %t9 = getelementptr i8, ptr %t0, i64 0
  %t10 = load i64, ptr %t9
  call void (ptr, ...) @__c0c_emit(i64 %t7, ptr %t8, i64 %t10)
  %t12 = getelementptr i8, ptr %t0, i64 0
  %t13 = load i64, ptr %t12
  %t14 = getelementptr [24 x i8], ptr @.str355, i64 0, i64 0
  %t15 = getelementptr i8, ptr %t0, i64 0
  %t16 = load i64, ptr %t15
  call void (ptr, ...) @__c0c_emit(i64 %t13, ptr %t14, i64 %t16)
  %t18 = getelementptr i8, ptr %t0, i64 0
  %t19 = load i64, ptr %t18
  %t20 = getelementptr [57 x i8], ptr @.str356, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t19, ptr %t20)
  %t22 = getelementptr i8, ptr %t0, i64 0
  %t23 = load i64, ptr %t22
  %t24 = getelementptr [45 x i8], ptr @.str357, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t23, ptr %t24)
  %t26 = getelementptr i8, ptr %t0, i64 0
  %t27 = load i64, ptr %t26
  %t28 = getelementptr [23 x i8], ptr @.str358, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t27, ptr %t28)
  %t30 = getelementptr i8, ptr %t0, i64 0
  %t31 = load i64, ptr %t30
  %t32 = getelementptr [26 x i8], ptr @.str359, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t31, ptr %t32)
  %t34 = getelementptr i8, ptr %t0, i64 0
  %t35 = load i64, ptr %t34
  %t36 = getelementptr [31 x i8], ptr @.str360, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t35, ptr %t36)
  %t38 = getelementptr i8, ptr %t0, i64 0
  %t39 = load i64, ptr %t38
  %t40 = getelementptr [32 x i8], ptr @.str361, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t39, ptr %t40)
  %t42 = getelementptr i8, ptr %t0, i64 0
  %t43 = load i64, ptr %t42
  %t44 = getelementptr [25 x i8], ptr @.str362, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t43, ptr %t44)
  %t46 = getelementptr i8, ptr %t0, i64 0
  %t47 = load i64, ptr %t46
  %t48 = getelementptr [26 x i8], ptr @.str363, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t47, ptr %t48)
  %t50 = getelementptr i8, ptr %t0, i64 0
  %t51 = load i64, ptr %t50
  %t52 = getelementptr [26 x i8], ptr @.str364, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t51, ptr %t52)
  %t54 = getelementptr i8, ptr %t0, i64 0
  %t55 = load i64, ptr %t54
  %t56 = getelementptr [32 x i8], ptr @.str365, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t55, ptr %t56)
  %t58 = getelementptr i8, ptr %t0, i64 0
  %t59 = load i64, ptr %t58
  %t60 = getelementptr [31 x i8], ptr @.str366, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t59, ptr %t60)
  %t62 = getelementptr i8, ptr %t0, i64 0
  %t63 = load i64, ptr %t62
  %t64 = getelementptr [37 x i8], ptr @.str367, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t63, ptr %t64)
  %t66 = getelementptr i8, ptr %t0, i64 0
  %t67 = load i64, ptr %t66
  %t68 = getelementptr [31 x i8], ptr @.str368, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t67, ptr %t68)
  %t70 = getelementptr i8, ptr %t0, i64 0
  %t71 = load i64, ptr %t70
  %t72 = getelementptr [31 x i8], ptr @.str369, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t71, ptr %t72)
  %t74 = getelementptr i8, ptr %t0, i64 0
  %t75 = load i64, ptr %t74
  %t76 = getelementptr [31 x i8], ptr @.str370, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t75, ptr %t76)
  %t78 = getelementptr i8, ptr %t0, i64 0
  %t79 = load i64, ptr %t78
  %t80 = getelementptr [31 x i8], ptr @.str371, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t79, ptr %t80)
  %t82 = getelementptr i8, ptr %t0, i64 0
  %t83 = load i64, ptr %t82
  %t84 = getelementptr [37 x i8], ptr @.str372, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t83, ptr %t84)
  %t86 = getelementptr i8, ptr %t0, i64 0
  %t87 = load i64, ptr %t86
  %t88 = getelementptr [36 x i8], ptr @.str373, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t87, ptr %t88)
  %t90 = getelementptr i8, ptr %t0, i64 0
  %t91 = load i64, ptr %t90
  %t92 = getelementptr [36 x i8], ptr @.str374, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t91, ptr %t92)
  %t94 = getelementptr i8, ptr %t0, i64 0
  %t95 = load i64, ptr %t94
  %t96 = getelementptr [36 x i8], ptr @.str375, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t95, ptr %t96)
  %t98 = getelementptr i8, ptr %t0, i64 0
  %t99 = load i64, ptr %t98
  %t100 = getelementptr [31 x i8], ptr @.str376, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t99, ptr %t100)
  %t102 = getelementptr i8, ptr %t0, i64 0
  %t103 = load i64, ptr %t102
  %t104 = getelementptr [37 x i8], ptr @.str377, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t103, ptr %t104)
  %t106 = getelementptr i8, ptr %t0, i64 0
  %t107 = load i64, ptr %t106
  %t108 = getelementptr [37 x i8], ptr @.str378, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t107, ptr %t108)
  %t110 = getelementptr i8, ptr %t0, i64 0
  %t111 = load i64, ptr %t110
  %t112 = getelementptr [43 x i8], ptr @.str379, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t111, ptr %t112)
  %t114 = getelementptr i8, ptr %t0, i64 0
  %t115 = load i64, ptr %t114
  %t116 = getelementptr [38 x i8], ptr @.str380, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t115, ptr %t116)
  %t118 = getelementptr i8, ptr %t0, i64 0
  %t119 = load i64, ptr %t118
  %t120 = getelementptr [44 x i8], ptr @.str381, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t119, ptr %t120)
  %t122 = getelementptr i8, ptr %t0, i64 0
  %t123 = load i64, ptr %t122
  %t124 = getelementptr [30 x i8], ptr @.str382, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t123, ptr %t124)
  %t126 = getelementptr i8, ptr %t0, i64 0
  %t127 = load i64, ptr %t126
  %t128 = getelementptr [26 x i8], ptr @.str383, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t127, ptr %t128)
  %t130 = getelementptr i8, ptr %t0, i64 0
  %t131 = load i64, ptr %t130
  %t132 = getelementptr [40 x i8], ptr @.str384, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t131, ptr %t132)
  %t134 = getelementptr i8, ptr %t0, i64 0
  %t135 = load i64, ptr %t134
  %t136 = getelementptr [41 x i8], ptr @.str385, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t135, ptr %t136)
  %t138 = getelementptr i8, ptr %t0, i64 0
  %t139 = load i64, ptr %t138
  %t140 = getelementptr [35 x i8], ptr @.str386, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t139, ptr %t140)
  %t142 = getelementptr i8, ptr %t0, i64 0
  %t143 = load i64, ptr %t142
  %t144 = getelementptr [25 x i8], ptr @.str387, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t143, ptr %t144)
  %t146 = getelementptr i8, ptr %t0, i64 0
  %t147 = load i64, ptr %t146
  %t148 = getelementptr [27 x i8], ptr @.str388, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t147, ptr %t148)
  %t150 = getelementptr i8, ptr %t0, i64 0
  %t151 = load i64, ptr %t150
  %t152 = getelementptr [25 x i8], ptr @.str389, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t151, ptr %t152)
  %t154 = getelementptr i8, ptr %t0, i64 0
  %t155 = load i64, ptr %t154
  %t156 = getelementptr [26 x i8], ptr @.str390, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t155, ptr %t156)
  %t158 = getelementptr i8, ptr %t0, i64 0
  %t159 = load i64, ptr %t158
  %t160 = getelementptr [24 x i8], ptr @.str391, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t159, ptr %t160)
  %t162 = getelementptr i8, ptr %t0, i64 0
  %t163 = load i64, ptr %t162
  %t164 = getelementptr [24 x i8], ptr @.str392, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t163, ptr %t164)
  %t166 = getelementptr i8, ptr %t0, i64 0
  %t167 = load i64, ptr %t166
  %t168 = getelementptr [36 x i8], ptr @.str393, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t167, ptr %t168)
  %t170 = getelementptr i8, ptr %t0, i64 0
  %t171 = load i64, ptr %t170
  %t172 = getelementptr [37 x i8], ptr @.str394, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t171, ptr %t172)
  %t174 = getelementptr i8, ptr %t0, i64 0
  %t175 = load i64, ptr %t174
  %t176 = getelementptr [27 x i8], ptr @.str395, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t175, ptr %t176)
  %t178 = getelementptr i8, ptr %t0, i64 0
  %t179 = load i64, ptr %t178
  %t180 = getelementptr [27 x i8], ptr @.str396, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t179, ptr %t180)
  %t182 = getelementptr i8, ptr %t0, i64 0
  %t183 = load i64, ptr %t182
  %t184 = getelementptr [27 x i8], ptr @.str397, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t183, ptr %t184)
  %t186 = getelementptr i8, ptr %t0, i64 0
  %t187 = load i64, ptr %t186
  %t188 = getelementptr [27 x i8], ptr @.str398, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t187, ptr %t188)
  %t190 = getelementptr i8, ptr %t0, i64 0
  %t191 = load i64, ptr %t190
  %t192 = getelementptr [27 x i8], ptr @.str399, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t191, ptr %t192)
  %t194 = getelementptr i8, ptr %t0, i64 0
  %t195 = load i64, ptr %t194
  %t196 = getelementptr [28 x i8], ptr @.str400, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t195, ptr %t196)
  %t198 = getelementptr i8, ptr %t0, i64 0
  %t199 = load i64, ptr %t198
  %t200 = getelementptr [27 x i8], ptr @.str401, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t199, ptr %t200)
  %t202 = getelementptr i8, ptr %t0, i64 0
  %t203 = load i64, ptr %t202
  %t204 = getelementptr [27 x i8], ptr @.str402, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t203, ptr %t204)
  %t206 = getelementptr i8, ptr %t0, i64 0
  %t207 = load i64, ptr %t206
  %t208 = getelementptr [27 x i8], ptr @.str403, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t207, ptr %t208)
  %t210 = getelementptr i8, ptr %t0, i64 0
  %t211 = load i64, ptr %t210
  %t212 = getelementptr [27 x i8], ptr @.str404, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t211, ptr %t212)
  %t214 = getelementptr i8, ptr %t0, i64 0
  %t215 = load i64, ptr %t214
  %t216 = getelementptr [26 x i8], ptr @.str405, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t215, ptr %t216)
  %t218 = getelementptr i8, ptr %t0, i64 0
  %t219 = load i64, ptr %t218
  %t220 = getelementptr [29 x i8], ptr @.str406, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t219, ptr %t220)
  %t222 = getelementptr i8, ptr %t0, i64 0
  %t223 = load i64, ptr %t222
  %t224 = getelementptr [29 x i8], ptr @.str407, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t223, ptr %t224)
  %t226 = getelementptr i8, ptr %t0, i64 0
  %t227 = load i64, ptr %t226
  %t228 = getelementptr [28 x i8], ptr @.str408, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t227, ptr %t228)
  %t230 = getelementptr i8, ptr %t0, i64 0
  %t231 = load i64, ptr %t230
  %t232 = getelementptr [34 x i8], ptr @.str409, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t231, ptr %t232)
  %t234 = getelementptr i8, ptr %t0, i64 0
  %t235 = load i64, ptr %t234
  %t236 = getelementptr [37 x i8], ptr @.str410, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t235, ptr %t236)
  %t238 = getelementptr i8, ptr %t0, i64 0
  %t239 = load i64, ptr %t238
  %t240 = getelementptr [37 x i8], ptr @.str411, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t239, ptr %t240)
  %t242 = getelementptr i8, ptr %t0, i64 0
  %t243 = load i64, ptr %t242
  %t244 = getelementptr [41 x i8], ptr @.str412, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t243, ptr %t244)
  %t246 = getelementptr i8, ptr %t0, i64 0
  %t247 = load i64, ptr %t246
  %t248 = getelementptr [2 x i8], ptr @.str413, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t247, ptr %t248)
  %t250 = alloca ptr
  %t251 = sext i32 0 to i64
  store i64 %t251, ptr %t250
  %t252 = alloca i64
  %t253 = sext i32 0 to i64
  store i64 %t253, ptr %t252
  br label %L4
L4:
  %t254 = load ptr, ptr %t250
  %t255 = load i64, ptr %t252
  %t256 = sext i32 %t255 to i64
  %t257 = getelementptr ptr, ptr %t254, i64 %t256
  %t258 = load ptr, ptr %t257
  %t259 = icmp ne ptr %t258, null
  br i1 %t259, label %L5, label %L7
L5:
  %t260 = alloca i64
  %t261 = sext i32 0 to i64
  store i64 %t261, ptr %t260
  %t262 = alloca i64
  %t263 = sext i32 0 to i64
  store i64 %t263, ptr %t262
  br label %L8
L8:
  %t264 = load i64, ptr %t262
  %t265 = getelementptr i8, ptr %t0, i64 0
  %t266 = load i64, ptr %t265
  %t268 = sext i32 %t264 to i64
  %t267 = icmp slt i64 %t268, %t266
  %t269 = zext i1 %t267 to i64
  %t270 = icmp ne i64 %t269, 0
  br i1 %t270, label %L9, label %L11
L9:
  %t271 = getelementptr i8, ptr %t0, i64 0
  %t272 = load i64, ptr %t271
  %t273 = load i64, ptr %t262
  %t275 = inttoptr i64 %t272 to ptr
  %t276 = sext i32 %t273 to i64
  %t274 = getelementptr ptr, ptr %t275, i64 %t276
  %t277 = getelementptr i8, ptr %t274, i64 0
  %t278 = load i64, ptr %t277
  %t279 = load ptr, ptr %t250
  %t280 = load i64, ptr %t252
  %t281 = sext i32 %t280 to i64
  %t282 = getelementptr ptr, ptr %t279, i64 %t281
  %t283 = load ptr, ptr %t282
  %t284 = call i32 @strcmp(i64 %t278, ptr %t283)
  %t285 = sext i32 %t284 to i64
  %t287 = sext i32 0 to i64
  %t286 = icmp eq i64 %t285, %t287
  %t288 = zext i1 %t286 to i64
  %t289 = icmp ne i64 %t288, 0
  br i1 %t289, label %L12, label %L14
L12:
  %t290 = sext i32 1 to i64
  store i64 %t290, ptr %t260
  br label %L11
L15:
  br label %L14
L14:
  br label %L10
L10:
  %t291 = load i64, ptr %t262
  %t293 = sext i32 %t291 to i64
  %t292 = add i64 %t293, 1
  store i64 %t292, ptr %t262
  br label %L8
L11:
  %t294 = load i64, ptr %t260
  %t296 = sext i32 %t294 to i64
  %t297 = icmp eq i64 %t296, 0
  %t295 = zext i1 %t297 to i64
  %t298 = icmp ne i64 %t295, 0
  br i1 %t298, label %L16, label %L17
L16:
  %t299 = getelementptr i8, ptr %t0, i64 0
  %t300 = load i64, ptr %t299
  %t302 = sext i32 2048 to i64
  %t301 = icmp slt i64 %t300, %t302
  %t303 = zext i1 %t301 to i64
  %t304 = icmp ne i64 %t303, 0
  %t305 = zext i1 %t304 to i64
  br label %L18
L17:
  br label %L18
L18:
  %t306 = phi i64 [ %t305, %L16 ], [ 0, %L17 ]
  %t307 = icmp ne i64 %t306, 0
  br i1 %t307, label %L19, label %L21
L19:
  %t308 = load ptr, ptr %t250
  %t309 = load i64, ptr %t252
  %t310 = sext i32 %t309 to i64
  %t311 = getelementptr ptr, ptr %t308, i64 %t310
  %t312 = load ptr, ptr %t311
  %t313 = call ptr @strdup(ptr %t312)
  %t314 = getelementptr i8, ptr %t0, i64 0
  %t315 = load i64, ptr %t314
  %t316 = getelementptr i8, ptr %t0, i64 0
  %t317 = load i64, ptr %t316
  %t319 = inttoptr i64 %t315 to ptr
  %t318 = getelementptr ptr, ptr %t319, i64 %t317
  %t320 = getelementptr i8, ptr %t318, i64 0
  store ptr %t313, ptr %t320
  %t322 = sext i32 0 to i64
  %t321 = inttoptr i64 %t322 to ptr
  %t323 = getelementptr i8, ptr %t0, i64 0
  %t324 = load i64, ptr %t323
  %t325 = getelementptr i8, ptr %t0, i64 0
  %t326 = load i64, ptr %t325
  %t328 = inttoptr i64 %t324 to ptr
  %t327 = getelementptr ptr, ptr %t328, i64 %t326
  %t329 = getelementptr i8, ptr %t327, i64 0
  store ptr %t321, ptr %t329
  %t330 = getelementptr i8, ptr %t0, i64 0
  %t331 = load i64, ptr %t330
  %t332 = getelementptr i8, ptr %t0, i64 0
  %t333 = load i64, ptr %t332
  %t335 = inttoptr i64 %t331 to ptr
  %t334 = getelementptr ptr, ptr %t335, i64 %t333
  %t336 = getelementptr i8, ptr %t334, i64 0
  %t337 = sext i32 1 to i64
  store i64 %t337, ptr %t336
  %t338 = getelementptr i8, ptr %t0, i64 0
  %t339 = load i64, ptr %t338
  %t340 = add i64 %t339, 1
  %t341 = getelementptr i8, ptr %t0, i64 0
  store i64 %t340, ptr %t341
  br label %L21
L21:
  br label %L6
L6:
  %t342 = load i64, ptr %t252
  %t344 = sext i32 %t342 to i64
  %t343 = add i64 %t344, 1
  store i64 %t343, ptr %t252
  br label %L4
L7:
  %t345 = alloca i64
  %t346 = sext i32 0 to i64
  store i64 %t346, ptr %t345
  br label %L22
L22:
  %t347 = load i64, ptr %t345
  %t348 = getelementptr i8, ptr %t1, i64 0
  %t349 = load i64, ptr %t348
  %t351 = sext i32 %t347 to i64
  %t350 = icmp slt i64 %t351, %t349
  %t352 = zext i1 %t350 to i64
  %t353 = icmp ne i64 %t352, 0
  br i1 %t353, label %L23, label %L25
L23:
  %t354 = alloca ptr
  %t355 = getelementptr i8, ptr %t1, i64 0
  %t356 = load i64, ptr %t355
  %t357 = load i64, ptr %t345
  %t358 = inttoptr i64 %t356 to ptr
  %t359 = sext i32 %t357 to i64
  %t360 = getelementptr ptr, ptr %t358, i64 %t359
  %t361 = load ptr, ptr %t360
  store ptr %t361, ptr %t354
  %t362 = load ptr, ptr %t354
  %t364 = ptrtoint ptr %t362 to i64
  %t365 = icmp eq i64 %t364, 0
  %t363 = zext i1 %t365 to i64
  %t366 = icmp ne i64 %t363, 0
  br i1 %t366, label %L26, label %L28
L26:
  br label %L24
L29:
  br label %L28
L28:
  %t367 = load ptr, ptr %t354
  %t368 = getelementptr i8, ptr %t367, i64 0
  %t369 = load i64, ptr %t368
  %t371 = sext i32 1 to i64
  %t370 = icmp eq i64 %t369, %t371
  %t372 = zext i1 %t370 to i64
  %t373 = icmp ne i64 %t372, 0
  br i1 %t373, label %L30, label %L32
L30:
  %t374 = alloca i64
  %t375 = sext i32 0 to i64
  store i64 %t375, ptr %t374
  %t376 = alloca i64
  %t377 = sext i32 0 to i64
  store i64 %t377, ptr %t376
  br label %L33
L33:
  %t378 = load i64, ptr %t376
  %t379 = getelementptr i8, ptr %t0, i64 0
  %t380 = load i64, ptr %t379
  %t382 = sext i32 %t378 to i64
  %t381 = icmp slt i64 %t382, %t380
  %t383 = zext i1 %t381 to i64
  %t384 = icmp ne i64 %t383, 0
  br i1 %t384, label %L34, label %L36
L34:
  %t385 = getelementptr i8, ptr %t0, i64 0
  %t386 = load i64, ptr %t385
  %t387 = load i64, ptr %t376
  %t389 = inttoptr i64 %t386 to ptr
  %t390 = sext i32 %t387 to i64
  %t388 = getelementptr ptr, ptr %t389, i64 %t390
  %t391 = getelementptr i8, ptr %t388, i64 0
  %t392 = load i64, ptr %t391
  %t393 = load ptr, ptr %t354
  %t394 = getelementptr i8, ptr %t393, i64 0
  %t395 = load i64, ptr %t394
  %t396 = icmp ne i64 %t395, 0
  br i1 %t396, label %L37, label %L38
L37:
  %t397 = load ptr, ptr %t354
  %t398 = getelementptr i8, ptr %t397, i64 0
  %t399 = load i64, ptr %t398
  br label %L39
L38:
  %t400 = getelementptr [1 x i8], ptr @.str414, i64 0, i64 0
  %t401 = ptrtoint ptr %t400 to i64
  br label %L39
L39:
  %t402 = phi i64 [ %t399, %L37 ], [ %t401, %L38 ]
  %t403 = call i32 @strcmp(i64 %t392, i64 %t402)
  %t404 = sext i32 %t403 to i64
  %t406 = sext i32 0 to i64
  %t405 = icmp eq i64 %t404, %t406
  %t407 = zext i1 %t405 to i64
  %t408 = icmp ne i64 %t407, 0
  br i1 %t408, label %L40, label %L42
L40:
  %t409 = sext i32 1 to i64
  store i64 %t409, ptr %t374
  br label %L36
L43:
  br label %L42
L42:
  br label %L35
L35:
  %t410 = load i64, ptr %t376
  %t412 = sext i32 %t410 to i64
  %t411 = add i64 %t412, 1
  store i64 %t411, ptr %t376
  br label %L33
L36:
  %t413 = load i64, ptr %t374
  %t415 = sext i32 %t413 to i64
  %t416 = icmp eq i64 %t415, 0
  %t414 = zext i1 %t416 to i64
  %t417 = icmp ne i64 %t414, 0
  br i1 %t417, label %L44, label %L45
L44:
  %t418 = getelementptr i8, ptr %t0, i64 0
  %t419 = load i64, ptr %t418
  %t421 = sext i32 2048 to i64
  %t420 = icmp slt i64 %t419, %t421
  %t422 = zext i1 %t420 to i64
  %t423 = icmp ne i64 %t422, 0
  %t424 = zext i1 %t423 to i64
  br label %L46
L45:
  br label %L46
L46:
  %t425 = phi i64 [ %t424, %L44 ], [ 0, %L45 ]
  %t426 = icmp ne i64 %t425, 0
  br i1 %t426, label %L47, label %L49
L47:
  %t427 = load ptr, ptr %t354
  %t428 = getelementptr i8, ptr %t427, i64 0
  %t429 = load i64, ptr %t428
  %t430 = icmp ne i64 %t429, 0
  br i1 %t430, label %L50, label %L51
L50:
  %t431 = load ptr, ptr %t354
  %t432 = getelementptr i8, ptr %t431, i64 0
  %t433 = load i64, ptr %t432
  br label %L52
L51:
  %t434 = getelementptr [7 x i8], ptr @.str415, i64 0, i64 0
  %t435 = ptrtoint ptr %t434 to i64
  br label %L52
L52:
  %t436 = phi i64 [ %t433, %L50 ], [ %t435, %L51 ]
  %t437 = call ptr @strdup(i64 %t436)
  %t438 = getelementptr i8, ptr %t0, i64 0
  %t439 = load i64, ptr %t438
  %t440 = getelementptr i8, ptr %t0, i64 0
  %t441 = load i64, ptr %t440
  %t443 = inttoptr i64 %t439 to ptr
  %t442 = getelementptr ptr, ptr %t443, i64 %t441
  %t444 = getelementptr i8, ptr %t442, i64 0
  store ptr %t437, ptr %t444
  %t445 = load ptr, ptr %t354
  %t446 = getelementptr i8, ptr %t445, i64 0
  %t447 = load i64, ptr %t446
  %t448 = getelementptr i8, ptr %t0, i64 0
  %t449 = load i64, ptr %t448
  %t450 = getelementptr i8, ptr %t0, i64 0
  %t451 = load i64, ptr %t450
  %t453 = inttoptr i64 %t449 to ptr
  %t452 = getelementptr ptr, ptr %t453, i64 %t451
  %t454 = getelementptr i8, ptr %t452, i64 0
  store i64 %t447, ptr %t454
  %t455 = getelementptr i8, ptr %t0, i64 0
  %t456 = load i64, ptr %t455
  %t457 = getelementptr i8, ptr %t0, i64 0
  %t458 = load i64, ptr %t457
  %t460 = inttoptr i64 %t456 to ptr
  %t459 = getelementptr ptr, ptr %t460, i64 %t458
  %t461 = getelementptr i8, ptr %t459, i64 0
  %t462 = sext i32 0 to i64
  store i64 %t462, ptr %t461
  %t463 = getelementptr i8, ptr %t0, i64 0
  %t464 = load i64, ptr %t463
  %t465 = add i64 %t464, 1
  %t466 = getelementptr i8, ptr %t0, i64 0
  store i64 %t465, ptr %t466
  br label %L49
L49:
  br label %L32
L32:
  br label %L24
L24:
  %t467 = load i64, ptr %t345
  %t469 = sext i32 %t467 to i64
  %t468 = add i64 %t469, 1
  store i64 %t468, ptr %t345
  br label %L22
L25:
  %t470 = alloca i64
  %t471 = sext i32 0 to i64
  store i64 %t471, ptr %t470
  br label %L53
L53:
  %t472 = load i64, ptr %t470
  %t473 = getelementptr i8, ptr %t1, i64 0
  %t474 = load i64, ptr %t473
  %t476 = sext i32 %t472 to i64
  %t475 = icmp slt i64 %t476, %t474
  %t477 = zext i1 %t475 to i64
  %t478 = icmp ne i64 %t477, 0
  br i1 %t478, label %L54, label %L56
L54:
  %t479 = alloca ptr
  %t480 = getelementptr i8, ptr %t1, i64 0
  %t481 = load i64, ptr %t480
  %t482 = load i64, ptr %t470
  %t483 = inttoptr i64 %t481 to ptr
  %t484 = sext i32 %t482 to i64
  %t485 = getelementptr ptr, ptr %t483, i64 %t484
  %t486 = load ptr, ptr %t485
  store ptr %t486, ptr %t479
  %t487 = load ptr, ptr %t479
  %t489 = ptrtoint ptr %t487 to i64
  %t490 = icmp eq i64 %t489, 0
  %t488 = zext i1 %t490 to i64
  %t491 = icmp ne i64 %t488, 0
  br i1 %t491, label %L57, label %L59
L57:
  br label %L55
L60:
  br label %L59
L59:
  %t492 = load ptr, ptr %t479
  %t493 = getelementptr i8, ptr %t492, i64 0
  %t494 = load i64, ptr %t493
  %t496 = sext i32 2 to i64
  %t495 = icmp eq i64 %t494, %t496
  %t497 = zext i1 %t495 to i64
  %t498 = icmp ne i64 %t497, 0
  br i1 %t498, label %L61, label %L63
L61:
  %t499 = load ptr, ptr %t479
  call void @emit_global_var(ptr %t0, ptr %t499)
  br label %L63
L63:
  br label %L55
L55:
  %t501 = load i64, ptr %t470
  %t503 = sext i32 %t501 to i64
  %t502 = add i64 %t503, 1
  store i64 %t502, ptr %t470
  br label %L53
L56:
  %t504 = getelementptr i8, ptr %t0, i64 0
  %t505 = load i64, ptr %t504
  %t506 = getelementptr [2 x i8], ptr @.str416, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t505, ptr %t506)
  %t508 = alloca i64
  %t509 = sext i32 0 to i64
  store i64 %t509, ptr %t508
  br label %L64
L64:
  %t510 = load i64, ptr %t508
  %t511 = getelementptr i8, ptr %t1, i64 0
  %t512 = load i64, ptr %t511
  %t514 = sext i32 %t510 to i64
  %t513 = icmp slt i64 %t514, %t512
  %t515 = zext i1 %t513 to i64
  %t516 = icmp ne i64 %t515, 0
  br i1 %t516, label %L65, label %L67
L65:
  %t517 = alloca ptr
  %t518 = getelementptr i8, ptr %t1, i64 0
  %t519 = load i64, ptr %t518
  %t520 = load i64, ptr %t508
  %t521 = inttoptr i64 %t519 to ptr
  %t522 = sext i32 %t520 to i64
  %t523 = getelementptr ptr, ptr %t521, i64 %t522
  %t524 = load ptr, ptr %t523
  store ptr %t524, ptr %t517
  %t525 = load ptr, ptr %t517
  %t527 = ptrtoint ptr %t525 to i64
  %t528 = icmp eq i64 %t527, 0
  %t526 = zext i1 %t528 to i64
  %t529 = icmp ne i64 %t526, 0
  br i1 %t529, label %L68, label %L70
L68:
  br label %L66
L71:
  br label %L70
L70:
  %t530 = load ptr, ptr %t517
  %t531 = getelementptr i8, ptr %t530, i64 0
  %t532 = load i64, ptr %t531
  %t534 = sext i32 1 to i64
  %t533 = icmp eq i64 %t532, %t534
  %t535 = zext i1 %t533 to i64
  %t536 = icmp ne i64 %t535, 0
  br i1 %t536, label %L72, label %L74
L72:
  %t537 = load ptr, ptr %t517
  call void @emit_func_def(ptr %t0, ptr %t537)
  br label %L74
L74:
  br label %L66
L66:
  %t539 = load i64, ptr %t508
  %t541 = sext i32 %t539 to i64
  %t540 = add i64 %t541, 1
  store i64 %t540, ptr %t508
  br label %L64
L67:
  %t542 = alloca i64
  %t543 = sext i32 0 to i64
  store i64 %t543, ptr %t542
  br label %L75
L75:
  %t544 = load i64, ptr %t542
  %t545 = getelementptr i8, ptr %t0, i64 0
  %t546 = load i64, ptr %t545
  %t548 = sext i32 %t544 to i64
  %t547 = icmp slt i64 %t548, %t546
  %t549 = zext i1 %t547 to i64
  %t550 = icmp ne i64 %t549, 0
  br i1 %t550, label %L76, label %L78
L76:
  %t551 = alloca i64
  %t552 = getelementptr i8, ptr %t0, i64 0
  %t553 = load i64, ptr %t552
  %t554 = load i64, ptr %t542
  %t555 = inttoptr i64 %t553 to ptr
  %t556 = sext i32 %t554 to i64
  %t557 = getelementptr ptr, ptr %t555, i64 %t556
  %t558 = load ptr, ptr %t557
  %t559 = call i32 @str_literal_len(ptr %t558)
  %t560 = sext i32 %t559 to i64
  store i64 %t560, ptr %t551
  %t561 = getelementptr i8, ptr %t0, i64 0
  %t562 = load i64, ptr %t561
  %t563 = getelementptr [53 x i8], ptr @.str417, i64 0, i64 0
  %t564 = getelementptr i8, ptr %t0, i64 0
  %t565 = load i64, ptr %t564
  %t566 = load i64, ptr %t542
  %t567 = inttoptr i64 %t565 to ptr
  %t568 = sext i32 %t566 to i64
  %t569 = getelementptr ptr, ptr %t567, i64 %t568
  %t570 = load ptr, ptr %t569
  %t571 = load i64, ptr %t551
  call void (ptr, ...) @__c0c_emit(i64 %t562, ptr %t563, ptr %t570, i64 %t571)
  %t573 = getelementptr i8, ptr %t0, i64 0
  %t574 = load i64, ptr %t573
  %t575 = load i64, ptr %t542
  %t576 = inttoptr i64 %t574 to ptr
  %t577 = sext i32 %t575 to i64
  %t578 = getelementptr ptr, ptr %t576, i64 %t577
  %t579 = load ptr, ptr %t578
  call void @emit_str_content(ptr %t0, ptr %t579)
  %t581 = getelementptr i8, ptr %t0, i64 0
  %t582 = load i64, ptr %t581
  %t583 = getelementptr [3 x i8], ptr @.str418, i64 0, i64 0
  call void (ptr, ...) @__c0c_emit(i64 %t582, ptr %t583)
  br label %L77
L77:
  %t585 = load i64, ptr %t542
  %t587 = sext i32 %t585 to i64
  %t586 = add i64 %t587, 1
  store i64 %t586, ptr %t542
  br label %L75
L78:
  ret void
}

@.str0 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str1 = private unnamed_addr constant [4 x i8] c"i32\00"
@.str2 = private unnamed_addr constant [5 x i8] c"void\00"
@.str3 = private unnamed_addr constant [3 x i8] c"i1\00"
@.str4 = private unnamed_addr constant [3 x i8] c"i8\00"
@.str5 = private unnamed_addr constant [4 x i8] c"i16\00"
@.str6 = private unnamed_addr constant [4 x i8] c"i32\00"
@.str7 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str8 = private unnamed_addr constant [6 x i8] c"float\00"
@.str9 = private unnamed_addr constant [7 x i8] c"double\00"
@.str10 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str11 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str12 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str13 = private unnamed_addr constant [12 x i8] c"%%struct.%s\00"
@.str14 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str15 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str16 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str17 = private unnamed_addr constant [4 x i8] c"i32\00"
@.str18 = private unnamed_addr constant [22 x i8] c"c0c: too many locals\0A\00"
@.str19 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str20 = private unnamed_addr constant [4 x i8] c"\5C0A\00"
@.str21 = private unnamed_addr constant [4 x i8] c"\5C09\00"
@.str22 = private unnamed_addr constant [4 x i8] c"\5C0D\00"
@.str23 = private unnamed_addr constant [4 x i8] c"\5C00\00"
@.str24 = private unnamed_addr constant [4 x i8] c"\5C22\00"
@.str25 = private unnamed_addr constant [4 x i8] c"\5C5C\00"
@.str26 = private unnamed_addr constant [6 x i8] c"\5C%02X\00"
@.str27 = private unnamed_addr constant [3 x i8] c"%c\00"
@.str28 = private unnamed_addr constant [4 x i8] c"\5C00\00"
@.str29 = private unnamed_addr constant [4 x i8] c"@%s\00"
@.str30 = private unnamed_addr constant [4 x i8] c"@%s\00"
@.str31 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str32 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str33 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str34 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str35 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str36 = private unnamed_addr constant [44 x i8] c"  %%t%d = getelementptr %s, ptr %s, i64 %s\0A\00"
@.str37 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str38 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str39 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str40 = private unnamed_addr constant [45 x i8] c"  %%t%d = getelementptr i8, ptr %s, i64 %ld\0A\00"
@.str41 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str42 = private unnamed_addr constant [34 x i8] c"  %%t%d = ptrtoint ptr %s to i64\0A\00"
@.str43 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str44 = private unnamed_addr constant [30 x i8] c"  %%t%d = sext i32 %s to i64\0A\00"
@.str45 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str46 = private unnamed_addr constant [32 x i8] c"  %%t%d = icmp ne ptr %s, null\0A\00"
@.str47 = private unnamed_addr constant [35 x i8] c"  %%t%d = fcmp one double %s, 0.0\0A\00"
@.str48 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp ne i64 %s, 0\0A\00"
@.str49 = private unnamed_addr constant [2 x i8] c"0\00"
@.str50 = private unnamed_addr constant [5 x i8] c"%lld\00"
@.str51 = private unnamed_addr constant [31 x i8] c"  %%t%d = fadd double 0.0, %g\0A\00"
@.str52 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str53 = private unnamed_addr constant [5 x i8] c"%lld\00"
@.str54 = private unnamed_addr constant [62 x i8] c"  %%t%d = getelementptr [%d x i8], ptr @.str%d, i64 0, i64 0\0A\00"
@.str55 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str56 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str57 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str58 = private unnamed_addr constant [27 x i8] c"  %%t%d = load %s, ptr %s\0A\00"
@.str59 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str60 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str61 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str62 = private unnamed_addr constant [28 x i8] c"  %%t%d = load %s, ptr @%s\0A\00"
@.str63 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str64 = private unnamed_addr constant [4 x i8] c"@%s\00"
@.str65 = private unnamed_addr constant [4 x i8] c"@%s\00"
@.str66 = private unnamed_addr constant [27 x i8] c"  call void (ptr, ...) %s(\00"
@.str67 = private unnamed_addr constant [16 x i8] c"  call void %s(\00"
@.str68 = private unnamed_addr constant [33 x i8] c"  %%t%d = call %s (ptr, ...) %s(\00"
@.str69 = private unnamed_addr constant [22 x i8] c"  %%t%d = call %s %s(\00"
@.str70 = private unnamed_addr constant [3 x i8] c", \00"
@.str71 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str72 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str73 = private unnamed_addr constant [6 x i8] c"%s %s\00"
@.str74 = private unnamed_addr constant [3 x i8] c")\0A\00"
@.str75 = private unnamed_addr constant [2 x i8] c"0\00"
@.str76 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str77 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str78 = private unnamed_addr constant [32 x i8] c"  %%t%d = sext %s %%t%d to i64\0A\00"
@.str79 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str80 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp ne i64 %s, 0\0A\00"
@.str81 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str82 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str83 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp ne i64 %s, 0\0A\00"
@.str84 = private unnamed_addr constant [32 x i8] c"  %%t%d = zext i1 %%t%d to i64\0A\00"
@.str85 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str86 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str87 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str88 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str89 = private unnamed_addr constant [50 x i8] c"  %%t%d = phi i64 [ %%t%d, %%L%d ], [ 0, %%L%d ]\0A\00"
@.str90 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str91 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp ne i64 %s, 0\0A\00"
@.str92 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str93 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str94 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str95 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str96 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp ne i64 %s, 0\0A\00"
@.str97 = private unnamed_addr constant [32 x i8] c"  %%t%d = zext i1 %%t%d to i64\0A\00"
@.str98 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str99 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str100 = private unnamed_addr constant [50 x i8] c"  %%t%d = phi i64 [ 1, %%L%d ], [ %%t%d, %%L%d ]\0A\00"
@.str101 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str102 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str103 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str104 = private unnamed_addr constant [35 x i8] c"  %%t%d = sitofp i64 %s to double\0A\00"
@.str105 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str106 = private unnamed_addr constant [5 x i8] c"fadd\00"
@.str107 = private unnamed_addr constant [14 x i8] c"getelementptr\00"
@.str108 = private unnamed_addr constant [4 x i8] c"add\00"
@.str109 = private unnamed_addr constant [5 x i8] c"fsub\00"
@.str110 = private unnamed_addr constant [4 x i8] c"sub\00"
@.str111 = private unnamed_addr constant [5 x i8] c"fmul\00"
@.str112 = private unnamed_addr constant [4 x i8] c"mul\00"
@.str113 = private unnamed_addr constant [5 x i8] c"fdiv\00"
@.str114 = private unnamed_addr constant [5 x i8] c"sdiv\00"
@.str115 = private unnamed_addr constant [5 x i8] c"frem\00"
@.str116 = private unnamed_addr constant [5 x i8] c"srem\00"
@.str117 = private unnamed_addr constant [4 x i8] c"and\00"
@.str118 = private unnamed_addr constant [3 x i8] c"or\00"
@.str119 = private unnamed_addr constant [4 x i8] c"xor\00"
@.str120 = private unnamed_addr constant [4 x i8] c"shl\00"
@.str121 = private unnamed_addr constant [5 x i8] c"ashr\00"
@.str122 = private unnamed_addr constant [9 x i8] c"fcmp oeq\00"
@.str123 = private unnamed_addr constant [8 x i8] c"icmp eq\00"
@.str124 = private unnamed_addr constant [9 x i8] c"fcmp one\00"
@.str125 = private unnamed_addr constant [8 x i8] c"icmp ne\00"
@.str126 = private unnamed_addr constant [9 x i8] c"fcmp olt\00"
@.str127 = private unnamed_addr constant [9 x i8] c"icmp slt\00"
@.str128 = private unnamed_addr constant [9 x i8] c"fcmp ogt\00"
@.str129 = private unnamed_addr constant [9 x i8] c"icmp sgt\00"
@.str130 = private unnamed_addr constant [9 x i8] c"fcmp ole\00"
@.str131 = private unnamed_addr constant [9 x i8] c"icmp sle\00"
@.str132 = private unnamed_addr constant [9 x i8] c"fcmp oge\00"
@.str133 = private unnamed_addr constant [9 x i8] c"icmp sge\00"
@.str134 = private unnamed_addr constant [4 x i8] c"add\00"
@.str135 = private unnamed_addr constant [4 x i8] c"add\00"
@.str136 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str137 = private unnamed_addr constant [47 x i8] c"  %%t%d = getelementptr i8, ptr %%t%d, i64 %s\0A\00"
@.str138 = private unnamed_addr constant [24 x i8] c"  %%t%d = %s %s %s, %s\0A\00"
@.str139 = private unnamed_addr constant [32 x i8] c"  %%t%d = zext i1 %%t%d to i64\0A\00"
@.str140 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str141 = private unnamed_addr constant [24 x i8] c"  %%t%d = %s %s %s, %s\0A\00"
@.str142 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str143 = private unnamed_addr constant [26 x i8] c"  %%t%d = fneg double %s\0A\00"
@.str144 = private unnamed_addr constant [25 x i8] c"  %%t%d = sub i64 0, %s\0A\00"
@.str145 = private unnamed_addr constant [29 x i8] c"  %%t%d = icmp eq i64 %s, 0\0A\00"
@.str146 = private unnamed_addr constant [32 x i8] c"  %%t%d = zext i1 %%t%d to i64\0A\00"
@.str147 = private unnamed_addr constant [26 x i8] c"  %%t%d = xor i64 %s, -1\0A\00"
@.str148 = private unnamed_addr constant [25 x i8] c"  %%t%d = add i64 %s, 0\0A\00"
@.str149 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str150 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str151 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str152 = private unnamed_addr constant [30 x i8] c"  %%t%d = sext i32 %s to i64\0A\00"
@.str153 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str154 = private unnamed_addr constant [23 x i8] c"  store %s %s, ptr %s\0A\00"
@.str155 = private unnamed_addr constant [7 x i8] c"double\00"
@.str156 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str157 = private unnamed_addr constant [5 x i8] c"fadd\00"
@.str158 = private unnamed_addr constant [4 x i8] c"add\00"
@.str159 = private unnamed_addr constant [5 x i8] c"fsub\00"
@.str160 = private unnamed_addr constant [4 x i8] c"sub\00"
@.str161 = private unnamed_addr constant [5 x i8] c"fmul\00"
@.str162 = private unnamed_addr constant [4 x i8] c"mul\00"
@.str163 = private unnamed_addr constant [5 x i8] c"fdiv\00"
@.str164 = private unnamed_addr constant [5 x i8] c"sdiv\00"
@.str165 = private unnamed_addr constant [5 x i8] c"srem\00"
@.str166 = private unnamed_addr constant [4 x i8] c"and\00"
@.str167 = private unnamed_addr constant [3 x i8] c"or\00"
@.str168 = private unnamed_addr constant [4 x i8] c"xor\00"
@.str169 = private unnamed_addr constant [4 x i8] c"shl\00"
@.str170 = private unnamed_addr constant [5 x i8] c"ashr\00"
@.str171 = private unnamed_addr constant [4 x i8] c"add\00"
@.str172 = private unnamed_addr constant [24 x i8] c"  %%t%d = %s %s %s, %s\0A\00"
@.str173 = private unnamed_addr constant [26 x i8] c"  store %s %%t%d, ptr %s\0A\00"
@.str174 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str175 = private unnamed_addr constant [4 x i8] c"add\00"
@.str176 = private unnamed_addr constant [4 x i8] c"sub\00"
@.str177 = private unnamed_addr constant [24 x i8] c"  %%t%d = %s i64 %s, 1\0A\00"
@.str178 = private unnamed_addr constant [27 x i8] c"  store i64 %%t%d, ptr %s\0A\00"
@.str179 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str180 = private unnamed_addr constant [4 x i8] c"add\00"
@.str181 = private unnamed_addr constant [4 x i8] c"sub\00"
@.str182 = private unnamed_addr constant [24 x i8] c"  %%t%d = %s i64 %s, 1\0A\00"
@.str183 = private unnamed_addr constant [27 x i8] c"  store i64 %%t%d, ptr %s\0A\00"
@.str184 = private unnamed_addr constant [5 x i8] c"null\00"
@.str185 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str186 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str187 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str188 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str189 = private unnamed_addr constant [27 x i8] c"  %%t%d = load %s, ptr %s\0A\00"
@.str190 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str191 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str192 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str193 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str194 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str195 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str196 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str197 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str198 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str199 = private unnamed_addr constant [44 x i8] c"  %%t%d = getelementptr %s, ptr %s, i64 %s\0A\00"
@.str200 = private unnamed_addr constant [30 x i8] c"  %%t%d = load %s, ptr %%t%d\0A\00"
@.str201 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str202 = private unnamed_addr constant [36 x i8] c"  %%t%d = fpext float %s to double\0A\00"
@.str203 = private unnamed_addr constant [38 x i8] c"  %%t%d = fptrunc double %s to float\0A\00"
@.str204 = private unnamed_addr constant [35 x i8] c"  %%t%d = fptosi double %s to i64\0A\00"
@.str205 = private unnamed_addr constant [31 x i8] c"  %%t%d = sitofp i64 %s to %s\0A\00"
@.str206 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str207 = private unnamed_addr constant [34 x i8] c"  %%t%d = ptrtoint ptr %s to i64\0A\00"
@.str208 = private unnamed_addr constant [33 x i8] c"  %%t%d = bitcast ptr %s to ptr\0A\00"
@.str209 = private unnamed_addr constant [25 x i8] c"  %%t%d = add i64 %s, 0\0A\00"
@.str210 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str211 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str212 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str213 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str214 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str215 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str216 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str217 = private unnamed_addr constant [48 x i8] c"  %%t%d = phi i64 [ %s, %%L%d ], [ %s, %%L%d ]\0A\00"
@.str218 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str219 = private unnamed_addr constant [3 x i8] c"%d\00"
@.str220 = private unnamed_addr constant [3 x i8] c"%d\00"
@.str221 = private unnamed_addr constant [2 x i8] c"0\00"
@.str222 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str223 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str224 = private unnamed_addr constant [45 x i8] c"  %%t%d = getelementptr i8, ptr %s, i64 %ld\0A\00"
@.str225 = private unnamed_addr constant [43 x i8] c"  %%t%d = getelementptr i8, ptr %s, i64 0\0A\00"
@.str226 = private unnamed_addr constant [31 x i8] c"  %%t%d = load i64, ptr %%t%d\0A\00"
@.str227 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str228 = private unnamed_addr constant [28 x i8] c"  ; unhandled expr node %d\0A\00"
@.str229 = private unnamed_addr constant [2 x i8] c"0\00"
@.str230 = private unnamed_addr constant [10 x i8] c"[%d x i8]\00"
@.str231 = private unnamed_addr constant [10 x i8] c"[%d x i8]\00"
@.str232 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str233 = private unnamed_addr constant [7 x i8] c"double\00"
@.str234 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str235 = private unnamed_addr constant [21 x i8] c"  %%t%d = alloca %s\0A\00"
@.str236 = private unnamed_addr constant [22 x i8] c"c0c: too many locals\0A\00"
@.str237 = private unnamed_addr constant [7 x i8] c"__anon\00"
@.str238 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str239 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str240 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str241 = private unnamed_addr constant [30 x i8] c"  %%t%d = sext i32 %s to i64\0A\00"
@.str242 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str243 = private unnamed_addr constant [26 x i8] c"  store %s %s, ptr %%t%d\0A\00"
@.str244 = private unnamed_addr constant [12 x i8] c"  ret void\0A\00"
@.str245 = private unnamed_addr constant [13 x i8] c"  ret %s %s\0A\00"
@.str246 = private unnamed_addr constant [14 x i8] c"  ret ptr %s\0A\00"
@.str247 = private unnamed_addr constant [34 x i8] c"  %%t%d = inttoptr i64 %s to ptr\0A\00"
@.str248 = private unnamed_addr constant [17 x i8] c"  ret ptr %%t%d\0A\00"
@.str249 = private unnamed_addr constant [3 x i8] c"i8\00"
@.str250 = private unnamed_addr constant [30 x i8] c"  %%t%d = trunc i64 %s to i8\0A\00"
@.str251 = private unnamed_addr constant [16 x i8] c"  ret i8 %%t%d\0A\00"
@.str252 = private unnamed_addr constant [4 x i8] c"i16\00"
@.str253 = private unnamed_addr constant [31 x i8] c"  %%t%d = trunc i64 %s to i16\0A\00"
@.str254 = private unnamed_addr constant [17 x i8] c"  ret i16 %%t%d\0A\00"
@.str255 = private unnamed_addr constant [4 x i8] c"i32\00"
@.str256 = private unnamed_addr constant [31 x i8] c"  %%t%d = trunc i64 %s to i32\0A\00"
@.str257 = private unnamed_addr constant [17 x i8] c"  ret i32 %%t%d\0A\00"
@.str258 = private unnamed_addr constant [14 x i8] c"  ret i64 %s\0A\00"
@.str259 = private unnamed_addr constant [12 x i8] c"  ret void\0A\00"
@.str260 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str261 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str262 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str263 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str264 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str265 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str266 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str267 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str268 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str269 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str270 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str271 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str272 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str273 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str274 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str275 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str276 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str277 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str278 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str279 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str280 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str281 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str282 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str283 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str284 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str285 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str286 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str287 = private unnamed_addr constant [41 x i8] c"  br i1 %%t%d, label %%L%d, label %%L%d\0A\00"
@.str288 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str289 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str290 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str291 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str292 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str293 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str294 = private unnamed_addr constant [17 x i8] c"  br label %%%s\0A\00"
@.str295 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str296 = private unnamed_addr constant [17 x i8] c"  br label %%%s\0A\00"
@.str297 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str298 = private unnamed_addr constant [4 x i8] c"L%d\00"
@.str299 = private unnamed_addr constant [25 x i8] c"  %%t%d = add i64 %s, 0\0A\00"
@.str300 = private unnamed_addr constant [35 x i8] c"  switch i64 %%t%d, label %%L%d [\0A\00"
@.str301 = private unnamed_addr constant [27 x i8] c"    i64 %lld, label %%L%d\0A\00"
@.str302 = private unnamed_addr constant [5 x i8] c"  ]\0A\00"
@.str303 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str304 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str305 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str306 = private unnamed_addr constant [18 x i8] c"  br label %%L%d\0A\00"
@.str307 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str308 = private unnamed_addr constant [17 x i8] c"  br label %%%s\0A\00"
@.str309 = private unnamed_addr constant [5 x i8] c"%s:\0A\00"
@.str310 = private unnamed_addr constant [17 x i8] c"  br label %%%s\0A\00"
@.str311 = private unnamed_addr constant [6 x i8] c"L%d:\0A\00"
@.str312 = private unnamed_addr constant [5 x i8] c"anon\00"
@.str313 = private unnamed_addr constant [9 x i8] c"internal\00"
@.str314 = private unnamed_addr constant [10 x i8] c"dso_local\00"
@.str315 = private unnamed_addr constant [18 x i8] c"define %s %s @%s(\00"
@.str316 = private unnamed_addr constant [5 x i8] c"anon\00"
@.str317 = private unnamed_addr constant [3 x i8] c", \00"
@.str318 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str319 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str320 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str321 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str322 = private unnamed_addr constant [9 x i8] c"%s %%t%d\00"
@.str323 = private unnamed_addr constant [22 x i8] c"c0c: too many locals\0A\00"
@.str324 = private unnamed_addr constant [6 x i8] c"%%t%d\00"
@.str325 = private unnamed_addr constant [3 x i8] c", \00"
@.str326 = private unnamed_addr constant [4 x i8] c"...\00"
@.str327 = private unnamed_addr constant [5 x i8] c") {\0A\00"
@.str328 = private unnamed_addr constant [8 x i8] c"entry:\0A\00"
@.str329 = private unnamed_addr constant [12 x i8] c"  ret void\0A\00"
@.str330 = private unnamed_addr constant [16 x i8] c"  ret ptr null\0A\00"
@.str331 = private unnamed_addr constant [14 x i8] c"  ret %s 0.0\0A\00"
@.str332 = private unnamed_addr constant [3 x i8] c"i8\00"
@.str333 = private unnamed_addr constant [12 x i8] c"  ret i8 0\0A\00"
@.str334 = private unnamed_addr constant [4 x i8] c"i16\00"
@.str335 = private unnamed_addr constant [13 x i8] c"  ret i16 0\0A\00"
@.str336 = private unnamed_addr constant [4 x i8] c"i32\00"
@.str337 = private unnamed_addr constant [13 x i8] c"  ret i32 0\0A\00"
@.str338 = private unnamed_addr constant [13 x i8] c"  ret i64 0\0A\00"
@.str339 = private unnamed_addr constant [4 x i8] c"}\0A\0A\00"
@.str340 = private unnamed_addr constant [3 x i8] c", \00"
@.str341 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str342 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str343 = private unnamed_addr constant [4 x i8] c"ptr\00"
@.str344 = private unnamed_addr constant [4 x i8] c"i64\00"
@.str345 = private unnamed_addr constant [3 x i8] c"%s\00"
@.str346 = private unnamed_addr constant [3 x i8] c", \00"
@.str347 = private unnamed_addr constant [4 x i8] c"...\00"
@.str348 = private unnamed_addr constant [20 x i8] c"declare %s @%s(%s)\0A\00"
@.str349 = private unnamed_addr constant [26 x i8] c"@%s = external global %s\0A\00"
@.str350 = private unnamed_addr constant [9 x i8] c"internal\00"
@.str351 = private unnamed_addr constant [10 x i8] c"dso_local\00"
@.str352 = private unnamed_addr constant [36 x i8] c"@%s = %s global %s zeroinitializer\0A\00"
@.str353 = private unnamed_addr constant [7 x i8] c"calloc\00"
@.str354 = private unnamed_addr constant [19 x i8] c"; ModuleID = '%s'\0A\00"
@.str355 = private unnamed_addr constant [24 x i8] c"source_filename = \22%s\22\0A\00"
@.str356 = private unnamed_addr constant [57 x i8] c"target datalayout = \22e-m:o-i64:64-i128:128-n32:64-S128\22\0A\00"
@.str357 = private unnamed_addr constant [45 x i8] c"target triple = \22arm64-apple-macosx15.0.0\22\0A\0A\00"
@.str358 = private unnamed_addr constant [23 x i8] c"; stdlib declarations\0A\00"
@.str359 = private unnamed_addr constant [26 x i8] c"declare ptr @malloc(i64)\0A\00"
@.str360 = private unnamed_addr constant [31 x i8] c"declare ptr @calloc(i64, i64)\0A\00"
@.str361 = private unnamed_addr constant [32 x i8] c"declare ptr @realloc(ptr, i64)\0A\00"
@.str362 = private unnamed_addr constant [25 x i8] c"declare void @free(ptr)\0A\00"
@.str363 = private unnamed_addr constant [26 x i8] c"declare i64 @strlen(ptr)\0A\00"
@.str364 = private unnamed_addr constant [26 x i8] c"declare ptr @strdup(ptr)\0A\00"
@.str365 = private unnamed_addr constant [32 x i8] c"declare ptr @strndup(ptr, i64)\0A\00"
@.str366 = private unnamed_addr constant [31 x i8] c"declare ptr @strcpy(ptr, ptr)\0A\00"
@.str367 = private unnamed_addr constant [37 x i8] c"declare ptr @strncpy(ptr, ptr, i64)\0A\00"
@.str368 = private unnamed_addr constant [31 x i8] c"declare ptr @strcat(ptr, ptr)\0A\00"
@.str369 = private unnamed_addr constant [31 x i8] c"declare ptr @strchr(ptr, i64)\0A\00"
@.str370 = private unnamed_addr constant [31 x i8] c"declare ptr @strstr(ptr, ptr)\0A\00"
@.str371 = private unnamed_addr constant [31 x i8] c"declare i32 @strcmp(ptr, ptr)\0A\00"
@.str372 = private unnamed_addr constant [37 x i8] c"declare i32 @strncmp(ptr, ptr, i64)\0A\00"
@.str373 = private unnamed_addr constant [36 x i8] c"declare ptr @memcpy(ptr, ptr, i64)\0A\00"
@.str374 = private unnamed_addr constant [36 x i8] c"declare ptr @memset(ptr, i32, i64)\0A\00"
@.str375 = private unnamed_addr constant [36 x i8] c"declare i32 @memcmp(ptr, ptr, i64)\0A\00"
@.str376 = private unnamed_addr constant [31 x i8] c"declare i32 @printf(ptr, ...)\0A\00"
@.str377 = private unnamed_addr constant [37 x i8] c"declare i32 @fprintf(ptr, ptr, ...)\0A\00"
@.str378 = private unnamed_addr constant [37 x i8] c"declare i32 @sprintf(ptr, ptr, ...)\0A\00"
@.str379 = private unnamed_addr constant [43 x i8] c"declare i32 @snprintf(ptr, i64, ptr, ...)\0A\00"
@.str380 = private unnamed_addr constant [38 x i8] c"declare i32 @vfprintf(ptr, ptr, ptr)\0A\00"
@.str381 = private unnamed_addr constant [44 x i8] c"declare i32 @vsnprintf(ptr, i64, ptr, ptr)\0A\00"
@.str382 = private unnamed_addr constant [30 x i8] c"declare ptr @fopen(ptr, ptr)\0A\00"
@.str383 = private unnamed_addr constant [26 x i8] c"declare i32 @fclose(ptr)\0A\00"
@.str384 = private unnamed_addr constant [40 x i8] c"declare i64 @fread(ptr, i64, i64, ptr)\0A\00"
@.str385 = private unnamed_addr constant [41 x i8] c"declare i64 @fwrite(ptr, i64, i64, ptr)\0A\00"
@.str386 = private unnamed_addr constant [35 x i8] c"declare i32 @fseek(ptr, i64, i32)\0A\00"
@.str387 = private unnamed_addr constant [25 x i8] c"declare i64 @ftell(ptr)\0A\00"
@.str388 = private unnamed_addr constant [27 x i8] c"declare void @perror(ptr)\0A\00"
@.str389 = private unnamed_addr constant [25 x i8] c"declare void @exit(i32)\0A\00"
@.str390 = private unnamed_addr constant [26 x i8] c"declare ptr @getenv(ptr)\0A\00"
@.str391 = private unnamed_addr constant [24 x i8] c"declare i32 @atoi(ptr)\0A\00"
@.str392 = private unnamed_addr constant [24 x i8] c"declare i64 @atol(ptr)\0A\00"
@.str393 = private unnamed_addr constant [36 x i8] c"declare i64 @strtol(ptr, ptr, i32)\0A\00"
@.str394 = private unnamed_addr constant [37 x i8] c"declare i64 @strtoll(ptr, ptr, i32)\0A\00"
@.str395 = private unnamed_addr constant [27 x i8] c"declare double @atof(ptr)\0A\00"
@.str396 = private unnamed_addr constant [27 x i8] c"declare i32 @isspace(i32)\0A\00"
@.str397 = private unnamed_addr constant [27 x i8] c"declare i32 @isdigit(i32)\0A\00"
@.str398 = private unnamed_addr constant [27 x i8] c"declare i32 @isalpha(i32)\0A\00"
@.str399 = private unnamed_addr constant [27 x i8] c"declare i32 @isalnum(i32)\0A\00"
@.str400 = private unnamed_addr constant [28 x i8] c"declare i32 @isxdigit(i32)\0A\00"
@.str401 = private unnamed_addr constant [27 x i8] c"declare i32 @isupper(i32)\0A\00"
@.str402 = private unnamed_addr constant [27 x i8] c"declare i32 @islower(i32)\0A\00"
@.str403 = private unnamed_addr constant [27 x i8] c"declare i32 @toupper(i32)\0A\00"
@.str404 = private unnamed_addr constant [27 x i8] c"declare i32 @tolower(i32)\0A\00"
@.str405 = private unnamed_addr constant [26 x i8] c"declare i32 @assert(i32)\0A\00"
@.str406 = private unnamed_addr constant [29 x i8] c"declare ptr @__c0c_stderr()\0A\00"
@.str407 = private unnamed_addr constant [29 x i8] c"declare ptr @__c0c_stdout()\0A\00"
@.str408 = private unnamed_addr constant [28 x i8] c"declare ptr @__c0c_stdin()\0A\00"
@.str409 = private unnamed_addr constant [34 x i8] c"declare ptr @__c0c_get_tbuf(i32)\0A\00"
@.str410 = private unnamed_addr constant [37 x i8] c"declare ptr @__c0c_get_td_name(i64)\0A\00"
@.str411 = private unnamed_addr constant [37 x i8] c"declare i64 @__c0c_get_td_kind(i64)\0A\00"
@.str412 = private unnamed_addr constant [41 x i8] c"declare void @__c0c_emit(ptr, ptr, ...)\0A\00"
@.str413 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str414 = private unnamed_addr constant [1 x i8] c"\00"
@.str415 = private unnamed_addr constant [7 x i8] c"__anon\00"
@.str416 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str417 = private unnamed_addr constant [53 x i8] c"@.str%d = private unnamed_addr constant [%d x i8] c\22\00"
@.str418 = private unnamed_addr constant [3 x i8] c"\22\0A\00"
