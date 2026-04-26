#ifndef MACRO_H
#define MACRO_H

/*
 * Simple C preprocessor:
 *   - #define NAME value  (object-like macros)
 *   - #define NAME(args) body  (function-like macros, basic)
 *   - #undef NAME
 *   - #ifdef / #ifndef / #else / #endif
 *   - #include "file" and #include <file>
 *   - #pragma (silently ignored)
 *   - line continuation (backslash-newline)
 *
 * Returns heap-allocated preprocessed source string.
 * The caller is responsible for free()ing it.
 */
char *macro_preprocess(const char *src, const char *filename,
                        int include_depth);

#endif /* MACRO_H */
