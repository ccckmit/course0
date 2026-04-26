; ModuleID = 'main.c'
source_filename = "main.c"
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

declare ptr @macro_preprocess(ptr, ptr, i64)
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
declare ptr @parser_new(ptr)
declare void @parser_free(ptr)
declare ptr @parser_parse(ptr)
declare ptr @codegen_new(ptr, ptr)
declare void @codegen_free(ptr)
declare void @codegen_emit(ptr, ptr)

define internal ptr @read_file(ptr %t0) {
entry:
  %t1 = alloca ptr
  %t2 = getelementptr [2 x i8], ptr @.str0, i64 0, i64 0
  %t3 = call ptr @fopen(ptr %t0, ptr %t2)
  store ptr %t3, ptr %t1
  %t4 = load ptr, ptr %t1
  %t6 = ptrtoint ptr %t4 to i64
  %t7 = icmp eq i64 %t6, 0
  %t5 = zext i1 %t7 to i64
  %t8 = icmp ne i64 %t5, 0
  br i1 %t8, label %L0, label %L2
L0:
  %t9 = call ptr @__c0c_stderr()
  %t10 = getelementptr [23 x i8], ptr @.str1, i64 0, i64 0
  %t11 = call i32 (ptr, ...) @fprintf(ptr %t9, ptr %t10, ptr %t0)
  %t12 = sext i32 %t11 to i64
  call void @exit(i64 1)
  br label %L2
L2:
  %t14 = load ptr, ptr %t1
  %t15 = call i64 @fseek(ptr %t14, i64 0, i64 2)
  %t16 = alloca i64
  %t17 = load ptr, ptr %t1
  %t18 = call i64 @ftell(ptr %t17)
  store i64 %t18, ptr %t16
  %t19 = load ptr, ptr %t1
  %t20 = call i64 @fseek(ptr %t19, i64 0, i64 0)
  %t21 = alloca ptr
  %t22 = load i64, ptr %t16
  %t24 = sext i32 2 to i64
  %t23 = add i64 %t22, %t24
  %t25 = call ptr @malloc(i64 %t23)
  store ptr %t25, ptr %t21
  %t26 = load ptr, ptr %t21
  %t28 = ptrtoint ptr %t26 to i64
  %t29 = icmp eq i64 %t28, 0
  %t27 = zext i1 %t29 to i64
  %t30 = icmp ne i64 %t27, 0
  br i1 %t30, label %L3, label %L5
L3:
  %t31 = getelementptr [7 x i8], ptr @.str2, i64 0, i64 0
  call void @perror(ptr %t31)
  call void @exit(i64 1)
  br label %L5
L5:
  %t34 = alloca i64
  %t35 = load ptr, ptr %t21
  %t36 = load i64, ptr %t16
  %t37 = load ptr, ptr %t1
  %t38 = call i64 @fread(ptr %t35, i64 1, i64 %t36, ptr %t37)
  store i64 %t38, ptr %t34
  %t39 = load ptr, ptr %t21
  %t40 = load i64, ptr %t34
  %t42 = sext i32 %t40 to i64
  %t41 = getelementptr ptr, ptr %t39, i64 %t42
  %t43 = sext i32 0 to i64
  store i64 %t43, ptr %t41
  %t44 = load ptr, ptr %t1
  %t45 = call i32 @fclose(ptr %t44)
  %t46 = sext i32 %t45 to i64
  %t47 = load ptr, ptr %t21
  ret ptr %t47
L6:
  ret ptr null
}

define internal void @usage(ptr %t0) {
entry:
  %t1 = call ptr @__c0c_stderr()
  %t2 = getelementptr [45 x i8], ptr @.str3, i64 0, i64 0
  %t3 = call i32 (ptr, ...) @fprintf(ptr %t1, ptr %t2, ptr %t0, ptr %t0)
  %t4 = sext i32 %t3 to i64
  ret void
}

