#ifndef _COMMON_H
#define _COMMON_H

#define GET_FIRST_ARG(N, ...) N
#define GET_OVERLOADED_MACRO12(_1, _2, ...) GET_FIRST_ARG(__VA_ARGS__, 0)

#endif // _COMMON_H
