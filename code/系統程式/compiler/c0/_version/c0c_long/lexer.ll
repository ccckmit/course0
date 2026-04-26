; ModuleID = 'lexer.c'
source_filename = "lexer.c"
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


define internal i8 @cur(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t5 = inttoptr i64 %t2 to ptr
  %t6 = getelementptr ptr, ptr %t5, i64 %t4
  %t7 = load ptr, ptr %t6
  %t8 = ptrtoint ptr %t7 to i64
  %t9 = trunc i64 %t8 to i8
  ret i8 %t9
L0:
  ret i8 0
}

define internal i8 @peek1(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t6 = sext i32 1 to i64
  %t5 = add i64 %t4, %t6
  %t7 = inttoptr i64 %t2 to ptr
  %t8 = getelementptr ptr, ptr %t7, i64 %t5
  %t9 = load ptr, ptr %t8
  %t10 = ptrtoint ptr %t9 to i64
  %t11 = trunc i64 %t10 to i8
  ret i8 %t11
L0:
  ret i8 0
}

define internal i8 @advance(ptr %t0) {
entry:
  %t1 = alloca i64
  %t2 = getelementptr i8, ptr %t0, i64 0
  %t3 = load i64, ptr %t2
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  %t6 = add i64 %t5, 1
  %t7 = getelementptr i8, ptr %t0, i64 0
  store i64 %t6, ptr %t7
  %t8 = inttoptr i64 %t3 to ptr
  %t9 = getelementptr ptr, ptr %t8, i64 %t5
  %t10 = load ptr, ptr %t9
  store ptr %t10, ptr %t1
  %t11 = load i64, ptr %t1
  %t13 = sext i32 %t11 to i64
  %t14 = sext i32 10 to i64
  %t12 = icmp eq i64 %t13, %t14
  %t15 = zext i1 %t12 to i64
  %t16 = icmp ne i64 %t15, 0
  br i1 %t16, label %L0, label %L1
L0:
  %t17 = getelementptr i8, ptr %t0, i64 0
  %t18 = load i64, ptr %t17
  %t19 = add i64 %t18, 1
  %t20 = getelementptr i8, ptr %t0, i64 0
  store i64 %t19, ptr %t20
  %t21 = getelementptr i8, ptr %t0, i64 0
  %t22 = sext i32 1 to i64
  store i64 %t22, ptr %t21
  br label %L2
L1:
  %t23 = getelementptr i8, ptr %t0, i64 0
  %t24 = load i64, ptr %t23
  %t25 = add i64 %t24, 1
  %t26 = getelementptr i8, ptr %t0, i64 0
  store i64 %t25, ptr %t26
  br label %L2
L2:
  %t27 = load i64, ptr %t1
  %t28 = sext i32 %t27 to i64
  %t29 = trunc i64 %t28 to i8
  ret i8 %t29
L3:
  ret i8 0
}

define internal ptr @strndup_local(ptr %t0, ptr %t1) {
entry:
  %t2 = alloca ptr
  %t4 = ptrtoint ptr %t1 to i64
  %t5 = sext i32 1 to i64
  %t6 = inttoptr i64 %t4 to ptr
  %t3 = getelementptr i8, ptr %t6, i64 %t5
  %t7 = call ptr @malloc(ptr %t3)
  store ptr %t7, ptr %t2
  %t8 = load ptr, ptr %t2
  %t10 = ptrtoint ptr %t8 to i64
  %t11 = icmp eq i64 %t10, 0
  %t9 = zext i1 %t11 to i64
  %t12 = icmp ne i64 %t9, 0
  br i1 %t12, label %L0, label %L2
L0:
  %t13 = getelementptr [7 x i8], ptr @.str0, i64 0, i64 0
  call void @perror(ptr %t13)
  call void @exit(i64 1)
  br label %L2
L2:
  %t16 = load ptr, ptr %t2
  %t17 = call ptr @memcpy(ptr %t16, ptr %t0, ptr %t1)
  %t18 = load ptr, ptr %t2
  %t20 = ptrtoint ptr %t1 to i64
  %t19 = getelementptr ptr, ptr %t18, i64 %t20
  %t21 = sext i32 0 to i64
  store i64 %t21, ptr %t19
  %t22 = load ptr, ptr %t2
  ret ptr %t22
L3:
  ret ptr null
}

define internal i64 @keyword_lookup(ptr %t0) {
entry:
  %t1 = getelementptr [4 x i8], ptr @.str1, i64 0, i64 0
  %t2 = call i32 @strcmp(ptr %t0, ptr %t1)
  %t3 = sext i32 %t2 to i64
  %t5 = sext i32 0 to i64
  %t4 = icmp eq i64 %t3, %t5
  %t6 = zext i1 %t4 to i64
  %t7 = icmp ne i64 %t6, 0
  br i1 %t7, label %L0, label %L2
L0:
  %t8 = sext i32 5 to i64
  ret i64 %t8
L3:
  br label %L2
L2:
  %t9 = getelementptr [5 x i8], ptr @.str2, i64 0, i64 0
  %t10 = call i32 @strcmp(ptr %t0, ptr %t9)
  %t11 = sext i32 %t10 to i64
  %t13 = sext i32 0 to i64
  %t12 = icmp eq i64 %t11, %t13
  %t14 = zext i1 %t12 to i64
  %t15 = icmp ne i64 %t14, 0
  br i1 %t15, label %L4, label %L6
L4:
  %t16 = sext i32 6 to i64
  ret i64 %t16
L7:
  br label %L6
L6:
  %t17 = getelementptr [6 x i8], ptr @.str3, i64 0, i64 0
  %t18 = call i32 @strcmp(ptr %t0, ptr %t17)
  %t19 = sext i32 %t18 to i64
  %t21 = sext i32 0 to i64
  %t20 = icmp eq i64 %t19, %t21
  %t22 = zext i1 %t20 to i64
  %t23 = icmp ne i64 %t22, 0
  br i1 %t23, label %L8, label %L10
L8:
  %t24 = sext i32 7 to i64
  ret i64 %t24
L11:
  br label %L10
L10:
  %t25 = getelementptr [7 x i8], ptr @.str4, i64 0, i64 0
  %t26 = call i32 @strcmp(ptr %t0, ptr %t25)
  %t27 = sext i32 %t26 to i64
  %t29 = sext i32 0 to i64
  %t28 = icmp eq i64 %t27, %t29
  %t30 = zext i1 %t28 to i64
  %t31 = icmp ne i64 %t30, 0
  br i1 %t31, label %L12, label %L14
L12:
  %t32 = sext i32 8 to i64
  ret i64 %t32
L15:
  br label %L14
L14:
  %t33 = getelementptr [5 x i8], ptr @.str5, i64 0, i64 0
  %t34 = call i32 @strcmp(ptr %t0, ptr %t33)
  %t35 = sext i32 %t34 to i64
  %t37 = sext i32 0 to i64
  %t36 = icmp eq i64 %t35, %t37
  %t38 = zext i1 %t36 to i64
  %t39 = icmp ne i64 %t38, 0
  br i1 %t39, label %L16, label %L18
L16:
  %t40 = sext i32 9 to i64
  ret i64 %t40
L19:
  br label %L18
L18:
  %t41 = getelementptr [5 x i8], ptr @.str6, i64 0, i64 0
  %t42 = call i32 @strcmp(ptr %t0, ptr %t41)
  %t43 = sext i32 %t42 to i64
  %t45 = sext i32 0 to i64
  %t44 = icmp eq i64 %t43, %t45
  %t46 = zext i1 %t44 to i64
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L20, label %L22
L20:
  %t48 = sext i32 10 to i64
  ret i64 %t48
L23:
  br label %L22
L22:
  %t49 = getelementptr [6 x i8], ptr @.str7, i64 0, i64 0
  %t50 = call i32 @strcmp(ptr %t0, ptr %t49)
  %t51 = sext i32 %t50 to i64
  %t53 = sext i32 0 to i64
  %t52 = icmp eq i64 %t51, %t53
  %t54 = zext i1 %t52 to i64
  %t55 = icmp ne i64 %t54, 0
  br i1 %t55, label %L24, label %L26
L24:
  %t56 = sext i32 11 to i64
  ret i64 %t56
L27:
  br label %L26
L26:
  %t57 = getelementptr [9 x i8], ptr @.str8, i64 0, i64 0
  %t58 = call i32 @strcmp(ptr %t0, ptr %t57)
  %t59 = sext i32 %t58 to i64
  %t61 = sext i32 0 to i64
  %t60 = icmp eq i64 %t59, %t61
  %t62 = zext i1 %t60 to i64
  %t63 = icmp ne i64 %t62, 0
  br i1 %t63, label %L28, label %L30
L28:
  %t64 = sext i32 12 to i64
  ret i64 %t64
L31:
  br label %L30
L30:
  %t65 = getelementptr [7 x i8], ptr @.str9, i64 0, i64 0
  %t66 = call i32 @strcmp(ptr %t0, ptr %t65)
  %t67 = sext i32 %t66 to i64
  %t69 = sext i32 0 to i64
  %t68 = icmp eq i64 %t67, %t69
  %t70 = zext i1 %t68 to i64
  %t71 = icmp ne i64 %t70, 0
  br i1 %t71, label %L32, label %L34
L32:
  %t72 = sext i32 13 to i64
  ret i64 %t72
L35:
  br label %L34
L34:
  %t73 = getelementptr [3 x i8], ptr @.str10, i64 0, i64 0
  %t74 = call i32 @strcmp(ptr %t0, ptr %t73)
  %t75 = sext i32 %t74 to i64
  %t77 = sext i32 0 to i64
  %t76 = icmp eq i64 %t75, %t77
  %t78 = zext i1 %t76 to i64
  %t79 = icmp ne i64 %t78, 0
  br i1 %t79, label %L36, label %L38
L36:
  %t80 = sext i32 14 to i64
  ret i64 %t80
L39:
  br label %L38
L38:
  %t81 = getelementptr [5 x i8], ptr @.str11, i64 0, i64 0
  %t82 = call i32 @strcmp(ptr %t0, ptr %t81)
  %t83 = sext i32 %t82 to i64
  %t85 = sext i32 0 to i64
  %t84 = icmp eq i64 %t83, %t85
  %t86 = zext i1 %t84 to i64
  %t87 = icmp ne i64 %t86, 0
  br i1 %t87, label %L40, label %L42
L40:
  %t88 = sext i32 15 to i64
  ret i64 %t88
L43:
  br label %L42
L42:
  %t89 = getelementptr [6 x i8], ptr @.str12, i64 0, i64 0
  %t90 = call i32 @strcmp(ptr %t0, ptr %t89)
  %t91 = sext i32 %t90 to i64
  %t93 = sext i32 0 to i64
  %t92 = icmp eq i64 %t91, %t93
  %t94 = zext i1 %t92 to i64
  %t95 = icmp ne i64 %t94, 0
  br i1 %t95, label %L44, label %L46
L44:
  %t96 = sext i32 16 to i64
  ret i64 %t96
L47:
  br label %L46
L46:
  %t97 = getelementptr [4 x i8], ptr @.str13, i64 0, i64 0
  %t98 = call i32 @strcmp(ptr %t0, ptr %t97)
  %t99 = sext i32 %t98 to i64
  %t101 = sext i32 0 to i64
  %t100 = icmp eq i64 %t99, %t101
  %t102 = zext i1 %t100 to i64
  %t103 = icmp ne i64 %t102, 0
  br i1 %t103, label %L48, label %L50
L48:
  %t104 = sext i32 17 to i64
  ret i64 %t104
L51:
  br label %L50
L50:
  %t105 = getelementptr [3 x i8], ptr @.str14, i64 0, i64 0
  %t106 = call i32 @strcmp(ptr %t0, ptr %t105)
  %t107 = sext i32 %t106 to i64
  %t109 = sext i32 0 to i64
  %t108 = icmp eq i64 %t107, %t109
  %t110 = zext i1 %t108 to i64
  %t111 = icmp ne i64 %t110, 0
  br i1 %t111, label %L52, label %L54
L52:
  %t112 = sext i32 18 to i64
  ret i64 %t112
L55:
  br label %L54
L54:
  %t113 = getelementptr [7 x i8], ptr @.str15, i64 0, i64 0
  %t114 = call i32 @strcmp(ptr %t0, ptr %t113)
  %t115 = sext i32 %t114 to i64
  %t117 = sext i32 0 to i64
  %t116 = icmp eq i64 %t115, %t117
  %t118 = zext i1 %t116 to i64
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L56, label %L58
L56:
  %t120 = sext i32 19 to i64
  ret i64 %t120
L59:
  br label %L58
L58:
  %t121 = getelementptr [6 x i8], ptr @.str16, i64 0, i64 0
  %t122 = call i32 @strcmp(ptr %t0, ptr %t121)
  %t123 = sext i32 %t122 to i64
  %t125 = sext i32 0 to i64
  %t124 = icmp eq i64 %t123, %t125
  %t126 = zext i1 %t124 to i64
  %t127 = icmp ne i64 %t126, 0
  br i1 %t127, label %L60, label %L62
L60:
  %t128 = sext i32 20 to i64
  ret i64 %t128
L63:
  br label %L62
L62:
  %t129 = getelementptr [9 x i8], ptr @.str17, i64 0, i64 0
  %t130 = call i32 @strcmp(ptr %t0, ptr %t129)
  %t131 = sext i32 %t130 to i64
  %t133 = sext i32 0 to i64
  %t132 = icmp eq i64 %t131, %t133
  %t134 = zext i1 %t132 to i64
  %t135 = icmp ne i64 %t134, 0
  br i1 %t135, label %L64, label %L66
L64:
  %t136 = sext i32 21 to i64
  ret i64 %t136
L67:
  br label %L66
L66:
  %t137 = getelementptr [7 x i8], ptr @.str18, i64 0, i64 0
  %t138 = call i32 @strcmp(ptr %t0, ptr %t137)
  %t139 = sext i32 %t138 to i64
  %t141 = sext i32 0 to i64
  %t140 = icmp eq i64 %t139, %t141
  %t142 = zext i1 %t140 to i64
  %t143 = icmp ne i64 %t142, 0
  br i1 %t143, label %L68, label %L70
L68:
  %t144 = sext i32 22 to i64
  ret i64 %t144
L71:
  br label %L70
L70:
  %t145 = getelementptr [5 x i8], ptr @.str19, i64 0, i64 0
  %t146 = call i32 @strcmp(ptr %t0, ptr %t145)
  %t147 = sext i32 %t146 to i64
  %t149 = sext i32 0 to i64
  %t148 = icmp eq i64 %t147, %t149
  %t150 = zext i1 %t148 to i64
  %t151 = icmp ne i64 %t150, 0
  br i1 %t151, label %L72, label %L74
L72:
  %t152 = sext i32 23 to i64
  ret i64 %t152
L75:
  br label %L74
L74:
  %t153 = getelementptr [8 x i8], ptr @.str20, i64 0, i64 0
  %t154 = call i32 @strcmp(ptr %t0, ptr %t153)
  %t155 = sext i32 %t154 to i64
  %t157 = sext i32 0 to i64
  %t156 = icmp eq i64 %t155, %t157
  %t158 = zext i1 %t156 to i64
  %t159 = icmp ne i64 %t158, 0
  br i1 %t159, label %L76, label %L78
L76:
  %t160 = sext i32 24 to i64
  ret i64 %t160
L79:
  br label %L78
L78:
  %t161 = getelementptr [5 x i8], ptr @.str21, i64 0, i64 0
  %t162 = call i32 @strcmp(ptr %t0, ptr %t161)
  %t163 = sext i32 %t162 to i64
  %t165 = sext i32 0 to i64
  %t164 = icmp eq i64 %t163, %t165
  %t166 = zext i1 %t164 to i64
  %t167 = icmp ne i64 %t166, 0
  br i1 %t167, label %L80, label %L82
L80:
  %t168 = sext i32 25 to i64
  ret i64 %t168
L83:
  br label %L82
L82:
  %t169 = getelementptr [7 x i8], ptr @.str22, i64 0, i64 0
  %t170 = call i32 @strcmp(ptr %t0, ptr %t169)
  %t171 = sext i32 %t170 to i64
  %t173 = sext i32 0 to i64
  %t172 = icmp eq i64 %t171, %t173
  %t174 = zext i1 %t172 to i64
  %t175 = icmp ne i64 %t174, 0
  br i1 %t175, label %L84, label %L86
L84:
  %t176 = sext i32 26 to i64
  ret i64 %t176
L87:
  br label %L86
L86:
  %t177 = getelementptr [6 x i8], ptr @.str23, i64 0, i64 0
  %t178 = call i32 @strcmp(ptr %t0, ptr %t177)
  %t179 = sext i32 %t178 to i64
  %t181 = sext i32 0 to i64
  %t180 = icmp eq i64 %t179, %t181
  %t182 = zext i1 %t180 to i64
  %t183 = icmp ne i64 %t182, 0
  br i1 %t183, label %L88, label %L90
L88:
  %t184 = sext i32 27 to i64
  ret i64 %t184
L91:
  br label %L90
L90:
  %t185 = getelementptr [5 x i8], ptr @.str24, i64 0, i64 0
  %t186 = call i32 @strcmp(ptr %t0, ptr %t185)
  %t187 = sext i32 %t186 to i64
  %t189 = sext i32 0 to i64
  %t188 = icmp eq i64 %t187, %t189
  %t190 = zext i1 %t188 to i64
  %t191 = icmp ne i64 %t190, 0
  br i1 %t191, label %L92, label %L94
L92:
  %t192 = sext i32 28 to i64
  ret i64 %t192
L95:
  br label %L94
L94:
  %t193 = getelementptr [8 x i8], ptr @.str25, i64 0, i64 0
  %t194 = call i32 @strcmp(ptr %t0, ptr %t193)
  %t195 = sext i32 %t194 to i64
  %t197 = sext i32 0 to i64
  %t196 = icmp eq i64 %t195, %t197
  %t198 = zext i1 %t196 to i64
  %t199 = icmp ne i64 %t198, 0
  br i1 %t199, label %L96, label %L98
L96:
  %t200 = sext i32 29 to i64
  ret i64 %t200
L99:
  br label %L98
L98:
  %t201 = getelementptr [7 x i8], ptr @.str26, i64 0, i64 0
  %t202 = call i32 @strcmp(ptr %t0, ptr %t201)
  %t203 = sext i32 %t202 to i64
  %t205 = sext i32 0 to i64
  %t204 = icmp eq i64 %t203, %t205
  %t206 = zext i1 %t204 to i64
  %t207 = icmp ne i64 %t206, 0
  br i1 %t207, label %L100, label %L102
L100:
  %t208 = sext i32 30 to i64
  ret i64 %t208
L103:
  br label %L102
L102:
  %t209 = getelementptr [7 x i8], ptr @.str27, i64 0, i64 0
  %t210 = call i32 @strcmp(ptr %t0, ptr %t209)
  %t211 = sext i32 %t210 to i64
  %t213 = sext i32 0 to i64
  %t212 = icmp eq i64 %t211, %t213
  %t214 = zext i1 %t212 to i64
  %t215 = icmp ne i64 %t214, 0
  br i1 %t215, label %L104, label %L106
L104:
  %t216 = sext i32 31 to i64
  ret i64 %t216
L107:
  br label %L106
L106:
  %t217 = getelementptr [6 x i8], ptr @.str28, i64 0, i64 0
  %t218 = call i32 @strcmp(ptr %t0, ptr %t217)
  %t219 = sext i32 %t218 to i64
  %t221 = sext i32 0 to i64
  %t220 = icmp eq i64 %t219, %t221
  %t222 = zext i1 %t220 to i64
  %t223 = icmp ne i64 %t222, 0
  br i1 %t223, label %L108, label %L110
L108:
  %t224 = sext i32 32 to i64
  ret i64 %t224
L111:
  br label %L110
L110:
  %t225 = getelementptr [9 x i8], ptr @.str29, i64 0, i64 0
  %t226 = call i32 @strcmp(ptr %t0, ptr %t225)
  %t227 = sext i32 %t226 to i64
  %t229 = sext i32 0 to i64
  %t228 = icmp eq i64 %t227, %t229
  %t230 = zext i1 %t228 to i64
  %t231 = icmp ne i64 %t230, 0
  br i1 %t231, label %L112, label %L114
L112:
  %t232 = sext i32 33 to i64
  ret i64 %t232
L115:
  br label %L114
L114:
  %t233 = getelementptr [7 x i8], ptr @.str30, i64 0, i64 0
  %t234 = call i32 @strcmp(ptr %t0, ptr %t233)
  %t235 = sext i32 %t234 to i64
  %t237 = sext i32 0 to i64
  %t236 = icmp eq i64 %t235, %t237
  %t238 = zext i1 %t236 to i64
  %t239 = icmp ne i64 %t238, 0
  br i1 %t239, label %L116, label %L118
L116:
  %t240 = sext i32 34 to i64
  ret i64 %t240
L119:
  br label %L118
L118:
  %t241 = sext i32 4 to i64
  ret i64 %t241
L120:
  ret i64 0
}