define internal void @compile(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t3 = call ptr @read_file(ptr %t0)
  store ptr %t3, ptr %t2
  %t4 = alloca ptr
  %t5 = load ptr, ptr %t2
  %t6 = call ptr @macro_preprocess(ptr %t5, ptr %t0, i64 0)
  store ptr %t6, ptr %t4
  %t7 = load ptr, ptr %t2
  call void @free(ptr %t7)
  %t9 = alloca ptr
  %t10 = load ptr, ptr %t4
  %t11 = call ptr @lexer_new(ptr %t10, ptr %t0)
  store ptr %t11, ptr %t9
  %t12 = alloca ptr
  %t13 = load ptr, ptr %t9
  %t14 = call ptr @parser_new(ptr %t13)
  store ptr %t14, ptr %t12
  %t15 = alloca ptr
  %t16 = load ptr, ptr %t12
  %t17 = call ptr @parser_parse(ptr %t16)
  store ptr %t17, ptr %t15
  %t18 = alloca ptr
  %t19 = call ptr @codegen_new(ptr %t1, ptr %t0)
  store ptr %t19, ptr %t18
  %t20 = load ptr, ptr %t18
  %t21 = load ptr, ptr %t15
  call void @codegen_emit(ptr %t20, ptr %t21)
  %t23 = load ptr, ptr %t18
  call void @codegen_free(ptr %t23)
  %t25 = load ptr, ptr %t15
  call void @node_free(ptr %t25)
  %t27 = load ptr, ptr %t12
  call void @parser_free(ptr %t27)
  %t29 = load ptr, ptr %t9
  call void @lexer_free(ptr %t29)
  %t31 = load ptr, ptr %t4
  call void @free(ptr %t31)
  ret void
}

