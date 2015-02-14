#ifndef DBG_HPP
#define DBG_HPP

#include <cstdio>

#ifndef NDEBUG

#define DBG_Decl(type, ...) type __VA_ARGS__
#define DBG_Eval(x) x
#define DBG_LogLn(line) fprintf(stderr, "%s\n", line);
#define DBG_LogFmt(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__);

#else

#define DBG_Decl(type, ...)
#define DBG_Eval(x)
#define DBG_LogLn(line)
#define DBG_LogFmt(fmt, ...)

#endif

#endif