define dso_local ptr @lexer_new(ptr %t0, ptr %t1) {
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
  %t9 = getelementptr [7 x i8], ptr @.str31, i64 0, i64 0
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
  %t17 = getelementptr i8, ptr %t16, i64 0
  %t18 = sext i32 1 to i64
  store i64 %t18, ptr %t17
  %t19 = load ptr, ptr %t2
  %t20 = getelementptr i8, ptr %t19, i64 0
  %t21 = sext i32 1 to i64
  store i64 %t21, ptr %t20
  %t22 = load ptr, ptr %t2
  ret ptr %t22
L3:
  ret ptr null
}

define dso_local void @lexer_free(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = icmp ne i64 %t2, 0
  br i1 %t3, label %L0, label %L2
L0:
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = load i64, ptr %t4
  call void @token_free(i64 %t5)
  br label %L2
L2:
  call void @free(ptr %t0)
  ret void
}

define dso_local void @token_free(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  call void @free(i64 %t2)
  ret void
}

define internal void @skip_ws(ptr %t0) {
entry:
  br label %L0
L0:
  br label %L1
L1:
  br label %L4
L4:
  %t1 = call i8 @cur(ptr %t0)
  %t2 = sext i8 %t1 to i64
  %t3 = icmp ne i64 %t2, 0
  br i1 %t3, label %L7, label %L8
L7:
  %t4 = call i8 @cur(ptr %t0)
  %t5 = sext i8 %t4 to i64
  %t6 = add i64 %t5, 0
  %t7 = call i32 @isspace(i64 %t6)
  %t8 = sext i32 %t7 to i64
  %t9 = icmp ne i64 %t8, 0
  %t10 = zext i1 %t9 to i64
  br label %L9
L8:
  br label %L9
L9:
  %t11 = phi i64 [ %t10, %L7 ], [ 0, %L8 ]
  %t12 = icmp ne i64 %t11, 0
  br i1 %t12, label %L5, label %L6
L5:
  %t13 = call i8 @advance(ptr %t0)
  %t14 = sext i8 %t13 to i64
  br label %L4
L6:
  %t15 = call i8 @cur(ptr %t0)
  %t16 = sext i8 %t15 to i64
  %t18 = sext i32 47 to i64
  %t17 = icmp eq i64 %t16, %t18
  %t19 = zext i1 %t17 to i64
  %t20 = icmp ne i64 %t19, 0
  br i1 %t20, label %L10, label %L11
L10:
  %t21 = call i8 @peek1(ptr %t0)
  %t22 = sext i8 %t21 to i64
  %t24 = sext i32 47 to i64
  %t23 = icmp eq i64 %t22, %t24
  %t25 = zext i1 %t23 to i64
  %t26 = icmp ne i64 %t25, 0
  %t27 = zext i1 %t26 to i64
  br label %L12
L11:
  br label %L12
L12:
  %t28 = phi i64 [ %t27, %L10 ], [ 0, %L11 ]
  %t29 = icmp ne i64 %t28, 0
  br i1 %t29, label %L13, label %L15
L13:
  br label %L16
L16:
  %t30 = call i8 @cur(ptr %t0)
  %t31 = sext i8 %t30 to i64
  %t32 = icmp ne i64 %t31, 0
  br i1 %t32, label %L19, label %L20
L19:
  %t33 = call i8 @cur(ptr %t0)
  %t34 = sext i8 %t33 to i64
  %t36 = sext i32 10 to i64
  %t35 = icmp ne i64 %t34, %t36
  %t37 = zext i1 %t35 to i64
  %t38 = icmp ne i64 %t37, 0
  %t39 = zext i1 %t38 to i64
  br label %L21
L20:
  br label %L21
L21:
  %t40 = phi i64 [ %t39, %L19 ], [ 0, %L20 ]
  %t41 = icmp ne i64 %t40, 0
  br i1 %t41, label %L17, label %L18
L17:
  %t42 = call i8 @advance(ptr %t0)
  %t43 = sext i8 %t42 to i64
  br label %L16
L18:
  br label %L2
L22:
  br label %L15
L15:
  %t44 = call i8 @cur(ptr %t0)
  %t45 = sext i8 %t44 to i64
  %t47 = sext i32 47 to i64
  %t46 = icmp eq i64 %t45, %t47
  %t48 = zext i1 %t46 to i64
  %t49 = icmp ne i64 %t48, 0
  br i1 %t49, label %L23, label %L24
L23:
  %t50 = call i8 @peek1(ptr %t0)
  %t51 = sext i8 %t50 to i64
  %t53 = sext i32 42 to i64
  %t52 = icmp eq i64 %t51, %t53
  %t54 = zext i1 %t52 to i64
  %t55 = icmp ne i64 %t54, 0
  %t56 = zext i1 %t55 to i64
  br label %L25
L24:
  br label %L25
L25:
  %t57 = phi i64 [ %t56, %L23 ], [ 0, %L24 ]
  %t58 = icmp ne i64 %t57, 0
  br i1 %t58, label %L26, label %L28
L26:
  %t59 = call i8 @advance(ptr %t0)
  %t60 = sext i8 %t59 to i64
  %t61 = call i8 @advance(ptr %t0)
  %t62 = sext i8 %t61 to i64
  br label %L29
L29:
  %t63 = call i8 @cur(ptr %t0)
  %t64 = sext i8 %t63 to i64
  %t65 = icmp ne i64 %t64, 0
  br i1 %t65, label %L32, label %L33
L32:
  %t66 = call i8 @cur(ptr %t0)
  %t67 = sext i8 %t66 to i64
  %t69 = sext i32 42 to i64
  %t68 = icmp eq i64 %t67, %t69
  %t70 = zext i1 %t68 to i64
  %t71 = icmp ne i64 %t70, 0
  br i1 %t71, label %L35, label %L36
L35:
  %t72 = call i8 @peek1(ptr %t0)
  %t73 = sext i8 %t72 to i64
  %t75 = sext i32 47 to i64
  %t74 = icmp eq i64 %t73, %t75
  %t76 = zext i1 %t74 to i64
  %t77 = icmp ne i64 %t76, 0
  %t78 = zext i1 %t77 to i64
  br label %L37
L36:
  br label %L37
L37:
  %t79 = phi i64 [ %t78, %L35 ], [ 0, %L36 ]
  %t81 = icmp eq i64 %t79, 0
  %t80 = zext i1 %t81 to i64
  %t82 = icmp ne i64 %t80, 0
  %t83 = zext i1 %t82 to i64
  br label %L34
L33:
  br label %L34
L34:
  %t84 = phi i64 [ %t83, %L32 ], [ 0, %L33 ]
  %t85 = icmp ne i64 %t84, 0
  br i1 %t85, label %L30, label %L31
L30:
  %t86 = call i8 @advance(ptr %t0)
  %t87 = sext i8 %t86 to i64
  br label %L29
L31:
  %t88 = call i8 @cur(ptr %t0)
  %t89 = sext i8 %t88 to i64
  %t90 = icmp ne i64 %t89, 0
  br i1 %t90, label %L38, label %L40
L38:
  %t91 = call i8 @advance(ptr %t0)
  %t92 = sext i8 %t91 to i64
  %t93 = call i8 @advance(ptr %t0)
  %t94 = sext i8 %t93 to i64
  br label %L40
L40:
  br label %L2
L41:
  br label %L28
L28:
  br label %L3
L42:
  br label %L2
L2:
  br label %L0
L3:
  ret void
}