define dso_local i32 @main(i64 %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t4 = sext i32 0 to i64
  %t3 = inttoptr i64 %t4 to ptr
  store ptr %t3, ptr %t2
  %t5 = alloca ptr
  %t7 = sext i32 0 to i64
  %t6 = inttoptr i64 %t7 to ptr
  store ptr %t6, ptr %t5
  %t8 = alloca i64
  %t9 = sext i32 1 to i64
  store i64 %t9, ptr %t8
  br label %L0
L0:
  %t10 = load i64, ptr %t8
  %t12 = sext i32 %t10 to i64
  %t11 = icmp slt i64 %t12, %t0
  %t13 = zext i1 %t11 to i64
  %t14 = icmp ne i64 %t13, 0
  br i1 %t14, label %L1, label %L3
L1:
  %t15 = load i64, ptr %t8
  %t16 = sext i32 %t15 to i64
  %t17 = getelementptr ptr, ptr %t1, i64 %t16
  %t18 = load ptr, ptr %t17
  %t19 = getelementptr [3 x i8], ptr @.str4, i64 0, i64 0
  %t20 = call i32 @strcmp(ptr %t18, ptr %t19)
  %t21 = sext i32 %t20 to i64
  %t23 = sext i32 0 to i64
  %t22 = icmp eq i64 %t21, %t23
  %t24 = zext i1 %t22 to i64
  %t25 = icmp ne i64 %t24, 0
  br i1 %t25, label %L4, label %L5
L4:
  br label %L6
L5:
  %t26 = load i64, ptr %t8
  %t27 = sext i32 %t26 to i64
  %t28 = getelementptr ptr, ptr %t1, i64 %t27
  %t29 = load ptr, ptr %t28
  %t30 = getelementptr [7 x i8], ptr @.str5, i64 0, i64 0
  %t31 = call i32 @strcmp(ptr %t29, ptr %t30)
  %t32 = sext i32 %t31 to i64
  %t34 = sext i32 0 to i64
  %t33 = icmp eq i64 %t32, %t34
  %t35 = zext i1 %t33 to i64
  %t36 = icmp ne i64 %t35, 0
  %t37 = zext i1 %t36 to i64
  br label %L6
L6:
  %t38 = phi i64 [ 1, %L4 ], [ %t37, %L5 ]
  %t39 = icmp ne i64 %t38, 0
  br i1 %t39, label %L7, label %L9
L7:
  %t40 = sext i32 0 to i64
  %t41 = getelementptr ptr, ptr %t1, i64 %t40
  %t42 = load ptr, ptr %t41
  call void @usage(ptr %t42)
  %t44 = sext i32 0 to i64
  %t45 = trunc i64 %t44 to i32
  ret i32 %t45
L10:
  br label %L9
L9:
  %t46 = load i64, ptr %t8
  %t47 = sext i32 %t46 to i64
  %t48 = getelementptr ptr, ptr %t1, i64 %t47
  %t49 = load ptr, ptr %t48
  %t50 = getelementptr [3 x i8], ptr @.str6, i64 0, i64 0
  %t51 = call i32 @strcmp(ptr %t49, ptr %t50)
  %t52 = sext i32 %t51 to i64
  %t54 = sext i32 0 to i64
  %t53 = icmp eq i64 %t52, %t54
  %t55 = zext i1 %t53 to i64
  %t56 = icmp ne i64 %t55, 0
  br i1 %t56, label %L11, label %L12
L11:
  br label %L13
L12:
  %t57 = load i64, ptr %t8
  %t58 = sext i32 %t57 to i64
  %t59 = getelementptr ptr, ptr %t1, i64 %t58
  %t60 = load ptr, ptr %t59
  %t61 = getelementptr [10 x i8], ptr @.str7, i64 0, i64 0
  %t62 = call i32 @strcmp(ptr %t60, ptr %t61)
  %t63 = sext i32 %t62 to i64
  %t65 = sext i32 0 to i64
  %t64 = icmp eq i64 %t63, %t65
  %t66 = zext i1 %t64 to i64
  %t67 = icmp ne i64 %t66, 0
  %t68 = zext i1 %t67 to i64
  br label %L13
L13:
  %t69 = phi i64 [ 1, %L11 ], [ %t68, %L12 ]
  %t70 = icmp ne i64 %t69, 0
  br i1 %t70, label %L14, label %L16
L14:
  %t71 = getelementptr [19 x i8], ptr @.str8, i64 0, i64 0
  %t72 = call i32 (ptr, ...) @printf(ptr %t71)
  %t73 = sext i32 %t72 to i64
  %t74 = sext i32 0 to i64
  %t75 = trunc i64 %t74 to i32
  ret i32 %t75
L17:
  br label %L16
L16:
  %t76 = load i64, ptr %t8
  %t77 = sext i32 %t76 to i64
  %t78 = getelementptr ptr, ptr %t1, i64 %t77
  %t79 = load ptr, ptr %t78
  %t80 = getelementptr [3 x i8], ptr @.str9, i64 0, i64 0
  %t81 = call i32 @strcmp(ptr %t79, ptr %t80)
  %t82 = sext i32 %t81 to i64
  %t84 = sext i32 0 to i64
  %t83 = icmp eq i64 %t82, %t84
  %t85 = zext i1 %t83 to i64
  %t86 = icmp ne i64 %t85, 0
  br i1 %t86, label %L18, label %L20
L18:
  %t87 = load i64, ptr %t8
  %t89 = sext i32 %t87 to i64
  %t88 = add i64 %t89, 1
  store i64 %t88, ptr %t8
  %t90 = icmp sge i64 %t88, %t0
  %t91 = zext i1 %t90 to i64
  %t92 = icmp ne i64 %t91, 0
  br i1 %t92, label %L21, label %L23
L21:
  %t93 = call ptr @__c0c_stderr()
  %t94 = getelementptr [30 x i8], ptr @.str10, i64 0, i64 0
  %t95 = call i32 (ptr, ...) @fprintf(ptr %t93, ptr %t94)
  %t96 = sext i32 %t95 to i64
  %t97 = sext i32 1 to i64
  %t98 = trunc i64 %t97 to i32
  ret i32 %t98
L24:
  br label %L23
L23:
  %t99 = load i64, ptr %t8
  %t100 = sext i32 %t99 to i64
  %t101 = getelementptr ptr, ptr %t1, i64 %t100
  %t102 = load ptr, ptr %t101
  store ptr %t102, ptr %t2
  br label %L2
L25:
  br label %L20
L20:
  %t103 = load i64, ptr %t8
  %t104 = sext i32 %t103 to i64
  %t105 = getelementptr ptr, ptr %t1, i64 %t104
  %t106 = load ptr, ptr %t105
  %t107 = getelementptr [3 x i8], ptr @.str11, i64 0, i64 0
  %t108 = call i32 @strcmp(ptr %t106, ptr %t107)
  %t109 = sext i32 %t108 to i64
  %t111 = sext i32 0 to i64
  %t110 = icmp eq i64 %t109, %t111
  %t112 = zext i1 %t110 to i64
  %t113 = icmp ne i64 %t112, 0
  br i1 %t113, label %L26, label %L28
L26:
  %t114 = load i64, ptr %t8
  %t116 = sext i32 %t114 to i64
  %t115 = add i64 %t116, 1
  store i64 %t115, ptr %t8
  %t117 = icmp sge i64 %t115, %t0
  %t118 = zext i1 %t117 to i64
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L29, label %L31
L29:
  %t120 = call ptr @__c0c_stderr()
  %t121 = getelementptr [30 x i8], ptr @.str12, i64 0, i64 0
  %t122 = call i32 (ptr, ...) @fprintf(ptr %t120, ptr %t121)
  %t123 = sext i32 %t122 to i64
  %t124 = sext i32 1 to i64
  %t125 = trunc i64 %t124 to i32
  ret i32 %t125
L32:
  br label %L31
L31:
  %t126 = load i64, ptr %t8
  %t127 = sext i32 %t126 to i64
  %t128 = getelementptr ptr, ptr %t1, i64 %t127
  %t129 = load ptr, ptr %t128
  store ptr %t129, ptr %t5
  br label %L2
L33:
  br label %L28
L28:
  %t130 = load ptr, ptr %t2
  %t132 = ptrtoint ptr %t130 to i64
  %t133 = icmp eq i64 %t132, 0
  %t131 = zext i1 %t133 to i64
  %t134 = icmp ne i64 %t131, 0
  br i1 %t134, label %L34, label %L35
L34:
  %t135 = load i64, ptr %t8
  %t136 = sext i32 %t135 to i64
  %t137 = getelementptr ptr, ptr %t1, i64 %t136
  %t138 = load ptr, ptr %t137
  store ptr %t138, ptr %t2
  br label %L36
L35:
  %t139 = call ptr @__c0c_stderr()
  %t140 = getelementptr [31 x i8], ptr @.str13, i64 0, i64 0
  %t141 = load i64, ptr %t8
  %t142 = sext i32 %t141 to i64
  %t143 = getelementptr ptr, ptr %t1, i64 %t142
  %t144 = load ptr, ptr %t143
  %t145 = call i32 (ptr, ...) @fprintf(ptr %t139, ptr %t140, ptr %t144)
  %t146 = sext i32 %t145 to i64
  %t147 = sext i32 0 to i64
  %t148 = getelementptr ptr, ptr %t1, i64 %t147
  %t149 = load ptr, ptr %t148
  call void @usage(ptr %t149)
  %t151 = sext i32 1 to i64
  %t152 = trunc i64 %t151 to i32
  ret i32 %t152
L37:
  br label %L36
L36:
  br label %L2
L2:
  %t153 = load i64, ptr %t8
  %t155 = sext i32 %t153 to i64
  %t154 = add i64 %t155, 1
  store i64 %t154, ptr %t8
  br label %L0
L3:
  %t156 = load ptr, ptr %t2
  %t158 = ptrtoint ptr %t156 to i64
  %t159 = icmp eq i64 %t158, 0
  %t157 = zext i1 %t159 to i64
  %t160 = icmp ne i64 %t157, 0
  br i1 %t160, label %L38, label %L40
L38:
  %t161 = call ptr @__c0c_stderr()
  %t162 = getelementptr [20 x i8], ptr @.str14, i64 0, i64 0
  %t163 = call i32 (ptr, ...) @fprintf(ptr %t161, ptr %t162)
  %t164 = sext i32 %t163 to i64
  %t165 = sext i32 0 to i64
  %t166 = getelementptr ptr, ptr %t1, i64 %t165
  %t167 = load ptr, ptr %t166
  call void @usage(ptr %t167)
  %t169 = sext i32 1 to i64
  %t170 = trunc i64 %t169 to i32
  ret i32 %t170
L41:
  br label %L40
L40:
  %t171 = alloca ptr
  %t172 = call ptr @__c0c_stdout()
  store ptr %t172, ptr %t171
  %t173 = load ptr, ptr %t5
  %t174 = icmp ne ptr %t173, null
  br i1 %t174, label %L42, label %L44
L42:
  %t175 = load ptr, ptr %t5
  %t176 = getelementptr [2 x i8], ptr @.str15, i64 0, i64 0
  %t177 = call ptr @fopen(ptr %t175, ptr %t176)
  store ptr %t177, ptr %t171
  %t178 = load ptr, ptr %t171
  %t180 = ptrtoint ptr %t178 to i64
  %t181 = icmp eq i64 %t180, 0
  %t179 = zext i1 %t181 to i64
  %t182 = icmp ne i64 %t179, 0
  br i1 %t182, label %L45, label %L47
L45:
  %t183 = call ptr @__c0c_stderr()
  %t184 = getelementptr [35 x i8], ptr @.str16, i64 0, i64 0
  %t185 = load ptr, ptr %t5
  %t186 = call i32 (ptr, ...) @fprintf(ptr %t183, ptr %t184, ptr %t185)
  %t187 = sext i32 %t186 to i64
  %t188 = sext i32 1 to i64
  %t189 = trunc i64 %t188 to i32
  ret i32 %t189
L48:
  br label %L47
L47:
  br label %L44
L44:
  %t190 = load ptr, ptr %t2
  %t191 = load ptr, ptr %t171
  call void @compile(ptr %t190, ptr %t191)
  %t193 = load ptr, ptr %t5
  %t194 = icmp ne ptr %t193, null
  br i1 %t194, label %L49, label %L51
L49:
  %t195 = load ptr, ptr %t171
  %t196 = call i32 @fclose(ptr %t195)
  %t197 = sext i32 %t196 to i64
  br label %L51
L51:
  %t198 = sext i32 0 to i64
  %t199 = trunc i64 %t198 to i32
  ret i32 %t199
L52:
  ret i32 0
}

