#pragma once

/**
 * Define overloaded ++ and -- operators for the enum T.
 *
 * This macro produces several inline function definitions; use it only at
 * file/namespace scope. It requires a trailing semicolon.
 *
 * @param T A type expression naming the enum type to augument. Evaluated
 *          several times.
 */
#define DEF_ENUM_INC(T) \
    static inline T &operator++(T &x) { return x = static_cast<T>(x + 1); } \
    static inline T &operator--(T &x) { return x = static_cast<T>(x - 1); } \
    static inline T operator++(T &x, int) { T y = x; ++x; return y; } \
    static inline T operator--(T &x, int) { T y = x; --x; return y; } \
    COMPILE_CHECK(is_enum<T>::value)

DEF_ENUM_INC(monster_type);