define internal i64 @read_token(ptr %t0) {
entry:
  call void @skip_ws(ptr %t0)
  %t2 = alloca i64
  %t3 = getelementptr i8, ptr %t0, i64 0
  %t4 = load i64, ptr %t3
  %t5 = getelementptr i8, ptr %t2, i64 0
  store i64 %t4, ptr %t5
  %t6 = getelementptr i8, ptr %t0, i64 0
  %t7 = load i64, ptr %t6
  %t8 = getelementptr i8, ptr %t2, i64 0
  store i64 %t7, ptr %t8
  %t10 = sext i32 0 to i64
  %t9 = inttoptr i64 %t10 to ptr
  %t11 = getelementptr i8, ptr %t2, i64 0
  store ptr %t9, ptr %t11
  %t12 = call i8 @cur(ptr %t0)
  %t13 = sext i8 %t12 to i64
  %t15 = icmp eq i64 %t13, 0
  %t14 = zext i1 %t15 to i64
  %t16 = icmp ne i64 %t14, 0
  br i1 %t16, label %L0, label %L2
L0:
  %t17 = getelementptr i8, ptr %t2, i64 0
  %t18 = sext i32 81 to i64
  store i64 %t18, ptr %t17
  %t19 = getelementptr [1 x i8], ptr @.str32, i64 0, i64 0
  %t20 = call ptr @strdup(ptr %t19)
  %t21 = getelementptr i8, ptr %t2, i64 0
  store ptr %t20, ptr %t21
  %t22 = load i64, ptr %t2
  %t23 = sext i32 %t22 to i64
  ret i64 %t23
L3:
  br label %L2
L2:
  %t24 = call i8 @cur(ptr %t0)
  %t25 = sext i8 %t24 to i64
  %t26 = add i64 %t25, 0
  %t27 = call i32 @isdigit(i64 %t26)
  %t28 = sext i32 %t27 to i64
  %t29 = icmp ne i64 %t28, 0
  br i1 %t29, label %L4, label %L5
L4:
  br label %L6
L5:
  %t30 = call i8 @cur(ptr %t0)
  %t31 = sext i8 %t30 to i64
  %t33 = sext i32 46 to i64
  %t32 = icmp eq i64 %t31, %t33
  %t34 = zext i1 %t32 to i64
  %t35 = icmp ne i64 %t34, 0
  br i1 %t35, label %L7, label %L8
L7:
  %t36 = call i8 @peek1(ptr %t0)
  %t37 = sext i8 %t36 to i64
  %t38 = add i64 %t37, 0
  %t39 = call i32 @isdigit(i64 %t38)
  %t40 = sext i32 %t39 to i64
  %t41 = icmp ne i64 %t40, 0
  %t42 = zext i1 %t41 to i64
  br label %L9
L8:
  br label %L9
L9:
  %t43 = phi i64 [ %t42, %L7 ], [ 0, %L8 ]
  %t44 = icmp ne i64 %t43, 0
  %t45 = zext i1 %t44 to i64
  br label %L6
L6:
  %t46 = phi i64 [ 1, %L4 ], [ %t45, %L5 ]
  %t47 = icmp ne i64 %t46, 0
  br i1 %t47, label %L10, label %L12
L10:
  %t48 = alloca i64
  %t49 = getelementptr i8, ptr %t0, i64 0
  %t50 = load i64, ptr %t49
  store i64 %t50, ptr %t48
  %t51 = alloca i64
  %t52 = sext i32 0 to i64
  store i64 %t52, ptr %t51
  %t53 = call i8 @cur(ptr %t0)
  %t54 = sext i8 %t53 to i64
  %t56 = sext i32 48 to i64
  %t55 = icmp eq i64 %t54, %t56
  %t57 = zext i1 %t55 to i64
  %t58 = icmp ne i64 %t57, 0
  br i1 %t58, label %L13, label %L14
L13:
  %t59 = call i8 @peek1(ptr %t0)
  %t60 = sext i8 %t59 to i64
  %t62 = sext i32 120 to i64
  %t61 = icmp eq i64 %t60, %t62
  %t63 = zext i1 %t61 to i64
  %t64 = icmp ne i64 %t63, 0
  br i1 %t64, label %L16, label %L17
L16:
  br label %L18
L17:
  %t65 = call i8 @peek1(ptr %t0)
  %t66 = sext i8 %t65 to i64
  %t68 = sext i32 88 to i64
  %t67 = icmp eq i64 %t66, %t68
  %t69 = zext i1 %t67 to i64
  %t70 = icmp ne i64 %t69, 0
  %t71 = zext i1 %t70 to i64
  br label %L18
L18:
  %t72 = phi i64 [ 1, %L16 ], [ %t71, %L17 ]
  %t73 = icmp ne i64 %t72, 0
  %t74 = zext i1 %t73 to i64
  br label %L15
L14:
  br label %L15
L15:
  %t75 = phi i64 [ %t74, %L13 ], [ 0, %L14 ]
  %t76 = icmp ne i64 %t75, 0
  br i1 %t76, label %L19, label %L20
L19:
  %t77 = call i8 @advance(ptr %t0)
  %t78 = sext i8 %t77 to i64
  %t79 = call i8 @advance(ptr %t0)
  %t80 = sext i8 %t79 to i64
  br label %L22
L22:
  %t81 = call i8 @cur(ptr %t0)
  %t82 = sext i8 %t81 to i64
  %t83 = add i64 %t82, 0
  %t84 = call i32 @isxdigit(i64 %t83)
  %t85 = sext i32 %t84 to i64
  %t86 = icmp ne i64 %t85, 0
  br i1 %t86, label %L23, label %L24
L23:
  %t87 = call i8 @advance(ptr %t0)
  %t88 = sext i8 %t87 to i64
  br label %L22
L24:
  br label %L21
L20:
  br label %L25
L25:
  %t89 = call i8 @cur(ptr %t0)
  %t90 = sext i8 %t89 to i64
  %t91 = add i64 %t90, 0
  %t92 = call i32 @isdigit(i64 %t91)
  %t93 = sext i32 %t92 to i64
  %t94 = icmp ne i64 %t93, 0
  br i1 %t94, label %L26, label %L27
L26:
  %t95 = call i8 @advance(ptr %t0)
  %t96 = sext i8 %t95 to i64
  br label %L25
L27:
  %t97 = call i8 @cur(ptr %t0)
  %t98 = sext i8 %t97 to i64
  %t100 = sext i32 46 to i64
  %t99 = icmp eq i64 %t98, %t100
  %t101 = zext i1 %t99 to i64
  %t102 = icmp ne i64 %t101, 0
  br i1 %t102, label %L28, label %L30
L28:
  %t103 = sext i32 1 to i64
  store i64 %t103, ptr %t51
  %t104 = call i8 @advance(ptr %t0)
  %t105 = sext i8 %t104 to i64
  br label %L30
L30:
  br label %L31
L31:
  %t106 = call i8 @cur(ptr %t0)
  %t107 = sext i8 %t106 to i64
  %t108 = add i64 %t107, 0
  %t109 = call i32 @isdigit(i64 %t108)
  %t110 = sext i32 %t109 to i64
  %t111 = icmp ne i64 %t110, 0
  br i1 %t111, label %L32, label %L33
L32:
  %t112 = call i8 @advance(ptr %t0)
  %t113 = sext i8 %t112 to i64
  br label %L31
L33:
  %t114 = call i8 @cur(ptr %t0)
  %t115 = sext i8 %t114 to i64
  %t117 = sext i32 101 to i64
  %t116 = icmp eq i64 %t115, %t117
  %t118 = zext i1 %t116 to i64
  %t119 = icmp ne i64 %t118, 0
  br i1 %t119, label %L34, label %L35
L34:
  br label %L36
L35:
  %t120 = call i8 @cur(ptr %t0)
  %t121 = sext i8 %t120 to i64
  %t123 = sext i32 69 to i64
  %t122 = icmp eq i64 %t121, %t123
  %t124 = zext i1 %t122 to i64
  %t125 = icmp ne i64 %t124, 0
  %t126 = zext i1 %t125 to i64
  br label %L36
L36:
  %t127 = phi i64 [ 1, %L34 ], [ %t126, %L35 ]
  %t128 = icmp ne i64 %t127, 0
  br i1 %t128, label %L37, label %L39
L37:
  %t129 = sext i32 1 to i64
  store i64 %t129, ptr %t51
  %t130 = call i8 @advance(ptr %t0)
  %t131 = sext i8 %t130 to i64
  %t132 = call i8 @cur(ptr %t0)
  %t133 = sext i8 %t132 to i64
  %t135 = sext i32 43 to i64
  %t134 = icmp eq i64 %t133, %t135
  %t136 = zext i1 %t134 to i64
  %t137 = icmp ne i64 %t136, 0
  br i1 %t137, label %L40, label %L41
L40:
  br label %L42
L41:
  %t138 = call i8 @cur(ptr %t0)
  %t139 = sext i8 %t138 to i64
  %t141 = sext i32 45 to i64
  %t140 = icmp eq i64 %t139, %t141
  %t142 = zext i1 %t140 to i64
  %t143 = icmp ne i64 %t142, 0
  %t144 = zext i1 %t143 to i64
  br label %L42
L42:
  %t145 = phi i64 [ 1, %L40 ], [ %t144, %L41 ]
  %t146 = icmp ne i64 %t145, 0
  br i1 %t146, label %L43, label %L45
L43:
  %t147 = call i8 @advance(ptr %t0)
  %t148 = sext i8 %t147 to i64
  br label %L45
L45:
  br label %L46
L46:
  %t149 = call i8 @cur(ptr %t0)
  %t150 = sext i8 %t149 to i64
  %t151 = add i64 %t150, 0
  %t152 = call i32 @isdigit(i64 %t151)
  %t153 = sext i32 %t152 to i64
  %t154 = icmp ne i64 %t153, 0
  br i1 %t154, label %L47, label %L48
L47:
  %t155 = call i8 @advance(ptr %t0)
  %t156 = sext i8 %t155 to i64
  br label %L46
L48:
  br label %L39
L39:
  br label %L21
L21:
  br label %L49
L49:
  %t157 = call i8 @cur(ptr %t0)
  %t158 = sext i8 %t157 to i64
  %t160 = sext i32 117 to i64
  %t159 = icmp eq i64 %t158, %t160
  %t161 = zext i1 %t159 to i64
  %t162 = icmp ne i64 %t161, 0
  br i1 %t162, label %L52, label %L53
L52:
  br label %L54
L53:
  %t163 = call i8 @cur(ptr %t0)
  %t164 = sext i8 %t163 to i64
  %t166 = sext i32 85 to i64
  %t165 = icmp eq i64 %t164, %t166
  %t167 = zext i1 %t165 to i64
  %t168 = icmp ne i64 %t167, 0
  %t169 = zext i1 %t168 to i64
  br label %L54
L54:
  %t170 = phi i64 [ 1, %L52 ], [ %t169, %L53 ]
  %t171 = icmp ne i64 %t170, 0
  br i1 %t171, label %L55, label %L56
L55:
  br label %L57
L56:
  %t172 = call i8 @cur(ptr %t0)
  %t173 = sext i8 %t172 to i64
  %t175 = sext i32 108 to i64
  %t174 = icmp eq i64 %t173, %t175
  %t176 = zext i1 %t174 to i64
  %t177 = icmp ne i64 %t176, 0
  %t178 = zext i1 %t177 to i64
  br label %L57
L57:
  %t179 = phi i64 [ 1, %L55 ], [ %t178, %L56 ]
  %t180 = icmp ne i64 %t179, 0
  br i1 %t180, label %L58, label %L59
L58:
  br label %L60
L59:
  %t181 = call i8 @cur(ptr %t0)
  %t182 = sext i8 %t181 to i64
  %t184 = sext i32 76 to i64
  %t183 = icmp eq i64 %t182, %t184
  %t185 = zext i1 %t183 to i64
  %t186 = icmp ne i64 %t185, 0
  %t187 = zext i1 %t186 to i64
  br label %L60
L60:
  %t188 = phi i64 [ 1, %L58 ], [ %t187, %L59 ]
  %t189 = icmp ne i64 %t188, 0
  br i1 %t189, label %L61, label %L62
L61:
  br label %L63
L62:
  %t190 = call i8 @cur(ptr %t0)
  %t191 = sext i8 %t190 to i64
  %t193 = sext i32 102 to i64
  %t192 = icmp eq i64 %t191, %t193
  %t194 = zext i1 %t192 to i64
  %t195 = icmp ne i64 %t194, 0
  %t196 = zext i1 %t195 to i64
  br label %L63
L63:
  %t197 = phi i64 [ 1, %L61 ], [ %t196, %L62 ]
  %t198 = icmp ne i64 %t197, 0
  br i1 %t198, label %L64, label %L65
L64:
  br label %L66
L65:
  %t199 = call i8 @cur(ptr %t0)
  %t200 = sext i8 %t199 to i64
  %t202 = sext i32 70 to i64
  %t201 = icmp eq i64 %t200, %t202
  %t203 = zext i1 %t201 to i64
  %t204 = icmp ne i64 %t203, 0
  %t205 = zext i1 %t204 to i64
  br label %L66
L66:
  %t206 = phi i64 [ 1, %L64 ], [ %t205, %L65 ]
  %t207 = icmp ne i64 %t206, 0
  br i1 %t207, label %L50, label %L51
L50:
  %t208 = call i8 @advance(ptr %t0)
  %t209 = sext i8 %t208 to i64
  br label %L49
L51:
  %t210 = load i64, ptr %t51
  %t212 = sext i32 %t210 to i64
  %t211 = icmp ne i64 %t212, 0
  br i1 %t211, label %L67, label %L68
L67:
  %t213 = sext i32 1 to i64
  br label %L69
L68:
  %t214 = sext i32 0 to i64
  br label %L69
L69:
  %t215 = phi i64 [ %t213, %L67 ], [ %t214, %L68 ]
  %t216 = getelementptr i8, ptr %t2, i64 0
  store i64 %t215, ptr %t216
  %t217 = getelementptr i8, ptr %t0, i64 0
  %t218 = load i64, ptr %t217
  %t219 = load i64, ptr %t48
  %t221 = sext i32 %t219 to i64
  %t220 = add i64 %t218, %t221
  %t222 = getelementptr i8, ptr %t0, i64 0
  %t223 = load i64, ptr %t222
  %t224 = load i64, ptr %t48
  %t226 = sext i32 %t224 to i64
  %t225 = sub i64 %t223, %t226
  %t227 = call ptr @strndup_local(i64 %t220, i64 %t225)
  %t228 = getelementptr i8, ptr %t2, i64 0
  store ptr %t227, ptr %t228
  %t229 = load i64, ptr %t2
  %t230 = sext i32 %t229 to i64
  ret i64 %t230
L70:
  br label %L12
L12:
  %t231 = call i8 @cur(ptr %t0)
  %t232 = sext i8 %t231 to i64
  %t234 = sext i32 39 to i64
  %t233 = icmp eq i64 %t232, %t234
  %t235 = zext i1 %t233 to i64
  %t236 = icmp ne i64 %t235, 0
  br i1 %t236, label %L71, label %L73
L71:
  %t237 = alloca i64
  %t238 = getelementptr i8, ptr %t0, i64 0
  %t239 = load i64, ptr %t238
  store i64 %t239, ptr %t237
  %t240 = call i8 @advance(ptr %t0)
  %t241 = sext i8 %t240 to i64
  %t242 = call i8 @cur(ptr %t0)
  %t243 = sext i8 %t242 to i64
  %t245 = sext i32 92 to i64
  %t244 = icmp eq i64 %t243, %t245
  %t246 = zext i1 %t244 to i64
  %t247 = icmp ne i64 %t246, 0
  br i1 %t247, label %L74, label %L75
L74:
  %t248 = call i8 @advance(ptr %t0)
  %t249 = sext i8 %t248 to i64
  %t250 = call i8 @advance(ptr %t0)
  %t251 = sext i8 %t250 to i64
  br label %L76
L75:
  %t252 = call i8 @advance(ptr %t0)
  %t253 = sext i8 %t252 to i64
  br label %L76
L76:
  %t254 = call i8 @cur(ptr %t0)
  %t255 = sext i8 %t254 to i64
  %t257 = sext i32 39 to i64
  %t256 = icmp eq i64 %t255, %t257
  %t258 = zext i1 %t256 to i64
  %t259 = icmp ne i64 %t258, 0
  br i1 %t259, label %L77, label %L79
L77:
  %t260 = call i8 @advance(ptr %t0)
  %t261 = sext i8 %t260 to i64
  br label %L79
L79:
  %t262 = getelementptr i8, ptr %t2, i64 0
  %t263 = sext i32 2 to i64
  store i64 %t263, ptr %t262
  %t264 = getelementptr i8, ptr %t0, i64 0
  %t265 = load i64, ptr %t264
  %t266 = load i64, ptr %t237
  %t268 = sext i32 %t266 to i64
  %t267 = add i64 %t265, %t268
  %t269 = getelementptr i8, ptr %t0, i64 0
  %t270 = load i64, ptr %t269
  %t271 = load i64, ptr %t237
  %t273 = sext i32 %t271 to i64
  %t272 = sub i64 %t270, %t273
  %t274 = call ptr @strndup_local(i64 %t267, i64 %t272)
  %t275 = getelementptr i8, ptr %t2, i64 0
  store ptr %t274, ptr %t275
  %t276 = load i64, ptr %t2
  %t277 = sext i32 %t276 to i64
  ret i64 %t277
L80:
  br label %L73
L73:
  %t278 = call i8 @cur(ptr %t0)
  %t279 = sext i8 %t278 to i64
  %t281 = sext i32 34 to i64
  %t280 = icmp eq i64 %t279, %t281
  %t282 = zext i1 %t280 to i64
  %t283 = icmp ne i64 %t282, 0
  br i1 %t283, label %L81, label %L83
L81:
  %t284 = alloca i64
  %t285 = getelementptr i8, ptr %t0, i64 0
  %t286 = load i64, ptr %t285
  store i64 %t286, ptr %t284
  %t287 = call i8 @advance(ptr %t0)
  %t288 = sext i8 %t287 to i64
  br label %L84
L84:
  %t289 = call i8 @cur(ptr %t0)
  %t290 = sext i8 %t289 to i64
  %t291 = icmp ne i64 %t290, 0
  br i1 %t291, label %L87, label %L88
L87:
  %t292 = call i8 @cur(ptr %t0)
  %t293 = sext i8 %t292 to i64
  %t295 = sext i32 34 to i64
  %t294 = icmp ne i64 %t293, %t295
  %t296 = zext i1 %t294 to i64
  %t297 = icmp ne i64 %t296, 0
  %t298 = zext i1 %t297 to i64
  br label %L89
L88:
  br label %L89
L89:
  %t299 = phi i64 [ %t298, %L87 ], [ 0, %L88 ]
  %t300 = icmp ne i64 %t299, 0
  br i1 %t300, label %L85, label %L86
L85:
  %t301 = call i8 @cur(ptr %t0)
  %t302 = sext i8 %t301 to i64
  %t304 = sext i32 92 to i64
  %t303 = icmp eq i64 %t302, %t304
  %t305 = zext i1 %t303 to i64
  %t306 = icmp ne i64 %t305, 0
  br i1 %t306, label %L90, label %L92
L90:
  %t307 = call i8 @advance(ptr %t0)
  %t308 = sext i8 %t307 to i64
  br label %L92
L92:
  %t309 = call i8 @cur(ptr %t0)
  %t310 = sext i8 %t309 to i64
  %t311 = icmp ne i64 %t310, 0
  br i1 %t311, label %L93, label %L95
L93:
  %t312 = call i8 @advance(ptr %t0)
  %t313 = sext i8 %t312 to i64
  br label %L95
L95:
  br label %L84
L86:
  %t314 = call i8 @cur(ptr %t0)
  %t315 = sext i8 %t314 to i64
  %t317 = sext i32 34 to i64
  %t316 = icmp eq i64 %t315, %t317
  %t318 = zext i1 %t316 to i64
  %t319 = icmp ne i64 %t318, 0
  br i1 %t319, label %L96, label %L98
L96:
  %t320 = call i8 @advance(ptr %t0)
  %t321 = sext i8 %t320 to i64
  br label %L98
L98:
  %t322 = getelementptr i8, ptr %t2, i64 0
  %t323 = sext i32 3 to i64
  store i64 %t323, ptr %t322
  %t324 = getelementptr i8, ptr %t0, i64 0
  %t325 = load i64, ptr %t324
  %t326 = load i64, ptr %t284
  %t328 = sext i32 %t326 to i64
  %t327 = add i64 %t325, %t328
  %t329 = getelementptr i8, ptr %t0, i64 0
  %t330 = load i64, ptr %t329
  %t331 = load i64, ptr %t284
  %t333 = sext i32 %t331 to i64
  %t332 = sub i64 %t330, %t333
  %t334 = call ptr @strndup_local(i64 %t327, i64 %t332)
  %t335 = getelementptr i8, ptr %t2, i64 0
  store ptr %t334, ptr %t335
  %t336 = load i64, ptr %t2
  %t337 = sext i32 %t336 to i64
  ret i64 %t337
L99:
  br label %L83
L83:
  %t338 = call i8 @cur(ptr %t0)
  %t339 = sext i8 %t338 to i64
  %t340 = add i64 %t339, 0
  %t341 = call i32 @isalpha(i64 %t340)
  %t342 = sext i32 %t341 to i64
  %t343 = icmp ne i64 %t342, 0
  br i1 %t343, label %L100, label %L101
L100:
  br label %L102
L101:
  %t344 = call i8 @cur(ptr %t0)
  %t345 = sext i8 %t344 to i64
  %t347 = sext i32 95 to i64
  %t346 = icmp eq i64 %t345, %t347
  %t348 = zext i1 %t346 to i64
  %t349 = icmp ne i64 %t348, 0
  %t350 = zext i1 %t349 to i64
  br label %L102
L102:
  %t351 = phi i64 [ 1, %L100 ], [ %t350, %L101 ]
  %t352 = icmp ne i64 %t351, 0
  br i1 %t352, label %L103, label %L105
L103:
  %t353 = alloca i64
  %t354 = getelementptr i8, ptr %t0, i64 0
  %t355 = load i64, ptr %t354
  store i64 %t355, ptr %t353
  br label %L106
L106:
  %t356 = call i8 @cur(ptr %t0)
  %t357 = sext i8 %t356 to i64
  %t358 = add i64 %t357, 0
  %t359 = call i32 @isalnum(i64 %t358)
  %t360 = sext i32 %t359 to i64
  %t361 = icmp ne i64 %t360, 0
  br i1 %t361, label %L109, label %L110
L109:
  br label %L111
L110:
  %t362 = call i8 @cur(ptr %t0)
  %t363 = sext i8 %t362 to i64
  %t365 = sext i32 95 to i64
  %t364 = icmp eq i64 %t363, %t365
  %t366 = zext i1 %t364 to i64
  %t367 = icmp ne i64 %t366, 0
  %t368 = zext i1 %t367 to i64
  br label %L111
L111:
  %t369 = phi i64 [ 1, %L109 ], [ %t368, %L110 ]
  %t370 = icmp ne i64 %t369, 0
  br i1 %t370, label %L107, label %L108
L107:
  %t371 = call i8 @advance(ptr %t0)
  %t372 = sext i8 %t371 to i64
  br label %L106
L108:
  %t373 = getelementptr i8, ptr %t0, i64 0
  %t374 = load i64, ptr %t373
  %t375 = load i64, ptr %t353
  %t377 = sext i32 %t375 to i64
  %t376 = add i64 %t374, %t377
  %t378 = getelementptr i8, ptr %t0, i64 0
  %t379 = load i64, ptr %t378
  %t380 = load i64, ptr %t353
  %t382 = sext i32 %t380 to i64
  %t381 = sub i64 %t379, %t382
  %t383 = call ptr @strndup_local(i64 %t376, i64 %t381)
  %t384 = getelementptr i8, ptr %t2, i64 0
  store ptr %t383, ptr %t384
  %t385 = getelementptr i8, ptr %t2, i64 0
  %t386 = load i64, ptr %t385
  %t387 = call i64 @keyword_lookup(i64 %t386)
  %t388 = getelementptr i8, ptr %t2, i64 0
  store i64 %t387, ptr %t388
  %t389 = load i64, ptr %t2
  %t390 = sext i32 %t389 to i64
  ret i64 %t390
L112:
  br label %L105
L105:
  %t391 = alloca i64
  %t392 = call i8 @advance(ptr %t0)
  %t393 = sext i8 %t392 to i64
  store i64 %t393, ptr %t391
  %t394 = alloca i64
  %t395 = call i8 @cur(ptr %t0)
  %t396 = sext i8 %t395 to i64
  store i64 %t396, ptr %t394
  %t397 = load i64, ptr %t391
  %t398 = sext i32 %t397 to i64
  %t399 = add i64 %t398, 0
  switch i64 %t399, label %L138 [
    i64 43, label %L114
    i64 45, label %L115
    i64 42, label %L116
    i64 47, label %L117
    i64 37, label %L118
    i64 38, label %L119
    i64 124, label %L120
    i64 94, label %L121
    i64 126, label %L122
    i64 60, label %L123
    i64 62, label %L124
    i64 61, label %L125
    i64 33, label %L126
    i64 46, label %L127
    i64 40, label %L128
    i64 41, label %L129
    i64 123, label %L130
    i64 125, label %L131
    i64 91, label %L132
    i64 93, label %L133
    i64 59, label %L134
    i64 44, label %L135
    i64 63, label %L136
    i64 58, label %L137
  ]
L114:
  %t400 = load i64, ptr %t394
  %t402 = sext i32 %t400 to i64
  %t403 = sext i32 43 to i64
  %t401 = icmp eq i64 %t402, %t403
  %t404 = zext i1 %t401 to i64
  %t405 = icmp ne i64 %t404, 0
  br i1 %t405, label %L139, label %L141
L139:
  br label %L142
L142:
  %t406 = call i8 @advance(ptr %t0)
  %t407 = sext i8 %t406 to i64
  br label %L145
L145:
  %t408 = getelementptr i8, ptr %t2, i64 0
  %t409 = sext i32 66 to i64
  store i64 %t409, ptr %t408
  %t410 = getelementptr [3 x i8], ptr @.str33, i64 0, i64 0
  %t411 = call ptr @strdup(ptr %t410)
  %t412 = getelementptr i8, ptr %t2, i64 0
  store ptr %t411, ptr %t412
  %t413 = load i64, ptr %t2
  %t414 = sext i32 %t413 to i64
  ret i64 %t414
L148:
  br label %L146
L146:
  %t416 = sext i32 0 to i64
  %t415 = icmp ne i64 %t416, 0
  br i1 %t415, label %L145, label %L147
L147:
  br label %L143
L143:
  %t418 = sext i32 0 to i64
  %t417 = icmp ne i64 %t418, 0
  br i1 %t417, label %L142, label %L144
L144:
  br label %L141
L141:
  %t419 = load i64, ptr %t394
  %t421 = sext i32 %t419 to i64
  %t422 = sext i32 61 to i64
  %t420 = icmp eq i64 %t421, %t422
  %t423 = zext i1 %t420 to i64
  %t424 = icmp ne i64 %t423, 0
  br i1 %t424, label %L149, label %L151
L149:
  br label %L152
L152:
  %t425 = call i8 @advance(ptr %t0)
  %t426 = sext i8 %t425 to i64
  br label %L155
L155:
  %t427 = getelementptr i8, ptr %t2, i64 0
  %t428 = sext i32 56 to i64
  store i64 %t428, ptr %t427
  %t429 = getelementptr [3 x i8], ptr @.str34, i64 0, i64 0
  %t430 = call ptr @strdup(ptr %t429)
  %t431 = getelementptr i8, ptr %t2, i64 0
  store ptr %t430, ptr %t431
  %t432 = load i64, ptr %t2
  %t433 = sext i32 %t432 to i64
  ret i64 %t433
L158:
  br label %L156
L156:
  %t435 = sext i32 0 to i64
  %t434 = icmp ne i64 %t435, 0
  br i1 %t434, label %L155, label %L157
L157:
  br label %L153
L153:
  %t437 = sext i32 0 to i64
  %t436 = icmp ne i64 %t437, 0
  br i1 %t436, label %L152, label %L154
L154:
  br label %L151
L151:
  br label %L159
L159:
  %t438 = getelementptr i8, ptr %t2, i64 0
  %t439 = sext i32 35 to i64
  store i64 %t439, ptr %t438
  %t440 = getelementptr [2 x i8], ptr @.str35, i64 0, i64 0
  %t441 = call ptr @strdup(ptr %t440)
  %t442 = getelementptr i8, ptr %t2, i64 0
  store ptr %t441, ptr %t442
  %t443 = load i64, ptr %t2
  %t444 = sext i32 %t443 to i64
  ret i64 %t444
L162:
  br label %L160
L160:
  %t446 = sext i32 0 to i64
  %t445 = icmp ne i64 %t446, 0
  br i1 %t445, label %L159, label %L161
L161:
  br label %L115
L115:
  %t447 = load i64, ptr %t394
  %t449 = sext i32 %t447 to i64
  %t450 = sext i32 45 to i64
  %t448 = icmp eq i64 %t449, %t450
  %t451 = zext i1 %t448 to i64
  %t452 = icmp ne i64 %t451, 0
  br i1 %t452, label %L163, label %L165
L163:
  br label %L166
L166:
  %t453 = call i8 @advance(ptr %t0)
  %t454 = sext i8 %t453 to i64
  br label %L169
L169:
  %t455 = getelementptr i8, ptr %t2, i64 0
  %t456 = sext i32 67 to i64
  store i64 %t456, ptr %t455
  %t457 = getelementptr [3 x i8], ptr @.str36, i64 0, i64 0
  %t458 = call ptr @strdup(ptr %t457)
  %t459 = getelementptr i8, ptr %t2, i64 0
  store ptr %t458, ptr %t459
  %t460 = load i64, ptr %t2
  %t461 = sext i32 %t460 to i64
  ret i64 %t461
L172:
  br label %L170
L170:
  %t463 = sext i32 0 to i64
  %t462 = icmp ne i64 %t463, 0
  br i1 %t462, label %L169, label %L171
L171:
  br label %L167
L167:
  %t465 = sext i32 0 to i64
  %t464 = icmp ne i64 %t465, 0
  br i1 %t464, label %L166, label %L168
L168:
  br label %L165
L165:
  %t466 = load i64, ptr %t394
  %t468 = sext i32 %t466 to i64
  %t469 = sext i32 61 to i64
  %t467 = icmp eq i64 %t468, %t469
  %t470 = zext i1 %t467 to i64
  %t471 = icmp ne i64 %t470, 0
  br i1 %t471, label %L173, label %L175
L173:
  br label %L176
L176:
  %t472 = call i8 @advance(ptr %t0)
  %t473 = sext i8 %t472 to i64
  br label %L179
L179:
  %t474 = getelementptr i8, ptr %t2, i64 0
  %t475 = sext i32 57 to i64
  store i64 %t475, ptr %t474
  %t476 = getelementptr [3 x i8], ptr @.str37, i64 0, i64 0
  %t477 = call ptr @strdup(ptr %t476)
  %t478 = getelementptr i8, ptr %t2, i64 0
  store ptr %t477, ptr %t478
  %t479 = load i64, ptr %t2
  %t480 = sext i32 %t479 to i64
  ret i64 %t480
L182:
  br label %L180
L180:
  %t482 = sext i32 0 to i64
  %t481 = icmp ne i64 %t482, 0
  br i1 %t481, label %L179, label %L181
L181:
  br label %L177
L177:
  %t484 = sext i32 0 to i64
  %t483 = icmp ne i64 %t484, 0
  br i1 %t483, label %L176, label %L178
L178:
  br label %L175
L175:
  %t485 = load i64, ptr %t394
  %t487 = sext i32 %t485 to i64
  %t488 = sext i32 62 to i64
  %t486 = icmp eq i64 %t487, %t488
  %t489 = zext i1 %t486 to i64
  %t490 = icmp ne i64 %t489, 0
  br i1 %t490, label %L183, label %L185
L183:
  br label %L186
L186:
  %t491 = call i8 @advance(ptr %t0)
  %t492 = sext i8 %t491 to i64
  br label %L189
L189:
  %t493 = getelementptr i8, ptr %t2, i64 0
  %t494 = sext i32 68 to i64
  store i64 %t494, ptr %t493
  %t495 = getelementptr [3 x i8], ptr @.str38, i64 0, i64 0
  %t496 = call ptr @strdup(ptr %t495)
  %t497 = getelementptr i8, ptr %t2, i64 0
  store ptr %t496, ptr %t497
  %t498 = load i64, ptr %t2
  %t499 = sext i32 %t498 to i64
  ret i64 %t499
L192:
  br label %L190
L190:
  %t501 = sext i32 0 to i64
  %t500 = icmp ne i64 %t501, 0
  br i1 %t500, label %L189, label %L191
L191:
  br label %L187
L187:
  %t503 = sext i32 0 to i64
  %t502 = icmp ne i64 %t503, 0
  br i1 %t502, label %L186, label %L188
L188:
  br label %L185
L185:
  br label %L193
L193:
  %t504 = getelementptr i8, ptr %t2, i64 0
  %t505 = sext i32 36 to i64
  store i64 %t505, ptr %t504
  %t506 = getelementptr [2 x i8], ptr @.str39, i64 0, i64 0
  %t507 = call ptr @strdup(ptr %t506)
  %t508 = getelementptr i8, ptr %t2, i64 0
  store ptr %t507, ptr %t508
  %t509 = load i64, ptr %t2
  %t510 = sext i32 %t509 to i64
  ret i64 %t510
L196:
  br label %L194
L194:
  %t512 = sext i32 0 to i64
  %t511 = icmp ne i64 %t512, 0
  br i1 %t511, label %L193, label %L195
L195:
  br label %L116
L116:
  %t513 = load i64, ptr %t394
  %t515 = sext i32 %t513 to i64
  %t516 = sext i32 61 to i64
  %t514 = icmp eq i64 %t515, %t516
  %t517 = zext i1 %t514 to i64
  %t518 = icmp ne i64 %t517, 0
  br i1 %t518, label %L197, label %L199
L197:
  br label %L200
L200:
  %t519 = call i8 @advance(ptr %t0)
  %t520 = sext i8 %t519 to i64
  br label %L203
L203:
  %t521 = getelementptr i8, ptr %t2, i64 0
  %t522 = sext i32 58 to i64
  store i64 %t522, ptr %t521
  %t523 = getelementptr [3 x i8], ptr @.str40, i64 0, i64 0
  %t524 = call ptr @strdup(ptr %t523)
  %t525 = getelementptr i8, ptr %t2, i64 0
  store ptr %t524, ptr %t525
  %t526 = load i64, ptr %t2
  %t527 = sext i32 %t526 to i64
  ret i64 %t527
L206:
  br label %L204
L204:
  %t529 = sext i32 0 to i64
  %t528 = icmp ne i64 %t529, 0
  br i1 %t528, label %L203, label %L205
L205:
  br label %L201
L201:
  %t531 = sext i32 0 to i64
  %t530 = icmp ne i64 %t531, 0
  br i1 %t530, label %L200, label %L202
L202:
  br label %L199
L199:
  br label %L207
L207:
  %t532 = getelementptr i8, ptr %t2, i64 0
  %t533 = sext i32 37 to i64
  store i64 %t533, ptr %t532
  %t534 = getelementptr [2 x i8], ptr @.str41, i64 0, i64 0
  %t535 = call ptr @strdup(ptr %t534)
  %t536 = getelementptr i8, ptr %t2, i64 0
  store ptr %t535, ptr %t536
  %t537 = load i64, ptr %t2
  %t538 = sext i32 %t537 to i64
  ret i64 %t538
L210:
  br label %L208
L208:
  %t540 = sext i32 0 to i64
  %t539 = icmp ne i64 %t540, 0
  br i1 %t539, label %L207, label %L209
L209:
  br label %L117
L117:
  %t541 = load i64, ptr %t394
  %t543 = sext i32 %t541 to i64
  %t544 = sext i32 61 to i64
  %t542 = icmp eq i64 %t543, %t544
  %t545 = zext i1 %t542 to i64
  %t546 = icmp ne i64 %t545, 0
  br i1 %t546, label %L211, label %L213
L211:
  br label %L214
L214:
  %t547 = call i8 @advance(ptr %t0)
  %t548 = sext i8 %t547 to i64
  br label %L217
L217:
  %t549 = getelementptr i8, ptr %t2, i64 0
  %t550 = sext i32 59 to i64
  store i64 %t550, ptr %t549
  %t551 = getelementptr [3 x i8], ptr @.str42, i64 0, i64 0
  %t552 = call ptr @strdup(ptr %t551)
  %t553 = getelementptr i8, ptr %t2, i64 0
  store ptr %t552, ptr %t553
  %t554 = load i64, ptr %t2
  %t555 = sext i32 %t554 to i64
  ret i64 %t555
L220:
  br label %L218
L218:
  %t557 = sext i32 0 to i64
  %t556 = icmp ne i64 %t557, 0
  br i1 %t556, label %L217, label %L219
L219:
  br label %L215
L215:
  %t559 = sext i32 0 to i64
  %t558 = icmp ne i64 %t559, 0
  br i1 %t558, label %L214, label %L216
L216:
  br label %L213
L213:
  br label %L221
L221:
  %t560 = getelementptr i8, ptr %t2, i64 0
  %t561 = sext i32 38 to i64
  store i64 %t561, ptr %t560
  %t562 = getelementptr [2 x i8], ptr @.str43, i64 0, i64 0
  %t563 = call ptr @strdup(ptr %t562)
  %t564 = getelementptr i8, ptr %t2, i64 0
  store ptr %t563, ptr %t564
  %t565 = load i64, ptr %t2
  %t566 = sext i32 %t565 to i64
  ret i64 %t566
L224:
  br label %L222
L222:
  %t568 = sext i32 0 to i64
  %t567 = icmp ne i64 %t568, 0
  br i1 %t567, label %L221, label %L223
L223:
  br label %L118
L118:
  %t569 = load i64, ptr %t394
  %t571 = sext i32 %t569 to i64
  %t572 = sext i32 61 to i64
  %t570 = icmp eq i64 %t571, %t572
  %t573 = zext i1 %t570 to i64
  %t574 = icmp ne i64 %t573, 0
  br i1 %t574, label %L225, label %L227
L225:
  br label %L228
L228:
  %t575 = call i8 @advance(ptr %t0)
  %t576 = sext i8 %t575 to i64
  br label %L231
L231:
  %t577 = getelementptr i8, ptr %t2, i64 0
  %t578 = sext i32 65 to i64
  store i64 %t578, ptr %t577
  %t579 = getelementptr [3 x i8], ptr @.str44, i64 0, i64 0
  %t580 = call ptr @strdup(ptr %t579)
  %t581 = getelementptr i8, ptr %t2, i64 0
  store ptr %t580, ptr %t581
  %t582 = load i64, ptr %t2
  %t583 = sext i32 %t582 to i64
  ret i64 %t583
L234:
  br label %L232
L232:
  %t585 = sext i32 0 to i64
  %t584 = icmp ne i64 %t585, 0
  br i1 %t584, label %L231, label %L233
L233:
  br label %L229
L229:
  %t587 = sext i32 0 to i64
  %t586 = icmp ne i64 %t587, 0
  br i1 %t586, label %L228, label %L230
L230:
  br label %L227
L227:
  br label %L235
L235:
  %t588 = getelementptr i8, ptr %t2, i64 0
  %t589 = sext i32 39 to i64
  store i64 %t589, ptr %t588
  %t590 = getelementptr [2 x i8], ptr @.str45, i64 0, i64 0
  %t591 = call ptr @strdup(ptr %t590)
  %t592 = getelementptr i8, ptr %t2, i64 0
  store ptr %t591, ptr %t592
  %t593 = load i64, ptr %t2
  %t594 = sext i32 %t593 to i64
  ret i64 %t594
L238:
  br label %L236
L236:
  %t596 = sext i32 0 to i64
  %t595 = icmp ne i64 %t596, 0
  br i1 %t595, label %L235, label %L237
L237:
  br label %L119
L119:
  %t597 = load i64, ptr %t394
  %t599 = sext i32 %t597 to i64
  %t600 = sext i32 38 to i64
  %t598 = icmp eq i64 %t599, %t600
  %t601 = zext i1 %t598 to i64
  %t602 = icmp ne i64 %t601, 0
  br i1 %t602, label %L239, label %L241
L239:
  br label %L242
L242:
  %t603 = call i8 @advance(ptr %t0)
  %t604 = sext i8 %t603 to i64
  br label %L245
L245:
  %t605 = getelementptr i8, ptr %t2, i64 0
  %t606 = sext i32 52 to i64
  store i64 %t606, ptr %t605
  %t607 = getelementptr [3 x i8], ptr @.str46, i64 0, i64 0
  %t608 = call ptr @strdup(ptr %t607)
  %t609 = getelementptr i8, ptr %t2, i64 0
  store ptr %t608, ptr %t609
  %t610 = load i64, ptr %t2
  %t611 = sext i32 %t610 to i64
  ret i64 %t611
L248:
  br label %L246
L246:
  %t613 = sext i32 0 to i64
  %t612 = icmp ne i64 %t613, 0
  br i1 %t612, label %L245, label %L247
L247:
  br label %L243
L243:
  %t615 = sext i32 0 to i64
  %t614 = icmp ne i64 %t615, 0
  br i1 %t614, label %L242, label %L244
L244:
  br label %L241
L241:
  %t616 = load i64, ptr %t394
  %t618 = sext i32 %t616 to i64
  %t619 = sext i32 61 to i64
  %t617 = icmp eq i64 %t618, %t619
  %t620 = zext i1 %t617 to i64
  %t621 = icmp ne i64 %t620, 0
  br i1 %t621, label %L249, label %L251
L249:
  br label %L252
L252:
  %t622 = call i8 @advance(ptr %t0)
  %t623 = sext i8 %t622 to i64
  br label %L255
L255:
  %t624 = getelementptr i8, ptr %t2, i64 0
  %t625 = sext i32 60 to i64
  store i64 %t625, ptr %t624
  %t626 = getelementptr [3 x i8], ptr @.str47, i64 0, i64 0
  %t627 = call ptr @strdup(ptr %t626)
  %t628 = getelementptr i8, ptr %t2, i64 0
  store ptr %t627, ptr %t628
  %t629 = load i64, ptr %t2
  %t630 = sext i32 %t629 to i64
  ret i64 %t630
L258:
  br label %L256
L256:
  %t632 = sext i32 0 to i64
  %t631 = icmp ne i64 %t632, 0
  br i1 %t631, label %L255, label %L257
L257:
  br label %L253
L253:
  %t634 = sext i32 0 to i64
  %t633 = icmp ne i64 %t634, 0
  br i1 %t633, label %L252, label %L254
L254:
  br label %L251
L251:
  br label %L259
L259:
  %t635 = getelementptr i8, ptr %t2, i64 0
  %t636 = sext i32 40 to i64
  store i64 %t636, ptr %t635
  %t637 = getelementptr [2 x i8], ptr @.str48, i64 0, i64 0
  %t638 = call ptr @strdup(ptr %t637)
  %t639 = getelementptr i8, ptr %t2, i64 0
  store ptr %t638, ptr %t639
  %t640 = load i64, ptr %t2
  %t641 = sext i32 %t640 to i64
  ret i64 %t641
L262:
  br label %L260
L260:
  %t643 = sext i32 0 to i64
  %t642 = icmp ne i64 %t643, 0
  br i1 %t642, label %L259, label %L261
L261:
  br label %L120
L120:
  %t644 = load i64, ptr %t394
  %t646 = sext i32 %t644 to i64
  %t647 = sext i32 124 to i64
  %t645 = icmp eq i64 %t646, %t647
  %t648 = zext i1 %t645 to i64
  %t649 = icmp ne i64 %t648, 0
  br i1 %t649, label %L263, label %L265
L263:
  br label %L266
L266:
  %t650 = call i8 @advance(ptr %t0)
  %t651 = sext i8 %t650 to i64
  br label %L269
L269:
  %t652 = getelementptr i8, ptr %t2, i64 0
  %t653 = sext i32 53 to i64
  store i64 %t653, ptr %t652
  %t654 = getelementptr [3 x i8], ptr @.str49, i64 0, i64 0
  %t655 = call ptr @strdup(ptr %t654)
  %t656 = getelementptr i8, ptr %t2, i64 0
  store ptr %t655, ptr %t656
  %t657 = load i64, ptr %t2
  %t658 = sext i32 %t657 to i64
  ret i64 %t658
L272:
  br label %L270
L270:
  %t660 = sext i32 0 to i64
  %t659 = icmp ne i64 %t660, 0
  br i1 %t659, label %L269, label %L271
L271:
  br label %L267
L267:
  %t662 = sext i32 0 to i64
  %t661 = icmp ne i64 %t662, 0
  br i1 %t661, label %L266, label %L268
L268:
  br label %L265
L265:
  %t663 = load i64, ptr %t394
  %t665 = sext i32 %t663 to i64
  %t666 = sext i32 61 to i64
  %t664 = icmp eq i64 %t665, %t666
  %t667 = zext i1 %t664 to i64
  %t668 = icmp ne i64 %t667, 0
  br i1 %t668, label %L273, label %L275
L273:
  br label %L276
L276:
  %t669 = call i8 @advance(ptr %t0)
  %t670 = sext i8 %t669 to i64
  br label %L279
L279:
  %t671 = getelementptr i8, ptr %t2, i64 0
  %t672 = sext i32 61 to i64
  store i64 %t672, ptr %t671
  %t673 = getelementptr [3 x i8], ptr @.str50, i64 0, i64 0
  %t674 = call ptr @strdup(ptr %t673)
  %t675 = getelementptr i8, ptr %t2, i64 0
  store ptr %t674, ptr %t675
  %t676 = load i64, ptr %t2
  %t677 = sext i32 %t676 to i64
  ret i64 %t677
L282:
  br label %L280
L280:
  %t679 = sext i32 0 to i64
  %t678 = icmp ne i64 %t679, 0
  br i1 %t678, label %L279, label %L281
L281:
  br label %L277
L277:
  %t681 = sext i32 0 to i64
  %t680 = icmp ne i64 %t681, 0
  br i1 %t680, label %L276, label %L278
L278:
  br label %L275
L275:
  br label %L283
L283:
  %t682 = getelementptr i8, ptr %t2, i64 0
  %t683 = sext i32 41 to i64
  store i64 %t683, ptr %t682
  %t684 = getelementptr [2 x i8], ptr @.str51, i64 0, i64 0
  %t685 = call ptr @strdup(ptr %t684)
  %t686 = getelementptr i8, ptr %t2, i64 0
  store ptr %t685, ptr %t686
  %t687 = load i64, ptr %t2
  %t688 = sext i32 %t687 to i64
  ret i64 %t688
L286:
  br label %L284
L284:
  %t690 = sext i32 0 to i64
  %t689 = icmp ne i64 %t690, 0
  br i1 %t689, label %L283, label %L285
L285:
  br label %L121
L121:
  %t691 = load i64, ptr %t394
  %t693 = sext i32 %t691 to i64
  %t694 = sext i32 61 to i64
  %t692 = icmp eq i64 %t693, %t694
  %t695 = zext i1 %t692 to i64
  %t696 = icmp ne i64 %t695, 0
  br i1 %t696, label %L287, label %L289
L287:
  br label %L290
L290:
  %t697 = call i8 @advance(ptr %t0)
  %t698 = sext i8 %t697 to i64
  br label %L293
L293:
  %t699 = getelementptr i8, ptr %t2, i64 0
  %t700 = sext i32 62 to i64
  store i64 %t700, ptr %t699
  %t701 = getelementptr [3 x i8], ptr @.str52, i64 0, i64 0
  %t702 = call ptr @strdup(ptr %t701)
  %t703 = getelementptr i8, ptr %t2, i64 0
  store ptr %t702, ptr %t703
  %t704 = load i64, ptr %t2
  %t705 = sext i32 %t704 to i64
  ret i64 %t705
L296:
  br label %L294
L294:
  %t707 = sext i32 0 to i64
  %t706 = icmp ne i64 %t707, 0
  br i1 %t706, label %L293, label %L295
L295:
  br label %L291
L291:
  %t709 = sext i32 0 to i64
  %t708 = icmp ne i64 %t709, 0
  br i1 %t708, label %L290, label %L292
L292:
  br label %L289
L289:
  br label %L297
L297:
  %t710 = getelementptr i8, ptr %t2, i64 0
  %t711 = sext i32 42 to i64
  store i64 %t711, ptr %t710
  %t712 = getelementptr [2 x i8], ptr @.str53, i64 0, i64 0
  %t713 = call ptr @strdup(ptr %t712)
  %t714 = getelementptr i8, ptr %t2, i64 0
  store ptr %t713, ptr %t714
  %t715 = load i64, ptr %t2
  %t716 = sext i32 %t715 to i64
  ret i64 %t716
L300:
  br label %L298
L298:
  %t718 = sext i32 0 to i64
  %t717 = icmp ne i64 %t718, 0
  br i1 %t717, label %L297, label %L299
L299:
  br label %L122
L122:
  br label %L301
L301:
  %t719 = getelementptr i8, ptr %t2, i64 0
  %t720 = sext i32 43 to i64
  store i64 %t720, ptr %t719
  %t721 = getelementptr [2 x i8], ptr @.str54, i64 0, i64 0
  %t722 = call ptr @strdup(ptr %t721)
  %t723 = getelementptr i8, ptr %t2, i64 0
  store ptr %t722, ptr %t723
  %t724 = load i64, ptr %t2
  %t725 = sext i32 %t724 to i64
  ret i64 %t725
L304:
  br label %L302
L302:
  %t727 = sext i32 0 to i64
  %t726 = icmp ne i64 %t727, 0
  br i1 %t726, label %L301, label %L303
L303:
  br label %L123
L123:
  %t728 = load i64, ptr %t394
  %t730 = sext i32 %t728 to i64
  %t731 = sext i32 60 to i64
  %t729 = icmp eq i64 %t730, %t731
  %t732 = zext i1 %t729 to i64
  %t733 = icmp ne i64 %t732, 0
  br i1 %t733, label %L305, label %L307
L305:
  %t734 = call i8 @advance(ptr %t0)
  %t735 = sext i8 %t734 to i64
  %t736 = call i8 @cur(ptr %t0)
  %t737 = sext i8 %t736 to i64
  %t739 = sext i32 61 to i64
  %t738 = icmp eq i64 %t737, %t739
  %t740 = zext i1 %t738 to i64
  %t741 = icmp ne i64 %t740, 0
  br i1 %t741, label %L308, label %L310
L308:
  br label %L311
L311:
  %t742 = call i8 @advance(ptr %t0)
  %t743 = sext i8 %t742 to i64
  br label %L314
L314:
  %t744 = getelementptr i8, ptr %t2, i64 0
  %t745 = sext i32 63 to i64
  store i64 %t745, ptr %t744
  %t746 = getelementptr [4 x i8], ptr @.str55, i64 0, i64 0
  %t747 = call ptr @strdup(ptr %t746)
  %t748 = getelementptr i8, ptr %t2, i64 0
  store ptr %t747, ptr %t748
  %t749 = load i64, ptr %t2
  %t750 = sext i32 %t749 to i64
  ret i64 %t750
L317:
  br label %L315
L315:
  %t752 = sext i32 0 to i64
  %t751 = icmp ne i64 %t752, 0
  br i1 %t751, label %L314, label %L316
L316:
  br label %L312
L312:
  %t754 = sext i32 0 to i64
  %t753 = icmp ne i64 %t754, 0
  br i1 %t753, label %L311, label %L313
L313:
  br label %L310
L310:
  br label %L318
L318:
  %t755 = getelementptr i8, ptr %t2, i64 0
  %t756 = sext i32 44 to i64
  store i64 %t756, ptr %t755
  %t757 = getelementptr [3 x i8], ptr @.str56, i64 0, i64 0
  %t758 = call ptr @strdup(ptr %t757)
  %t759 = getelementptr i8, ptr %t2, i64 0
  store ptr %t758, ptr %t759
  %t760 = load i64, ptr %t2
  %t761 = sext i32 %t760 to i64
  ret i64 %t761
L321:
  br label %L319
L319:
  %t763 = sext i32 0 to i64
  %t762 = icmp ne i64 %t763, 0
  br i1 %t762, label %L318, label %L320
L320:
  br label %L307
L307:
  %t764 = load i64, ptr %t394
  %t766 = sext i32 %t764 to i64
  %t767 = sext i32 61 to i64
  %t765 = icmp eq i64 %t766, %t767
  %t768 = zext i1 %t765 to i64
  %t769 = icmp ne i64 %t768, 0
  br i1 %t769, label %L322, label %L324
L322:
  br label %L325
L325:
  %t770 = call i8 @advance(ptr %t0)
  %t771 = sext i8 %t770 to i64
  br label %L328
L328:
  %t772 = getelementptr i8, ptr %t2, i64 0
  %t773 = sext i32 50 to i64
  store i64 %t773, ptr %t772
  %t774 = getelementptr [3 x i8], ptr @.str57, i64 0, i64 0
  %t775 = call ptr @strdup(ptr %t774)
  %t776 = getelementptr i8, ptr %t2, i64 0
  store ptr %t775, ptr %t776
  %t777 = load i64, ptr %t2
  %t778 = sext i32 %t777 to i64
  ret i64 %t778
L331:
  br label %L329
L329:
  %t780 = sext i32 0 to i64
  %t779 = icmp ne i64 %t780, 0
  br i1 %t779, label %L328, label %L330
L330:
  br label %L326
L326:
  %t782 = sext i32 0 to i64
  %t781 = icmp ne i64 %t782, 0
  br i1 %t781, label %L325, label %L327
L327:
  br label %L324
L324:
  br label %L332
L332:
  %t783 = getelementptr i8, ptr %t2, i64 0
  %t784 = sext i32 48 to i64
  store i64 %t784, ptr %t783
  %t785 = getelementptr [2 x i8], ptr @.str58, i64 0, i64 0
  %t786 = call ptr @strdup(ptr %t785)
  %t787 = getelementptr i8, ptr %t2, i64 0
  store ptr %t786, ptr %t787
  %t788 = load i64, ptr %t2
  %t789 = sext i32 %t788 to i64
  ret i64 %t789
L335:
  br label %L333
L333:
  %t791 = sext i32 0 to i64
  %t790 = icmp ne i64 %t791, 0
  br i1 %t790, label %L332, label %L334
L334:
  br label %L124
L124:
  %t792 = load i64, ptr %t394
  %t794 = sext i32 %t792 to i64
  %t795 = sext i32 62 to i64
  %t793 = icmp eq i64 %t794, %t795
  %t796 = zext i1 %t793 to i64
  %t797 = icmp ne i64 %t796, 0
  br i1 %t797, label %L336, label %L338
L336:
  %t798 = call i8 @advance(ptr %t0)
  %t799 = sext i8 %t798 to i64
  %t800 = call i8 @cur(ptr %t0)
  %t801 = sext i8 %t800 to i64
  %t803 = sext i32 61 to i64
  %t802 = icmp eq i64 %t801, %t803
  %t804 = zext i1 %t802 to i64
  %t805 = icmp ne i64 %t804, 0
  br i1 %t805, label %L339, label %L341
L339:
  br label %L342
L342:
  %t806 = call i8 @advance(ptr %t0)
  %t807 = sext i8 %t806 to i64
  br label %L345
L345:
  %t808 = getelementptr i8, ptr %t2, i64 0
  %t809 = sext i32 64 to i64
  store i64 %t809, ptr %t808
  %t810 = getelementptr [4 x i8], ptr @.str59, i64 0, i64 0
  %t811 = call ptr @strdup(ptr %t810)
  %t812 = getelementptr i8, ptr %t2, i64 0
  store ptr %t811, ptr %t812
  %t813 = load i64, ptr %t2
  %t814 = sext i32 %t813 to i64
  ret i64 %t814
L348:
  br label %L346
L346:
  %t816 = sext i32 0 to i64
  %t815 = icmp ne i64 %t816, 0
  br i1 %t815, label %L345, label %L347
L347:
  br label %L343
L343:
  %t818 = sext i32 0 to i64
  %t817 = icmp ne i64 %t818, 0
  br i1 %t817, label %L342, label %L344
L344:
  br label %L341
L341:
  br label %L349
L349:
  %t819 = getelementptr i8, ptr %t2, i64 0
  %t820 = sext i32 45 to i64
  store i64 %t820, ptr %t819
  %t821 = getelementptr [3 x i8], ptr @.str60, i64 0, i64 0
  %t822 = call ptr @strdup(ptr %t821)
  %t823 = getelementptr i8, ptr %t2, i64 0
  store ptr %t822, ptr %t823
  %t824 = load i64, ptr %t2
  %t825 = sext i32 %t824 to i64
  ret i64 %t825
L352:
  br label %L350
L350:
  %t827 = sext i32 0 to i64
  %t826 = icmp ne i64 %t827, 0
  br i1 %t826, label %L349, label %L351
L351:
  br label %L338
L338:
  %t828 = load i64, ptr %t394
  %t830 = sext i32 %t828 to i64
  %t831 = sext i32 61 to i64
  %t829 = icmp eq i64 %t830, %t831
  %t832 = zext i1 %t829 to i64
  %t833 = icmp ne i64 %t832, 0
  br i1 %t833, label %L353, label %L355
L353:
  br label %L356
L356:
  %t834 = call i8 @advance(ptr %t0)
  %t835 = sext i8 %t834 to i64
  br label %L359
L359:
  %t836 = getelementptr i8, ptr %t2, i64 0
  %t837 = sext i32 51 to i64
  store i64 %t837, ptr %t836
  %t838 = getelementptr [3 x i8], ptr @.str61, i64 0, i64 0
  %t839 = call ptr @strdup(ptr %t838)
  %t840 = getelementptr i8, ptr %t2, i64 0
  store ptr %t839, ptr %t840
  %t841 = load i64, ptr %t2
  %t842 = sext i32 %t841 to i64
  ret i64 %t842
L362:
  br label %L360
L360:
  %t844 = sext i32 0 to i64
  %t843 = icmp ne i64 %t844, 0
  br i1 %t843, label %L359, label %L361
L361:
  br label %L357
L357:
  %t846 = sext i32 0 to i64
  %t845 = icmp ne i64 %t846, 0
  br i1 %t845, label %L356, label %L358
L358:
  br label %L355
L355:
  br label %L363
L363:
  %t847 = getelementptr i8, ptr %t2, i64 0
  %t848 = sext i32 49 to i64
  store i64 %t848, ptr %t847
  %t849 = getelementptr [2 x i8], ptr @.str62, i64 0, i64 0
  %t850 = call ptr @strdup(ptr %t849)
  %t851 = getelementptr i8, ptr %t2, i64 0
  store ptr %t850, ptr %t851
  %t852 = load i64, ptr %t2
  %t853 = sext i32 %t852 to i64
  ret i64 %t853
L366:
  br label %L364
L364:
  %t855 = sext i32 0 to i64
  %t854 = icmp ne i64 %t855, 0
  br i1 %t854, label %L363, label %L365
L365:
  br label %L125
L125:
  %t856 = load i64, ptr %t394
  %t858 = sext i32 %t856 to i64
  %t859 = sext i32 61 to i64
  %t857 = icmp eq i64 %t858, %t859
  %t860 = zext i1 %t857 to i64
  %t861 = icmp ne i64 %t860, 0
  br i1 %t861, label %L367, label %L369
L367:
  br label %L370
L370:
  %t862 = call i8 @advance(ptr %t0)
  %t863 = sext i8 %t862 to i64
  br label %L373
L373:
  %t864 = getelementptr i8, ptr %t2, i64 0
  %t865 = sext i32 46 to i64
  store i64 %t865, ptr %t864
  %t866 = getelementptr [3 x i8], ptr @.str63, i64 0, i64 0
  %t867 = call ptr @strdup(ptr %t866)
  %t868 = getelementptr i8, ptr %t2, i64 0
  store ptr %t867, ptr %t868
  %t869 = load i64, ptr %t2
  %t870 = sext i32 %t869 to i64
  ret i64 %t870
L376:
  br label %L374
L374:
  %t872 = sext i32 0 to i64
  %t871 = icmp ne i64 %t872, 0
  br i1 %t871, label %L373, label %L375
L375:
  br label %L371
L371:
  %t874 = sext i32 0 to i64
  %t873 = icmp ne i64 %t874, 0
  br i1 %t873, label %L370, label %L372
L372:
  br label %L369
L369:
  br label %L377
L377:
  %t875 = getelementptr i8, ptr %t2, i64 0
  %t876 = sext i32 55 to i64
  store i64 %t876, ptr %t875
  %t877 = getelementptr [2 x i8], ptr @.str64, i64 0, i64 0
  %t878 = call ptr @strdup(ptr %t877)
  %t879 = getelementptr i8, ptr %t2, i64 0
  store ptr %t878, ptr %t879
  %t880 = load i64, ptr %t2
  %t881 = sext i32 %t880 to i64
  ret i64 %t881
L380:
  br label %L378
L378:
  %t883 = sext i32 0 to i64
  %t882 = icmp ne i64 %t883, 0
  br i1 %t882, label %L377, label %L379
L379:
  br label %L126
L126:
  %t884 = load i64, ptr %t394
  %t886 = sext i32 %t884 to i64
  %t887 = sext i32 61 to i64
  %t885 = icmp eq i64 %t886, %t887
  %t888 = zext i1 %t885 to i64
  %t889 = icmp ne i64 %t888, 0
  br i1 %t889, label %L381, label %L383
L381:
  br label %L384
L384:
  %t890 = call i8 @advance(ptr %t0)
  %t891 = sext i8 %t890 to i64
  br label %L387
L387:
  %t892 = getelementptr i8, ptr %t2, i64 0
  %t893 = sext i32 47 to i64
  store i64 %t893, ptr %t892
  %t894 = getelementptr [3 x i8], ptr @.str65, i64 0, i64 0
  %t895 = call ptr @strdup(ptr %t894)
  %t896 = getelementptr i8, ptr %t2, i64 0
  store ptr %t895, ptr %t896
  %t897 = load i64, ptr %t2
  %t898 = sext i32 %t897 to i64
  ret i64 %t898
L390:
  br label %L388
L388:
  %t900 = sext i32 0 to i64
  %t899 = icmp ne i64 %t900, 0
  br i1 %t899, label %L387, label %L389
L389:
  br label %L385
L385:
  %t902 = sext i32 0 to i64
  %t901 = icmp ne i64 %t902, 0
  br i1 %t901, label %L384, label %L386
L386:
  br label %L383
L383:
  br label %L391
L391:
  %t903 = getelementptr i8, ptr %t2, i64 0
  %t904 = sext i32 54 to i64
  store i64 %t904, ptr %t903
  %t905 = getelementptr [2 x i8], ptr @.str66, i64 0, i64 0
  %t906 = call ptr @strdup(ptr %t905)
  %t907 = getelementptr i8, ptr %t2, i64 0
  store ptr %t906, ptr %t907
  %t908 = load i64, ptr %t2
  %t909 = sext i32 %t908 to i64
  ret i64 %t909
L394:
  br label %L392
L392:
  %t911 = sext i32 0 to i64
  %t910 = icmp ne i64 %t911, 0
  br i1 %t910, label %L391, label %L393
L393:
  br label %L127
L127:
  %t912 = load i64, ptr %t394
  %t914 = sext i32 %t912 to i64
  %t915 = sext i32 46 to i64
  %t913 = icmp eq i64 %t914, %t915
  %t916 = zext i1 %t913 to i64
  %t917 = icmp ne i64 %t916, 0
  br i1 %t917, label %L395, label %L396
L395:
  %t918 = getelementptr i8, ptr %t0, i64 0
  %t919 = load i64, ptr %t918
  %t920 = getelementptr i8, ptr %t0, i64 0
  %t921 = load i64, ptr %t920
  %t923 = sext i32 1 to i64
  %t922 = add i64 %t921, %t923
  %t924 = inttoptr i64 %t919 to ptr
  %t925 = getelementptr ptr, ptr %t924, i64 %t922
  %t926 = load ptr, ptr %t925
  %t928 = ptrtoint ptr %t926 to i64
  %t929 = sext i32 46 to i64
  %t927 = icmp eq i64 %t928, %t929
  %t930 = zext i1 %t927 to i64
  %t931 = icmp ne i64 %t930, 0
  %t932 = zext i1 %t931 to i64
  br label %L397
L396:
  br label %L397
L397:
  %t933 = phi i64 [ %t932, %L395 ], [ 0, %L396 ]
  %t934 = icmp ne i64 %t933, 0
  br i1 %t934, label %L398, label %L400
L398:
  %t935 = call i8 @advance(ptr %t0)
  %t936 = sext i8 %t935 to i64
  %t937 = call i8 @advance(ptr %t0)
  %t938 = sext i8 %t937 to i64
  br label %L401
L401:
  %t939 = getelementptr i8, ptr %t2, i64 0
  %t940 = sext i32 80 to i64
  store i64 %t940, ptr %t939
  %t941 = getelementptr [4 x i8], ptr @.str67, i64 0, i64 0
  %t942 = call ptr @strdup(ptr %t941)
  %t943 = getelementptr i8, ptr %t2, i64 0
  store ptr %t942, ptr %t943
  %t944 = load i64, ptr %t2
  %t945 = sext i32 %t944 to i64
  ret i64 %t945
L404:
  br label %L402
L402:
  %t947 = sext i32 0 to i64
  %t946 = icmp ne i64 %t947, 0
  br i1 %t946, label %L401, label %L403
L403:
  br label %L400
L400:
  br label %L405
L405:
  %t948 = getelementptr i8, ptr %t2, i64 0
  %t949 = sext i32 69 to i64
  store i64 %t949, ptr %t948
  %t950 = getelementptr [2 x i8], ptr @.str68, i64 0, i64 0
  %t951 = call ptr @strdup(ptr %t950)
  %t952 = getelementptr i8, ptr %t2, i64 0
  store ptr %t951, ptr %t952
  %t953 = load i64, ptr %t2
  %t954 = sext i32 %t953 to i64
  ret i64 %t954
L408:
  br label %L406
L406:
  %t956 = sext i32 0 to i64
  %t955 = icmp ne i64 %t956, 0
  br i1 %t955, label %L405, label %L407
L407:
  br label %L128
L128:
  br label %L409
L409:
  %t957 = getelementptr i8, ptr %t2, i64 0
  %t958 = sext i32 72 to i64
  store i64 %t958, ptr %t957
  %t959 = getelementptr [2 x i8], ptr @.str69, i64 0, i64 0
  %t960 = call ptr @strdup(ptr %t959)
  %t961 = getelementptr i8, ptr %t2, i64 0
  store ptr %t960, ptr %t961
  %t962 = load i64, ptr %t2
  %t963 = sext i32 %t962 to i64
  ret i64 %t963
L412:
  br label %L410
L410:
  %t965 = sext i32 0 to i64
  %t964 = icmp ne i64 %t965, 0
  br i1 %t964, label %L409, label %L411
L411:
  br label %L129
L129:
  br label %L413
L413:
  %t966 = getelementptr i8, ptr %t2, i64 0
  %t967 = sext i32 73 to i64
  store i64 %t967, ptr %t966
  %t968 = getelementptr [2 x i8], ptr @.str70, i64 0, i64 0
  %t969 = call ptr @strdup(ptr %t968)
  %t970 = getelementptr i8, ptr %t2, i64 0
  store ptr %t969, ptr %t970
  %t971 = load i64, ptr %t2
  %t972 = sext i32 %t971 to i64
  ret i64 %t972
L416:
  br label %L414
L414:
  %t974 = sext i32 0 to i64
  %t973 = icmp ne i64 %t974, 0
  br i1 %t973, label %L413, label %L415
L415:
  br label %L130
L130:
  br label %L417
L417:
  %t975 = getelementptr i8, ptr %t2, i64 0
  %t976 = sext i32 74 to i64
  store i64 %t976, ptr %t975
  %t977 = getelementptr [2 x i8], ptr @.str71, i64 0, i64 0
  %t978 = call ptr @strdup(ptr %t977)
  %t979 = getelementptr i8, ptr %t2, i64 0
  store ptr %t978, ptr %t979
  %t980 = load i64, ptr %t2
  %t981 = sext i32 %t980 to i64
  ret i64 %t981
L420:
  br label %L418
L418:
  %t983 = sext i32 0 to i64
  %t982 = icmp ne i64 %t983, 0
  br i1 %t982, label %L417, label %L419
L419:
  br label %L131
L131:
  br label %L421
L421:
  %t984 = getelementptr i8, ptr %t2, i64 0
  %t985 = sext i32 75 to i64
  store i64 %t985, ptr %t984
  %t986 = getelementptr [2 x i8], ptr @.str72, i64 0, i64 0
  %t987 = call ptr @strdup(ptr %t986)
  %t988 = getelementptr i8, ptr %t2, i64 0
  store ptr %t987, ptr %t988
  %t989 = load i64, ptr %t2
  %t990 = sext i32 %t989 to i64
  ret i64 %t990
L424:
  br label %L422
L422:
  %t992 = sext i32 0 to i64
  %t991 = icmp ne i64 %t992, 0
  br i1 %t991, label %L421, label %L423
L423:
  br label %L132
L132:
  br label %L425
L425:
  %t993 = getelementptr i8, ptr %t2, i64 0
  %t994 = sext i32 76 to i64
  store i64 %t994, ptr %t993
  %t995 = getelementptr [2 x i8], ptr @.str73, i64 0, i64 0
  %t996 = call ptr @strdup(ptr %t995)
  %t997 = getelementptr i8, ptr %t2, i64 0
  store ptr %t996, ptr %t997
  %t998 = load i64, ptr %t2
  %t999 = sext i32 %t998 to i64
  ret i64 %t999
L428:
  br label %L426
L426:
  %t1001 = sext i32 0 to i64
  %t1000 = icmp ne i64 %t1001, 0
  br i1 %t1000, label %L425, label %L427
L427:
  br label %L133
L133:
  br label %L429
L429:
  %t1002 = getelementptr i8, ptr %t2, i64 0
  %t1003 = sext i32 77 to i64
  store i64 %t1003, ptr %t1002
  %t1004 = getelementptr [2 x i8], ptr @.str74, i64 0, i64 0
  %t1005 = call ptr @strdup(ptr %t1004)
  %t1006 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1005, ptr %t1006
  %t1007 = load i64, ptr %t2
  %t1008 = sext i32 %t1007 to i64
  ret i64 %t1008
L432:
  br label %L430
L430:
  %t1010 = sext i32 0 to i64
  %t1009 = icmp ne i64 %t1010, 0
  br i1 %t1009, label %L429, label %L431
L431:
  br label %L134
L134:
  br label %L433
L433:
  %t1011 = getelementptr i8, ptr %t2, i64 0
  %t1012 = sext i32 78 to i64
  store i64 %t1012, ptr %t1011
  %t1013 = getelementptr [2 x i8], ptr @.str75, i64 0, i64 0
  %t1014 = call ptr @strdup(ptr %t1013)
  %t1015 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1014, ptr %t1015
  %t1016 = load i64, ptr %t2
  %t1017 = sext i32 %t1016 to i64
  ret i64 %t1017
L436:
  br label %L434
L434:
  %t1019 = sext i32 0 to i64
  %t1018 = icmp ne i64 %t1019, 0
  br i1 %t1018, label %L433, label %L435
L435:
  br label %L135
L135:
  br label %L437
L437:
  %t1020 = getelementptr i8, ptr %t2, i64 0
  %t1021 = sext i32 79 to i64
  store i64 %t1021, ptr %t1020
  %t1022 = getelementptr [2 x i8], ptr @.str76, i64 0, i64 0
  %t1023 = call ptr @strdup(ptr %t1022)
  %t1024 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1023, ptr %t1024
  %t1025 = load i64, ptr %t2
  %t1026 = sext i32 %t1025 to i64
  ret i64 %t1026
L440:
  br label %L438
L438:
  %t1028 = sext i32 0 to i64
  %t1027 = icmp ne i64 %t1028, 0
  br i1 %t1027, label %L437, label %L439
L439:
  br label %L136
L136:
  br label %L441
L441:
  %t1029 = getelementptr i8, ptr %t2, i64 0
  %t1030 = sext i32 70 to i64
  store i64 %t1030, ptr %t1029
  %t1031 = getelementptr [2 x i8], ptr @.str77, i64 0, i64 0
  %t1032 = call ptr @strdup(ptr %t1031)
  %t1033 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1032, ptr %t1033
  %t1034 = load i64, ptr %t2
  %t1035 = sext i32 %t1034 to i64
  ret i64 %t1035
L444:
  br label %L442
L442:
  %t1037 = sext i32 0 to i64
  %t1036 = icmp ne i64 %t1037, 0
  br i1 %t1036, label %L441, label %L443
L443:
  br label %L137
L137:
  br label %L445
L445:
  %t1038 = getelementptr i8, ptr %t2, i64 0
  %t1039 = sext i32 71 to i64
  store i64 %t1039, ptr %t1038
  %t1040 = getelementptr [2 x i8], ptr @.str78, i64 0, i64 0
  %t1041 = call ptr @strdup(ptr %t1040)
  %t1042 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1041, ptr %t1042
  %t1043 = load i64, ptr %t2
  %t1044 = sext i32 %t1043 to i64
  ret i64 %t1044
L448:
  br label %L446
L446:
  %t1046 = sext i32 0 to i64
  %t1045 = icmp ne i64 %t1046, 0
  br i1 %t1045, label %L445, label %L447
L447:
  br label %L113
L138:
  %t1047 = getelementptr i8, ptr %t2, i64 0
  %t1048 = sext i32 82 to i64
  store i64 %t1048, ptr %t1047
  %t1049 = call ptr @malloc(i64 2)
  %t1050 = getelementptr i8, ptr %t2, i64 0
  store ptr %t1049, ptr %t1050
  %t1051 = load i64, ptr %t391
  %t1052 = getelementptr i8, ptr %t2, i64 0
  %t1053 = load i64, ptr %t1052
  %t1055 = inttoptr i64 %t1053 to ptr
  %t1056 = sext i32 0 to i64
  %t1054 = getelementptr ptr, ptr %t1055, i64 %t1056
  %t1057 = sext i32 %t1051 to i64
  store i64 %t1057, ptr %t1054
  %t1058 = getelementptr i8, ptr %t2, i64 0
  %t1059 = load i64, ptr %t1058
  %t1061 = inttoptr i64 %t1059 to ptr
  %t1062 = sext i32 1 to i64
  %t1060 = getelementptr ptr, ptr %t1061, i64 %t1062
  %t1063 = sext i32 0 to i64
  store i64 %t1063, ptr %t1060
  %t1064 = load i64, ptr %t2
  %t1065 = sext i32 %t1064 to i64
  ret i64 %t1065
L449:
  br label %L113
L113:
  ret i64 0
}

