/* stub stdarg.h – use real builtins so va_list is properly typed */
#ifndef _STUB_STDARG_H
#define _STUB_STDARG_H
typedef __builtin_va_list va_list;
#define va_start(ap, last)  __builtin_va_start(ap, last)
#define va_arg(ap, T)       __builtin_va_arg(ap, T)
#define va_end(ap)          __builtin_va_end(ap)
#define va_copy(dst, src)   __builtin_va_copy(dst, src)
#endif
