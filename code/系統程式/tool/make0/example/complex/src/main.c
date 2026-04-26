#include <stdio.h>
#include "calc.h"
#include "utils.h"

int main() {
    log_message("Calculator Demo");

    int a = 10, b = 5;

    print_result("10 + 5", add(a, b));
    print_result("10 - 5", subtract(a, b));
    print_result("10 * 5", multiply(a, b));
    print_result("10 / 5", divide(a, b));

    return 0;
}