define dso_local i64 @lexer_next(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t3 = icmp ne i64 %t2, 0
  br i1 %t3, label %L0, label %L2
L0:
  %t4 = getelementptr i8, ptr %t0, i64 0
  %t5 = sext i32 0 to i64
  store i64 %t5, ptr %t4
  %t6 = getelementptr i8, ptr %t0, i64 0
  %t7 = load i64, ptr %t6
  ret i64 %t7
L3:
  br label %L2
L2:
  %t8 = call i64 @read_token(ptr %t0)
  ret i64 %t8
L4:
  ret i64 0
}

define dso_local i64 @lexer_peek(ptr %t0) {
entry:
  %t1 = getelementptr i8, ptr %t0, i64 0
  %t2 = load i64, ptr %t1
  %t4 = icmp eq i64 %t2, 0
  %t3 = zext i1 %t4 to i64
  %t5 = icmp ne i64 %t3, 0
  br i1 %t5, label %L0, label %L2
L0:
  %t6 = call i64 @read_token(ptr %t0)
  %t7 = getelementptr i8, ptr %t0, i64 0
  store i64 %t6, ptr %t7
  %t8 = getelementptr i8, ptr %t0, i64 0
  %t9 = sext i32 1 to i64
  store i64 %t9, ptr %t8
  br label %L2
L2:
  %t10 = getelementptr i8, ptr %t0, i64 0
  %t11 = load i64, ptr %t10
  ret i64 %t11
L3:
  ret i64 0
}

