; ModuleID = '/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_array.c'
source_filename = "/Users/Shared/ccc/ccc114b/cccocw/_code/c0py/_data/test_array.c"
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


define dso_local i32 @test_array_1d() {
entry:
  %t0 = alloca ptr
  %t1 = load ptr, ptr %t0
  %t3 = sext i32 0 to i64
  %t2 = getelementptr ptr, ptr %t1, i64 %t3
  %t4 = sext i32 10 to i64
  store i64 %t4, ptr %t2
  %t5 = load ptr, ptr %t0
  %t7 = sext i32 1 to i64
  %t6 = getelementptr ptr, ptr %t5, i64 %t7
  %t8 = sext i32 20 to i64
  store i64 %t8, ptr %t6
  %t9 = load ptr, ptr %t0
  %t11 = sext i32 2 to i64
  %t10 = getelementptr ptr, ptr %t9, i64 %t11
  %t12 = sext i32 30 to i64
  store i64 %t12, ptr %t10
  %t13 = load ptr, ptr %t0
  %t14 = sext i32 0 to i64
  %t15 = getelementptr ptr, ptr %t13, i64 %t14
  %t16 = load ptr, ptr %t15
  %t17 = load ptr, ptr %t0
  %t18 = sext i32 1 to i64
  %t19 = getelementptr ptr, ptr %t17, i64 %t18
  %t20 = load ptr, ptr %t19
  %t22 = ptrtoint ptr %t16 to i64
  %t23 = ptrtoint ptr %t20 to i64
  %t24 = inttoptr i64 %t22 to ptr
  %t21 = getelementptr i8, ptr %t24, i64 %t23
  %t25 = load ptr, ptr %t0
  %t26 = sext i32 2 to i64
  %t27 = getelementptr ptr, ptr %t25, i64 %t26
  %t28 = load ptr, ptr %t27
  %t30 = ptrtoint ptr %t21 to i64
  %t31 = ptrtoint ptr %t28 to i64
  %t32 = inttoptr i64 %t30 to ptr
  %t29 = getelementptr i8, ptr %t32, i64 %t31
  %t33 = ptrtoint ptr %t29 to i64
  %t34 = trunc i64 %t33 to i32
  ret i32 %t34
L0:
  ret i32 0
}

define dso_local i32 @main() {
entry:
  %t0 = call i32 @test_array_1d()
  %t1 = sext i32 %t0 to i64
  %t2 = trunc i64 %t1 to i32
  ret i32 %t2
L0:
  ret i32 0
}

