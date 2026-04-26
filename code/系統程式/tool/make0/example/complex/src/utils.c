#include "utils.h"
#include <stdio.h>

void print_result(const char* op, int result) {
    printf("%s = %d\n", op, result);
}

void log_message(const char* msg) {
    printf("[LOG] %s\n", msg);
}