define dso_local ptr @token_type_name(ptr %t0) {
entry:
  %t1 = ptrtoint ptr %t0 to i64
  %t2 = add i64 %t1, 0
  switch i64 %t2, label %L84 [
    i64 0, label %L1
    i64 1, label %L2
    i64 2, label %L3
    i64 3, label %L4
    i64 4, label %L5
    i64 5, label %L6
    i64 6, label %L7
    i64 7, label %L8
    i64 8, label %L9
    i64 9, label %L10
    i64 10, label %L11
    i64 11, label %L12
    i64 12, label %L13
    i64 13, label %L14
    i64 14, label %L15
    i64 15, label %L16
    i64 16, label %L17
    i64 17, label %L18
    i64 18, label %L19
    i64 19, label %L20
    i64 20, label %L21
    i64 21, label %L22
    i64 26, label %L23
    i64 27, label %L24
    i64 22, label %L25
    i64 23, label %L26
    i64 24, label %L27
    i64 25, label %L28
    i64 28, label %L29
    i64 29, label %L30
    i64 30, label %L31
    i64 31, label %L32
    i64 32, label %L33
    i64 33, label %L34
    i64 34, label %L35
    i64 35, label %L36
    i64 36, label %L37
    i64 37, label %L38
    i64 38, label %L39
    i64 39, label %L40
    i64 40, label %L41
    i64 41, label %L42
    i64 42, label %L43
    i64 43, label %L44
    i64 44, label %L45
    i64 45, label %L46
    i64 46, label %L47
    i64 47, label %L48
    i64 48, label %L49
    i64 49, label %L50
    i64 50, label %L51
    i64 51, label %L52
    i64 52, label %L53
    i64 53, label %L54
    i64 54, label %L55
    i64 55, label %L56
    i64 56, label %L57
    i64 57, label %L58
    i64 58, label %L59
    i64 59, label %L60
    i64 60, label %L61
    i64 61, label %L62
    i64 62, label %L63
    i64 63, label %L64
    i64 64, label %L65
    i64 65, label %L66
    i64 66, label %L67
    i64 67, label %L68
    i64 68, label %L69
    i64 69, label %L70
    i64 70, label %L71
    i64 71, label %L72
    i64 72, label %L73
    i64 73, label %L74
    i64 74, label %L75
    i64 75, label %L76
    i64 76, label %L77
    i64 77, label %L78
    i64 78, label %L79
    i64 79, label %L80
    i64 80, label %L81
    i64 81, label %L82
    i64 82, label %L83
  ]
L1:
  %t3 = getelementptr [12 x i8], ptr @.str79, i64 0, i64 0
  ret ptr %t3
L85:
  br label %L2
L2:
  %t4 = getelementptr [14 x i8], ptr @.str80, i64 0, i64 0
  ret ptr %t4
L86:
  br label %L3
L3:
  %t5 = getelementptr [13 x i8], ptr @.str81, i64 0, i64 0
  ret ptr %t5
L87:
  br label %L4
L4:
  %t6 = getelementptr [15 x i8], ptr @.str82, i64 0, i64 0
  ret ptr %t6
L88:
  br label %L5
L5:
  %t7 = getelementptr [10 x i8], ptr @.str83, i64 0, i64 0
  ret ptr %t7
L89:
  br label %L6
L6:
  %t8 = getelementptr [8 x i8], ptr @.str84, i64 0, i64 0
  ret ptr %t8
L90:
  br label %L7
L7:
  %t9 = getelementptr [9 x i8], ptr @.str85, i64 0, i64 0
  ret ptr %t9
L91:
  br label %L8
L8:
  %t10 = getelementptr [10 x i8], ptr @.str86, i64 0, i64 0
  ret ptr %t10
L92:
  br label %L9
L9:
  %t11 = getelementptr [11 x i8], ptr @.str87, i64 0, i64 0
  ret ptr %t11
L93:
  br label %L10
L10:
  %t12 = getelementptr [9 x i8], ptr @.str88, i64 0, i64 0
  ret ptr %t12
L94:
  br label %L11
L11:
  %t13 = getelementptr [9 x i8], ptr @.str89, i64 0, i64 0
  ret ptr %t13
L95:
  br label %L12
L12:
  %t14 = getelementptr [10 x i8], ptr @.str90, i64 0, i64 0
  ret ptr %t14
L96:
  br label %L13
L13:
  %t15 = getelementptr [13 x i8], ptr @.str91, i64 0, i64 0
  ret ptr %t15
L97:
  br label %L14
L14:
  %t16 = getelementptr [11 x i8], ptr @.str92, i64 0, i64 0
  ret ptr %t16
L98:
  br label %L15
L15:
  %t17 = getelementptr [7 x i8], ptr @.str93, i64 0, i64 0
  ret ptr %t17
L99:
  br label %L16
L16:
  %t18 = getelementptr [9 x i8], ptr @.str94, i64 0, i64 0
  ret ptr %t18
L100:
  br label %L17
L17:
  %t19 = getelementptr [10 x i8], ptr @.str95, i64 0, i64 0
  ret ptr %t19
L101:
  br label %L18
L18:
  %t20 = getelementptr [8 x i8], ptr @.str96, i64 0, i64 0
  ret ptr %t20
L102:
  br label %L19
L19:
  %t21 = getelementptr [7 x i8], ptr @.str97, i64 0, i64 0
  ret ptr %t21
L103:
  br label %L20
L20:
  %t22 = getelementptr [11 x i8], ptr @.str98, i64 0, i64 0
  ret ptr %t22
L104:
  br label %L21
L21:
  %t23 = getelementptr [10 x i8], ptr @.str99, i64 0, i64 0
  ret ptr %t23
L105:
  br label %L22
L22:
  %t24 = getelementptr [13 x i8], ptr @.str100, i64 0, i64 0
  ret ptr %t24
L106:
  br label %L23
L23:
  %t25 = getelementptr [11 x i8], ptr @.str101, i64 0, i64 0
  ret ptr %t25
L107:
  br label %L24
L24:
  %t26 = getelementptr [10 x i8], ptr @.str102, i64 0, i64 0
  ret ptr %t26
L108:
  br label %L25
L25:
  %t27 = getelementptr [11 x i8], ptr @.str103, i64 0, i64 0
  ret ptr %t27
L109:
  br label %L26
L26:
  %t28 = getelementptr [9 x i8], ptr @.str104, i64 0, i64 0
  ret ptr %t28
L110:
  br label %L27
L27:
  %t29 = getelementptr [12 x i8], ptr @.str105, i64 0, i64 0
  ret ptr %t29
L111:
  br label %L28
L28:
  %t30 = getelementptr [9 x i8], ptr @.str106, i64 0, i64 0
  ret ptr %t30
L112:
  br label %L29
L29:
  %t31 = getelementptr [9 x i8], ptr @.str107, i64 0, i64 0
  ret ptr %t31
L113:
  br label %L30
L30:
  %t32 = getelementptr [12 x i8], ptr @.str108, i64 0, i64 0
  ret ptr %t32
L114:
  br label %L31
L31:
  %t33 = getelementptr [11 x i8], ptr @.str109, i64 0, i64 0
  ret ptr %t33
L115:
  br label %L32
L32:
  %t34 = getelementptr [11 x i8], ptr @.str110, i64 0, i64 0
  ret ptr %t34
L116:
  br label %L33
L33:
  %t35 = getelementptr [10 x i8], ptr @.str111, i64 0, i64 0
  ret ptr %t35
L117:
  br label %L34
L34:
  %t36 = getelementptr [13 x i8], ptr @.str112, i64 0, i64 0
  ret ptr %t36
L118:
  br label %L35
L35:
  %t37 = getelementptr [11 x i8], ptr @.str113, i64 0, i64 0
  ret ptr %t37
L119:
  br label %L36
L36:
  %t38 = getelementptr [9 x i8], ptr @.str114, i64 0, i64 0
  ret ptr %t38
L120:
  br label %L37
L37:
  %t39 = getelementptr [10 x i8], ptr @.str115, i64 0, i64 0
  ret ptr %t39
L121:
  br label %L38
L38:
  %t40 = getelementptr [9 x i8], ptr @.str116, i64 0, i64 0
  ret ptr %t40
L122:
  br label %L39
L39:
  %t41 = getelementptr [10 x i8], ptr @.str117, i64 0, i64 0
  ret ptr %t41
L123:
  br label %L40
L40:
  %t42 = getelementptr [12 x i8], ptr @.str118, i64 0, i64 0
  ret ptr %t42
L124:
  br label %L41
L41:
  %t43 = getelementptr [8 x i8], ptr @.str119, i64 0, i64 0
  ret ptr %t43
L125:
  br label %L42
L42:
  %t44 = getelementptr [9 x i8], ptr @.str120, i64 0, i64 0
  ret ptr %t44
L126:
  br label %L43
L43:
  %t45 = getelementptr [10 x i8], ptr @.str121, i64 0, i64 0
  ret ptr %t45
L127:
  br label %L44
L44:
  %t46 = getelementptr [10 x i8], ptr @.str122, i64 0, i64 0
  ret ptr %t46
L128:
  br label %L45
L45:
  %t47 = getelementptr [11 x i8], ptr @.str123, i64 0, i64 0
  ret ptr %t47
L129:
  br label %L46
L46:
  %t48 = getelementptr [11 x i8], ptr @.str124, i64 0, i64 0
  ret ptr %t48
L130:
  br label %L47
L47:
  %t49 = getelementptr [7 x i8], ptr @.str125, i64 0, i64 0
  ret ptr %t49
L131:
  br label %L48
L48:
  %t50 = getelementptr [8 x i8], ptr @.str126, i64 0, i64 0
  ret ptr %t50
L132:
  br label %L49
L49:
  %t51 = getelementptr [7 x i8], ptr @.str127, i64 0, i64 0
  ret ptr %t51
L133:
  br label %L50
L50:
  %t52 = getelementptr [7 x i8], ptr @.str128, i64 0, i64 0
  ret ptr %t52
L134:
  br label %L51
L51:
  %t53 = getelementptr [8 x i8], ptr @.str129, i64 0, i64 0
  ret ptr %t53
L135:
  br label %L52
L52:
  %t54 = getelementptr [8 x i8], ptr @.str130, i64 0, i64 0
  ret ptr %t54
L136:
  br label %L53
L53:
  %t55 = getelementptr [8 x i8], ptr @.str131, i64 0, i64 0
  ret ptr %t55
L137:
  br label %L54
L54:
  %t56 = getelementptr [7 x i8], ptr @.str132, i64 0, i64 0
  ret ptr %t56
L138:
  br label %L55
L55:
  %t57 = getelementptr [9 x i8], ptr @.str133, i64 0, i64 0
  ret ptr %t57
L139:
  br label %L56
L56:
  %t58 = getelementptr [11 x i8], ptr @.str134, i64 0, i64 0
  ret ptr %t58
L140:
  br label %L57
L57:
  %t59 = getelementptr [16 x i8], ptr @.str135, i64 0, i64 0
  ret ptr %t59
L141:
  br label %L58
L58:
  %t60 = getelementptr [17 x i8], ptr @.str136, i64 0, i64 0
  ret ptr %t60
L142:
  br label %L59
L59:
  %t61 = getelementptr [16 x i8], ptr @.str137, i64 0, i64 0
  ret ptr %t61
L143:
  br label %L60
L60:
  %t62 = getelementptr [17 x i8], ptr @.str138, i64 0, i64 0
  ret ptr %t62
L144:
  br label %L61
L61:
  %t63 = getelementptr [15 x i8], ptr @.str139, i64 0, i64 0
  ret ptr %t63
L145:
  br label %L62
L62:
  %t64 = getelementptr [16 x i8], ptr @.str140, i64 0, i64 0
  ret ptr %t64
L146:
  br label %L63
L63:
  %t65 = getelementptr [17 x i8], ptr @.str141, i64 0, i64 0
  ret ptr %t65
L147:
  br label %L64
L64:
  %t66 = getelementptr [18 x i8], ptr @.str142, i64 0, i64 0
  ret ptr %t66
L148:
  br label %L65
L65:
  %t67 = getelementptr [18 x i8], ptr @.str143, i64 0, i64 0
  ret ptr %t67
L149:
  br label %L66
L66:
  %t68 = getelementptr [19 x i8], ptr @.str144, i64 0, i64 0
  ret ptr %t68
L150:
  br label %L67
L67:
  %t69 = getelementptr [8 x i8], ptr @.str145, i64 0, i64 0
  ret ptr %t69
L151:
  br label %L68
L68:
  %t70 = getelementptr [8 x i8], ptr @.str146, i64 0, i64 0
  ret ptr %t70
L152:
  br label %L69
L69:
  %t71 = getelementptr [10 x i8], ptr @.str147, i64 0, i64 0
  ret ptr %t71
L153:
  br label %L70
L70:
  %t72 = getelementptr [8 x i8], ptr @.str148, i64 0, i64 0
  ret ptr %t72
L154:
  br label %L71
L71:
  %t73 = getelementptr [13 x i8], ptr @.str149, i64 0, i64 0
  ret ptr %t73
L155:
  br label %L72
L72:
  %t74 = getelementptr [10 x i8], ptr @.str150, i64 0, i64 0
  ret ptr %t74
L156:
  br label %L73
L73:
  %t75 = getelementptr [11 x i8], ptr @.str151, i64 0, i64 0
  ret ptr %t75
L157:
  br label %L74
L74:
  %t76 = getelementptr [11 x i8], ptr @.str152, i64 0, i64 0
  ret ptr %t76
L158:
  br label %L75
L75:
  %t77 = getelementptr [11 x i8], ptr @.str153, i64 0, i64 0
  ret ptr %t77
L159:
  br label %L76
L76:
  %t78 = getelementptr [11 x i8], ptr @.str154, i64 0, i64 0
  ret ptr %t78
L160:
  br label %L77
L77:
  %t79 = getelementptr [13 x i8], ptr @.str155, i64 0, i64 0
  ret ptr %t79
L161:
  br label %L78
L78:
  %t80 = getelementptr [13 x i8], ptr @.str156, i64 0, i64 0
  ret ptr %t80
L162:
  br label %L79
L79:
  %t81 = getelementptr [14 x i8], ptr @.str157, i64 0, i64 0
  ret ptr %t81
L163:
  br label %L80
L80:
  %t82 = getelementptr [10 x i8], ptr @.str158, i64 0, i64 0
  ret ptr %t82
L164:
  br label %L81
L81:
  %t83 = getelementptr [13 x i8], ptr @.str159, i64 0, i64 0
  ret ptr %t83
L165:
  br label %L82
L82:
  %t84 = getelementptr [8 x i8], ptr @.str160, i64 0, i64 0
  ret ptr %t84
L166:
  br label %L83
L83:
  %t85 = getelementptr [12 x i8], ptr @.str161, i64 0, i64 0
  ret ptr %t85
L167:
  br label %L0
L84:
  %t86 = getelementptr [4 x i8], ptr @.str162, i64 0, i64 0
  ret ptr %t86
L168:
  br label %L0
L0:
  ret ptr null
}

