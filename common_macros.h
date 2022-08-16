#pragma once

#define ALWAYS_INLINE __attribute__((__always_inline__))

#define NO_INLINE __attribute__((__noinline__))

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)


#define IS_CONSTANT_FOR_OPTIMIZER(exp) __builtin_constant_p(exp)

#ifndef __cplusplus

#define ARRAY_LENGTH(a) ({ _Static_assert(!__builtin_types_compatible_p(                               \
                                                  __typeof__((a)+0),                                   \
                                                  __typeof__(a)),                                      \
                           "ARRAY_LENGTH is only for actual array");                                   \
                           sizeof(a) / sizeof((a)[0]); })
#else


template<typename T, decltype(sizeof 0) N>
constexpr bool
ARRAY_LENGTH_helper_is_array(T const (&)[N])
{
    return 1;
}

template<typename T>
constexpr bool
ARRAY_LENGTH_helper_is_array(T)
{
    return 0;
}


#define ARRAY_LENGTH(a) ({ static_assert(ARRAY_LENGTH_helper_is_array(a),     \
                           "ARRAY_LENGTH is only for actual array");          \
                           sizeof(a) / sizeof((a)[0]); })

#endif

