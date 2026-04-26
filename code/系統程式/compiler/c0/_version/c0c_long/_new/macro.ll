; ModuleID = 'c0c'
declare void @macro_init()

declare void @macro_free()

declare i32 @macro_parse_line(ptr %line, i32 %line_num)

declare ptr @macro_expand(ptr %input)

declare ptr @macro_expand_file(ptr %filename)

declare void @macro_define(ptr %name, ptr %replacement)

declare void @macro_undef(ptr %name)

declare i32 @macro_defined(ptr %name)

declare void @macro_enable_expand()

declare void @macro_disable_expand()