@.str0 = private unnamed_addr constant [7 x i8] c"malloc\00"
@.str1 = private unnamed_addr constant [4 x i8] c"int\00"
@.str2 = private unnamed_addr constant [5 x i8] c"char\00"
@.str3 = private unnamed_addr constant [6 x i8] c"float\00"
@.str4 = private unnamed_addr constant [7 x i8] c"double\00"
@.str5 = private unnamed_addr constant [5 x i8] c"void\00"
@.str6 = private unnamed_addr constant [5 x i8] c"long\00"
@.str7 = private unnamed_addr constant [6 x i8] c"short\00"
@.str8 = private unnamed_addr constant [9 x i8] c"unsigned\00"
@.str9 = private unnamed_addr constant [7 x i8] c"signed\00"
@.str10 = private unnamed_addr constant [3 x i8] c"if\00"
@.str11 = private unnamed_addr constant [5 x i8] c"else\00"
@.str12 = private unnamed_addr constant [6 x i8] c"while\00"
@.str13 = private unnamed_addr constant [4 x i8] c"for\00"
@.str14 = private unnamed_addr constant [3 x i8] c"do\00"
@.str15 = private unnamed_addr constant [7 x i8] c"return\00"
@.str16 = private unnamed_addr constant [6 x i8] c"break\00"
@.str17 = private unnamed_addr constant [9 x i8] c"continue\00"
@.str18 = private unnamed_addr constant [7 x i8] c"switch\00"
@.str19 = private unnamed_addr constant [5 x i8] c"case\00"
@.str20 = private unnamed_addr constant [8 x i8] c"default\00"
@.str21 = private unnamed_addr constant [5 x i8] c"goto\00"
@.str22 = private unnamed_addr constant [7 x i8] c"struct\00"
@.str23 = private unnamed_addr constant [6 x i8] c"union\00"
@.str24 = private unnamed_addr constant [5 x i8] c"enum\00"
@.str25 = private unnamed_addr constant [8 x i8] c"typedef\00"
@.str26 = private unnamed_addr constant [7 x i8] c"static\00"
@.str27 = private unnamed_addr constant [7 x i8] c"extern\00"
@.str28 = private unnamed_addr constant [6 x i8] c"const\00"
@.str29 = private unnamed_addr constant [9 x i8] c"volatile\00"
@.str30 = private unnamed_addr constant [7 x i8] c"sizeof\00"
@.str31 = private unnamed_addr constant [7 x i8] c"calloc\00"
@.str32 = private unnamed_addr constant [1 x i8] c"\00"
@.str33 = private unnamed_addr constant [3 x i8] c"++\00"
@.str34 = private unnamed_addr constant [3 x i8] c"+=\00"
@.str35 = private unnamed_addr constant [2 x i8] c"+\00"
@.str36 = private unnamed_addr constant [3 x i8] c"--\00"
@.str37 = private unnamed_addr constant [3 x i8] c"-=\00"
@.str38 = private unnamed_addr constant [3 x i8] c"->\00"
@.str39 = private unnamed_addr constant [2 x i8] c"-\00"
@.str40 = private unnamed_addr constant [3 x i8] c"*=\00"
@.str41 = private unnamed_addr constant [2 x i8] c"*\00"
@.str42 = private unnamed_addr constant [3 x i8] c"/=\00"
@.str43 = private unnamed_addr constant [2 x i8] c"/\00"
@.str44 = private unnamed_addr constant [3 x i8] c"%=\00"
@.str45 = private unnamed_addr constant [2 x i8] c"%\00"
@.str46 = private unnamed_addr constant [3 x i8] c"&&\00"
@.str47 = private unnamed_addr constant [3 x i8] c"&=\00"
@.str48 = private unnamed_addr constant [2 x i8] c"&\00"
@.str49 = private unnamed_addr constant [3 x i8] c"||\00"
@.str50 = private unnamed_addr constant [3 x i8] c"|=\00"
@.str51 = private unnamed_addr constant [2 x i8] c"|\00"
@.str52 = private unnamed_addr constant [3 x i8] c"^=\00"
@.str53 = private unnamed_addr constant [2 x i8] c"^\00"
@.str54 = private unnamed_addr constant [2 x i8] c"~\00"
@.str55 = private unnamed_addr constant [4 x i8] c"<<=\00"
@.str56 = private unnamed_addr constant [3 x i8] c"<<\00"
@.str57 = private unnamed_addr constant [3 x i8] c"<=\00"
@.str58 = private unnamed_addr constant [2 x i8] c"<\00"
@.str59 = private unnamed_addr constant [4 x i8] c">>=\00"
@.str60 = private unnamed_addr constant [3 x i8] c">>\00"
@.str61 = private unnamed_addr constant [3 x i8] c">=\00"
@.str62 = private unnamed_addr constant [2 x i8] c">\00"
@.str63 = private unnamed_addr constant [3 x i8] c"==\00"
@.str64 = private unnamed_addr constant [2 x i8] c"=\00"
@.str65 = private unnamed_addr constant [3 x i8] c"!=\00"
@.str66 = private unnamed_addr constant [2 x i8] c"!\00"
@.str67 = private unnamed_addr constant [4 x i8] c"...\00"
@.str68 = private unnamed_addr constant [2 x i8] c".\00"
@.str69 = private unnamed_addr constant [2 x i8] c"(\00"
@.str70 = private unnamed_addr constant [2 x i8] c")\00"
@.str71 = private unnamed_addr constant [2 x i8] c"{\00"
@.str72 = private unnamed_addr constant [2 x i8] c"}\00"
@.str73 = private unnamed_addr constant [2 x i8] c"[\00"
@.str74 = private unnamed_addr constant [2 x i8] c"]\00"
@.str75 = private unnamed_addr constant [2 x i8] c";\00"
@.str76 = private unnamed_addr constant [2 x i8] c",\00"
@.str77 = private unnamed_addr constant [2 x i8] c"?\00"
@.str78 = private unnamed_addr constant [2 x i8] c":\00"
@.str79 = private unnamed_addr constant [12 x i8] c"TOK_INT_LIT\00"
@.str80 = private unnamed_addr constant [14 x i8] c"TOK_FLOAT_LIT\00"
@.str81 = private unnamed_addr constant [13 x i8] c"TOK_CHAR_LIT\00"
@.str82 = private unnamed_addr constant [15 x i8] c"TOK_STRING_LIT\00"
@.str83 = private unnamed_addr constant [10 x i8] c"TOK_IDENT\00"
@.str84 = private unnamed_addr constant [8 x i8] c"TOK_INT\00"
@.str85 = private unnamed_addr constant [9 x i8] c"TOK_CHAR\00"
@.str86 = private unnamed_addr constant [10 x i8] c"TOK_FLOAT\00"
@.str87 = private unnamed_addr constant [11 x i8] c"TOK_DOUBLE\00"
@.str88 = private unnamed_addr constant [9 x i8] c"TOK_VOID\00"
@.str89 = private unnamed_addr constant [9 x i8] c"TOK_LONG\00"
@.str90 = private unnamed_addr constant [10 x i8] c"TOK_SHORT\00"
@.str91 = private unnamed_addr constant [13 x i8] c"TOK_UNSIGNED\00"
@.str92 = private unnamed_addr constant [11 x i8] c"TOK_SIGNED\00"
@.str93 = private unnamed_addr constant [7 x i8] c"TOK_IF\00"
@.str94 = private unnamed_addr constant [9 x i8] c"TOK_ELSE\00"
@.str95 = private unnamed_addr constant [10 x i8] c"TOK_WHILE\00"
@.str96 = private unnamed_addr constant [8 x i8] c"TOK_FOR\00"
@.str97 = private unnamed_addr constant [7 x i8] c"TOK_DO\00"
@.str98 = private unnamed_addr constant [11 x i8] c"TOK_RETURN\00"
@.str99 = private unnamed_addr constant [10 x i8] c"TOK_BREAK\00"
@.str100 = private unnamed_addr constant [13 x i8] c"TOK_CONTINUE\00"
@.str101 = private unnamed_addr constant [11 x i8] c"TOK_STRUCT\00"
@.str102 = private unnamed_addr constant [10 x i8] c"TOK_UNION\00"
@.str103 = private unnamed_addr constant [11 x i8] c"TOK_SWITCH\00"
@.str104 = private unnamed_addr constant [9 x i8] c"TOK_CASE\00"
@.str105 = private unnamed_addr constant [12 x i8] c"TOK_DEFAULT\00"
@.str106 = private unnamed_addr constant [9 x i8] c"TOK_GOTO\00"
@.str107 = private unnamed_addr constant [9 x i8] c"TOK_ENUM\00"
@.str108 = private unnamed_addr constant [12 x i8] c"TOK_TYPEDEF\00"
@.str109 = private unnamed_addr constant [11 x i8] c"TOK_STATIC\00"
@.str110 = private unnamed_addr constant [11 x i8] c"TOK_EXTERN\00"
@.str111 = private unnamed_addr constant [10 x i8] c"TOK_CONST\00"
@.str112 = private unnamed_addr constant [13 x i8] c"TOK_VOLATILE\00"
@.str113 = private unnamed_addr constant [11 x i8] c"TOK_SIZEOF\00"
@.str114 = private unnamed_addr constant [9 x i8] c"TOK_PLUS\00"
@.str115 = private unnamed_addr constant [10 x i8] c"TOK_MINUS\00"
@.str116 = private unnamed_addr constant [9 x i8] c"TOK_STAR\00"
@.str117 = private unnamed_addr constant [10 x i8] c"TOK_SLASH\00"
@.str118 = private unnamed_addr constant [12 x i8] c"TOK_PERCENT\00"
@.str119 = private unnamed_addr constant [8 x i8] c"TOK_AMP\00"
@.str120 = private unnamed_addr constant [9 x i8] c"TOK_PIPE\00"
@.str121 = private unnamed_addr constant [10 x i8] c"TOK_CARET\00"
@.str122 = private unnamed_addr constant [10 x i8] c"TOK_TILDE\00"
@.str123 = private unnamed_addr constant [11 x i8] c"TOK_LSHIFT\00"
@.str124 = private unnamed_addr constant [11 x i8] c"TOK_RSHIFT\00"
@.str125 = private unnamed_addr constant [7 x i8] c"TOK_EQ\00"
@.str126 = private unnamed_addr constant [8 x i8] c"TOK_NEQ\00"
@.str127 = private unnamed_addr constant [7 x i8] c"TOK_LT\00"
@.str128 = private unnamed_addr constant [7 x i8] c"TOK_GT\00"
@.str129 = private unnamed_addr constant [8 x i8] c"TOK_LEQ\00"
@.str130 = private unnamed_addr constant [8 x i8] c"TOK_GEQ\00"
@.str131 = private unnamed_addr constant [8 x i8] c"TOK_AND\00"
@.str132 = private unnamed_addr constant [7 x i8] c"TOK_OR\00"
@.str133 = private unnamed_addr constant [9 x i8] c"TOK_BANG\00"
@.str134 = private unnamed_addr constant [11 x i8] c"TOK_ASSIGN\00"
@.str135 = private unnamed_addr constant [16 x i8] c"TOK_PLUS_ASSIGN\00"
@.str136 = private unnamed_addr constant [17 x i8] c"TOK_MINUS_ASSIGN\00"
@.str137 = private unnamed_addr constant [16 x i8] c"TOK_STAR_ASSIGN\00"
@.str138 = private unnamed_addr constant [17 x i8] c"TOK_SLASH_ASSIGN\00"
@.str139 = private unnamed_addr constant [15 x i8] c"TOK_AMP_ASSIGN\00"
@.str140 = private unnamed_addr constant [16 x i8] c"TOK_PIPE_ASSIGN\00"
@.str141 = private unnamed_addr constant [17 x i8] c"TOK_CARET_ASSIGN\00"
@.str142 = private unnamed_addr constant [18 x i8] c"TOK_LSHIFT_ASSIGN\00"
@.str143 = private unnamed_addr constant [18 x i8] c"TOK_RSHIFT_ASSIGN\00"
@.str144 = private unnamed_addr constant [19 x i8] c"TOK_PERCENT_ASSIGN\00"
@.str145 = private unnamed_addr constant [8 x i8] c"TOK_INC\00"
@.str146 = private unnamed_addr constant [8 x i8] c"TOK_DEC\00"
@.str147 = private unnamed_addr constant [10 x i8] c"TOK_ARROW\00"
@.str148 = private unnamed_addr constant [8 x i8] c"TOK_DOT\00"
@.str149 = private unnamed_addr constant [13 x i8] c"TOK_QUESTION\00"
@.str150 = private unnamed_addr constant [10 x i8] c"TOK_COLON\00"
@.str151 = private unnamed_addr constant [11 x i8] c"TOK_LPAREN\00"
@.str152 = private unnamed_addr constant [11 x i8] c"TOK_RPAREN\00"
@.str153 = private unnamed_addr constant [11 x i8] c"TOK_LBRACE\00"
@.str154 = private unnamed_addr constant [11 x i8] c"TOK_RBRACE\00"
@.str155 = private unnamed_addr constant [13 x i8] c"TOK_LBRACKET\00"
@.str156 = private unnamed_addr constant [13 x i8] c"TOK_RBRACKET\00"
@.str157 = private unnamed_addr constant [14 x i8] c"TOK_SEMICOLON\00"
@.str158 = private unnamed_addr constant [10 x i8] c"TOK_COMMA\00"
@.str159 = private unnamed_addr constant [13 x i8] c"TOK_ELLIPSIS\00"
@.str160 = private unnamed_addr constant [8 x i8] c"TOK_EOF\00"
@.str161 = private unnamed_addr constant [12 x i8] c"TOK_UNKNOWN\00"
@.str162 = private unnamed_addr constant [4 x i8] c"???\00"
