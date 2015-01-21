#ifndef DBG_H
#define DBG_H

#ifndef NDEBUG

#define DBG_Decl(type, ...) type __VA_ARGS__
#define DBG_Eval(x) x

#else

#define DBG_Decl(type, ...)
#define DBG_Eval(x)

#endif

#endif
