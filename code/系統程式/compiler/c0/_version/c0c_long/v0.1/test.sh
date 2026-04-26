set -x
./c0run.sh char_literal
./c0run.sh factorial
./c0run.sh loop_break
./c0run.sh optest
./c0run.sh ptr_test
./c0run.sh char_test
./c0run.sh loop
./c0run.sh ptr_arith
./c0run.sh ptr_diff
./c0run.sh sizeof_test
./c0run.sh array_init
./c0run.sh array_init2
./c0run.sh scope
./c0run.sh proto_void
./c0run.sh switch_test
./c0run.sh do_while
./c0run.sh struct_test
./c0run.sh typedef_struct
./c0run.sh string_ptr
./c0run.sh float_test
./c0run.sh double_test
./c0run.sh type_test
./c0run.sh cast_test
./c0c c/error_line.c -o ll/error_line.ll
./c0c c/error_syntax1.c -o ll/error_syntax1.ll
./c0c c/error_syntax2.c -o ll/error_syntax2.ll