@.str0 = private unnamed_addr constant [2 x i8] c"r\00"
@.str1 = private unnamed_addr constant [23 x i8] c"c0c: cannot open '%s'\0A\00"
@.str2 = private unnamed_addr constant [7 x i8] c"malloc\00"
@.str3 = private unnamed_addr constant [45 x i8] c"c0c - a self-hosting C to LLVM IR compiler\0A\0A\00"
@.str4 = private unnamed_addr constant [3 x i8] c"-h\00"
@.str5 = private unnamed_addr constant [7 x i8] c"--help\00"
@.str6 = private unnamed_addr constant [3 x i8] c"-v\00"
@.str7 = private unnamed_addr constant [10 x i8] c"--version\00"
@.str8 = private unnamed_addr constant [19 x i8] c"c0c version 0.1.0\0A\00"
@.str9 = private unnamed_addr constant [3 x i8] c"-c\00"
@.str10 = private unnamed_addr constant [30 x i8] c"c0c: -c requires an argument\0A\00"
@.str11 = private unnamed_addr constant [3 x i8] c"-o\00"
@.str12 = private unnamed_addr constant [30 x i8] c"c0c: -o requires an argument\0A\00"
@.str13 = private unnamed_addr constant [31 x i8] c"c0c: unexpected argument '%s'\0A\00"
@.str14 = private unnamed_addr constant [20 x i8] c"c0c: no input file\0A\00"
@.str15 = private unnamed_addr constant [2 x i8] c"w\00"
@.str16 = private unnamed_addr constant [35 x i8] c"c0c: cannot open output file '%s'\0A\00"
