; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_struct_array.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_struct_array.c"
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


define dso_local i32 @test_struct_array() {
entry:
  %t0 = alloca [8 x i8]
  %t1 = load ptr, ptr %t0
  %t3 = sext i32 0 to i64
  %t2 = getelementptr ptr, ptr %t1, i64 %t3
  %t4 = getelementptr i8, ptr %t2, i64 0
  %t5 = sext i32 1 to i64
  store i64 %t5, ptr %t4
  %t6 = load ptr, ptr %t0
  %t8 = sext i32 1 to i64
  %t7 = getelementptr ptr, ptr %t6, i64 %t8
  %t9 = getelementptr i8, ptr %t7, i64 0
  %t10 = sext i32 2 to i64
  store i64 %t10, ptr %t9
  %t11 = load ptr, ptr %t0
  %t13 = sext i32 2 to i64
  %t12 = getelementptr ptr, ptr %t11, i64 %t13
  %t14 = getelementptr i8, ptr %t12, i64 0
  %t15 = sext i32 3 to i64
  store i64 %t15, ptr %t14
  %t16 = load ptr, ptr %t0
  %t18 = sext i32 0 to i64
  %t17 = getelementptr ptr, ptr %t16, i64 %t18
  %t19 = getelementptr i8, ptr %t17, i64 0
  %t20 = load i64, ptr %t19
  %t21 = load ptr, ptr %t0
  %t23 = sext i32 1 to i64
  %t22 = getelementptr ptr, ptr %t21, i64 %t23
  %t24 = getelementptr i8, ptr %t22, i64 0
  %t25 = load i64, ptr %t24
  %t26 = add i64 %t20, %t25
  %t27 = load ptr, ptr %t0
  %t29 = sext i32 2 to i64
  %t28 = getelementptr ptr, ptr %t27, i64 %t29
  %t30 = getelementptr i8, ptr %t28, i64 0
  %t31 = load i64, ptr %t30
  %t32 = add i64 %t26, %t31
  %t33 = trunc i64 %t32 to i32
  ret i32 %t33
L0:
  ret i32 0
}

define dso_local i32 @main() {
entry:
  %t0 = call i32 @test_struct_array()
  %t1 = sext i32 %t0 to i64
  %t2 = trunc i64 %t1 to i32
  ret i32 %t2
L0:
  ret i32 0
}

