#pragma once

#ifndef __cplusplus
_Static_assert(((unsigned char)-1) == 255, "failed assert UCHAR_MAX");
_Static_assert(sizeof(short) == 2, "failed assert sizeof(short) is 2");
_Static_assert(((unsigned short)-1) == 65535, "failed assert USHRT_MAX");
_Static_assert(sizeof(int) == 4, "failed assert sizeof(int) is 4");
_Static_assert(((unsigned int)-1) == 4294967295L, "failed assert UINT_MAX");
_Static_assert(sizeof(long long) == 8, "failed assert sizeof(long long) is 8");
_Static_assert(((unsigned long long)-1) == 18446744073709551615ULL, "failed assert ULLONG_MAX");
#else
static_assert(static_cast<unsigned char>(-1) == 255, "failed assert UCHAR_MAX");
static_assert(sizeof(short) == 2, "failed assert sizeof(short) is 2");
static_assert(static_cast<unsigned short>(-1) == 65535, "failed assert USHRT_MAX");
static_assert(sizeof(int) == 4, "failed assert sizeof(int) is 4");
static_assert(static_cast<unsigned int>(-1) == 4294967295L, "failed assert UINT_MAX");
static_assert(sizeof(long long) == 8, "failed assert sizeof(long long) is 8");
static_assert(static_cast<unsigned long long>(-1) == 18446744073709551615ULL, "failed assert ULLONG_MAX");
#endif

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


// true for array arg, false for pointer
// used for static_assert
template<class T>
constexpr bool
ARRAY_LENGTH_helper_is_array(const T&, const T&)
{
    return false;
}

template<class T, class U>
constexpr bool
ARRAY_LENGTH_helper_is_array(T, U)
{
    return true;
}



#define ARRAY_LENGTH(a) ({ static_assert(ARRAY_LENGTH_helper_is_array((a),(a)+0),     \
                           "ARRAY_LENGTH is only for actual array");                  \
                           sizeof(a) / sizeof((a)[0]); })

#endif


#define COMPILER_BARRIER() __asm__ __volatile__ ("":::"memory")


#define MAX_SIMPLE(a,b) ((b)>(a)?(b):(a))
#define MIN_SIMPLE(a,b) ((b)<(a)?(b):(a))


#ifndef __cplusplus

// use __auto_type to prevent self initialization
// every arg is evaluated once
// unfortunately this macro doesn't work for C++
#define MAX_MIN_cmp_helper(a,b,op)  ({ __auto_type s =                       \
                (struct { __typeof__(a) _a; __typeof__(b) _b; })             \
                { ._a = (a), ._b = (b) };                                    \
                (s._b op s._a) ? s._b : s._a; })

#define COMMON_MACROS_helper_is_constexpr_helper(x) _Generic((x), int*:1,void*:0)
#define COMMON_MACROS_helper_is_constexpr(x) COMMON_MACROS_helper_is_constexpr_helper( \
                1?(void*)((long long)(x)*0LL):(int*)0)

#define MAX_eval_once_helper(a,b)  MAX_MIN_cmp_helper(a,b,>)
#define MIN_eval_once_helper(a,b)  MAX_MIN_cmp_helper(a,b,<)

#define MAX(a,b) __builtin_choose_expr(                          \
                COMMON_MACROS_helper_is_constexpr(a)             \
                && COMMON_MACROS_helper_is_constexpr(b),         \
                MAX_SIMPLE(a,b),                                 \
                MAX_eval_once_helper(a,b))

#define MIN(a,b) __builtin_choose_expr(                          \
                COMMON_MACROS_helper_is_constexpr(a)             \
                && COMMON_MACROS_helper_is_constexpr(b),         \
                MIN_SIMPLE(a,b),                                 \
                MIN_eval_once_helper(a,b))


#else

// some SFINAE boilerplates

// integral constant
template<class T, T v>
struct COMMON_MACROS_helper_integral_constant { static constexpr T value = v; };

// bool type
using COMMON_MACROS_helper_true_type = COMMON_MACROS_helper_integral_constant<bool, true>;
using COMMON_MACROS_helper_false_type = COMMON_MACROS_helper_integral_constant<bool, false>;

// the enable_if
template<bool, class T = void>
struct COMMON_MACROS_helper_enable_if {};
template<class T>
struct COMMON_MACROS_helper_enable_if<true, T> { typedef T type; };

// are T and U the same type
template<class T, class U>
struct COMMON_MACROS_helper_type_is_same : COMMON_MACROS_helper_false_type {};
template<class T>
struct COMMON_MACROS_helper_type_is_same<T, T> : COMMON_MACROS_helper_true_type {};

// remove reference
template<class T> struct COMMON_MACROS_helper_remove_reference { typedef T type; };
template<class T> struct COMMON_MACROS_helper_remove_reference<T&> { typedef T type; };
template<class T> struct COMMON_MACROS_helper_remove_reference<T&&> { typedef T type; };

// for perfect forwarding
template<class T>
constexpr T&& COMMON_MACROS_helper_forward(
    typename COMMON_MACROS_helper_remove_reference<T>::type& t
) noexcept { return static_cast<T&&>(t); }

// remove_cv
template<class T> struct COMMON_MACROS_helper_remove_cv { typedef T type; };
template<class T> struct COMMON_MACROS_helper_remove_cv<const T> { typedef T type; };
template<class T> struct COMMON_MACROS_helper_remove_cv<volatile T> { typedef T type; };
template<class T> struct COMMON_MACROS_helper_remove_cv<const volatile T> { typedef T type; };

// remove reference and cv
template<class T> struct COMMON_MACROS_helper_remove_reference_cv
{
    typedef typename COMMON_MACROS_helper_remove_cv<typename COMMON_MACROS_helper_remove_reference<T>::type>::type type;
};

// are T and U same type after remove_reference and remove_cv
template<class T, class U>
struct COMMON_MACROS_helper_type_is_same_after_remove_reference_cv :
    COMMON_MACROS_helper_type_is_same<
    typename COMMON_MACROS_helper_remove_reference_cv<T>::type,
    typename COMMON_MACROS_helper_remove_reference_cv<T>::type> {};

// are T and U same type after remove_reference
template<class T, class U>
struct COMMON_MACROS_helper_type_is_same_after_remove_reference :
    COMMON_MACROS_helper_type_is_same<
    typename COMMON_MACROS_helper_remove_reference<T>::type,
    typename COMMON_MACROS_helper_remove_reference<U>::type> {};

// only defined if T and U are same type after remove reference and cv
// remove reference but retain cv

// this is just a helper template
template<class T, class U> struct COMMON_MACROS_helper_remove_reference_retain_cv_helper1 { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper1<T, T>
{ typedef T type; };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const T, T>
{ typedef const T type; };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper1<volatile T, T>
{ typedef volatile T type; };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const volatile T, T>
{ typedef const volatile T type; };

// another helper template
template<class T, class U> struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2 :
    COMMON_MACROS_helper_remove_reference_retain_cv_helper1<T, U> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<T, T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<T, T> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<T, const T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const T, T> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<T, volatile T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<volatile T, T> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<T, const volatile T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const volatile T, T> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<const T, volatile T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const volatile T, T> { };
template<class T>
struct COMMON_MACROS_helper_remove_reference_retain_cv_helper2<volatile T, const T> :
COMMON_MACROS_helper_remove_reference_retain_cv_helper1<const volatile T, T> { };

// now define COMMON_MACROS_helper_remove_reference_retain_cv
template<class T, class U> struct COMMON_MACROS_helper_remove_reference_retain_cv :
    COMMON_MACROS_helper_remove_reference_retain_cv_helper2<
    typename COMMON_MACROS_helper_remove_reference<T>::type,
    typename COMMON_MACROS_helper_remove_reference<U>::type
    > { };


template<class T, class U>
ALWAYS_INLINE
inline auto
constexpr
MAX_MIN_macro_helper(T&& a, U&& b, int maxOrMin)
    -> typename COMMON_MACROS_helper_remove_reference_retain_cv<T, U>::type
{
    bool cond = false;
    if (maxOrMin == 1) {
        cond = (b > a);
    } else {
        cond = (b < a);
    }
    if (cond) {
        return COMMON_MACROS_helper_forward<U>(b);
    } else {
        return COMMON_MACROS_helper_forward<T>(a);
    }
}

template<class T, class U>
ALWAYS_INLINE
inline auto
constexpr
MAX_MIN_macro_helper(const T& a, const U& b, int maxOrMin) -> decltype(0?a:b)
{
    bool cond = false;
    if (maxOrMin == 1) {
        cond = (b > a);
    } else {
        cond = (b < a);
    }
    if (cond) {
        return b;
    } else {
        return a;
    }
}

template<class T> struct COMMON_MACROS_helper_is_const : COMMON_MACROS_helper_false_type {};
template<class T> struct COMMON_MACROS_helper_is_const<const T> : COMMON_MACROS_helper_true_type {};


template<class T>
ALWAYS_INLINE
inline auto
constexpr
MAX_MIN_macro_helper(T &a, T &b, int maxOrMin) ->
    typename COMMON_MACROS_helper_enable_if<
        ! COMMON_MACROS_helper_is_const<T>::value,
        T&
    >::type
{
    bool cond = false;
    if (maxOrMin == 1) {
        cond = (b > a);
    } else {
        cond = (b < a);
    }
    if (cond) {
        return b;
    } else {
        return a;
    }
}

#define MAX(a,b) MAX_MIN_macro_helper(a,b,1)
#define MIN(a,b) MAX_MIN_macro_helper(a,b,2)

#endif

#ifndef __cplusplus
#define COMMON_MACROS_reinterpret_cast_helper(T,a) ((T)(a))
#define COMMON_MACROS_static_cast_helper(T,a) ((T)(a))
#else
#define COMMON_MACROS_reinterpret_cast_helper(T,a) (reinterpret_cast<T>(a))
#define COMMON_MACROS_static_cast_helper(T,a) (static_cast<T>(a))
#endif

ALWAYS_INLINE
static inline int
COMMON_MACROS_is_pointer_align(void *p, unsigned n)
{
    return COMMON_MACROS_reinterpret_cast_helper(unsigned long long, p) % n == 0;
}

static void
COMMON_MACROS_explicit_bzero(void *a, __typeof__(sizeof 0) n)
{
    char *p = COMMON_MACROS_reinterpret_cast_helper(char*, a);
    if (UNLIKELY(n <= 16)) {
        if (n == 0) {
            return;
        }
        if (n == 1) {
            *p = 0;
            __asm__ __volatile__ ("" : : "m"(*p) );
            return;
        }
        if (n == 2) {
            __asm__ __volatile__ ("movw\t$0, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(short*, p)));
            return;
        }
        if (n == 4) {
            __asm__ __volatile__ ("movl\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(int*, p))
                    : "ai"(0));
            return;
        }
        if (n == 8) {
            __asm__ __volatile__ ("movq\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(long long*, p))
                    : "ai"(0));
            return;
        }
        if (n == 16 && COMMON_MACROS_is_pointer_align(p, 16)) {
            __asm__ __volatile__ ("movdqa\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p)) : "x"(0));
            return;
        }
        if (n == 16) {
            __asm__ __volatile__ ("movdqu\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p)) : "x"(0));
            return;
        }
    }

    if (COMMON_MACROS_is_pointer_align(p, 8) && n % 8 == 0 && n > 0) {
        if (!COMMON_MACROS_is_pointer_align(p, 16) && n >= 8) {
            __asm__ __volatile__ ("movq\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(long long*, p))
                        : "ai"(0));
            p += 8;
            n -= 8;
        }
        while (n >= 16) {
            while (n >= 64) {
                __asm__ __volatile__ ("movdqa\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p))
                        : "x"(0));
                __asm__ __volatile__ ("movdqa\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 16))
                        : "x"(0));
                __asm__ __volatile__ ("movdqa\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 32))
                        : "x"(0));
                __asm__ __volatile__ ("movdqa\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 48))
                        : "x"(0));
                p += 64;
                n -= 64;
            }
            if (n < 16) {
                break;
            }
            __asm__ __volatile__ ("movdqa\t%1, %0"
                    : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p))
                    : "x"(0));
            p += 16;
            n -= 16;
        }
        if (n >= 8) {
            __asm__ __volatile__ ("movq\t%1, %0"
                        : "=m"(*COMMON_MACROS_reinterpret_cast_helper(long long*, p))
                        : "ai"(0));
            p += 8;
            n -= 8;
        }
        return;
    }

    int align_n = COMMON_MACROS_reinterpret_cast_helper(unsigned long long, p) % 16;
    __typeof__(n) n1 = n;
    while (1) {
        switch (align_n) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 11:
            case 13:
            case 15:
                if (n1 < 1) {
                    goto tail;
                }
                *p = 0;
                __asm__ __volatile__ ("" : : "m"(*p) );
                p++;
                n1--;
                align_n++;
                continue;
            case 2:
            case 6:
            case 10:
            case 14:
                if (n1 < 2) {
                    goto tail;
                }
                __asm__ __volatile__ ("movw\t$0, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(short*, p)));
                p += 2;
                n1 -= 2;
                align_n += 2;
                continue;
            case 4:
            case 12:
                if (n1 < 4) {
                    goto tail;
                }
                __asm__ __volatile__ ("movl\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(int*, p))
                        : "ai"(0));
                p += 4;
                n1 -= 4;
                align_n += 4;
                continue;
            case 8:
                if (n1 < 8) {
                    goto tail;
                }
                __asm__ __volatile__ ("movq\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(long long*, p))
                        : "ai"(0));
                p += 8;
                n1 -= 8;
                align_n += 8;
                continue;
            case 0:
            case 16:
                if (n1 < 16) {
                    goto tail;
                }
                while (n1 >= 16) {
                    while (n1 >= 64) {
                        __asm__ __volatile__ ("movdqa\t%1, %0"
                                : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p))
                                : "x"(0));
                        __asm__ __volatile__ ("movdqa\t%1, %0"
                                : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 16))
                                : "x"(0));
                        __asm__ __volatile__ ("movdqa\t%1, %0"
                                : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 32))
                                : "x"(0));
                        __asm__ __volatile__ ("movdqa\t%1, %0"
                                : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p + 48))
                                : "x"(0));
                        p += 64;
                        n1 -= 64;
                    }
                    if (n1 < 16) {
                        break;
                    }
                    __asm__ __volatile__ ("movdqa\t%1, %0"
                            : "=m"(*COMMON_MACROS_reinterpret_cast_helper(__int128*, p))
                            : "x"(0));
                    p += 16;
                    n1 -= 16;
                }
                break;
            default:
                __builtin_unreachable();
        }
        break;
    }
tail:
    if (n1 == 0) {
        return;
    }
    while (1) {
        switch (n1) {
            case 15:
            case 14:
            case 13:
            case 12:
            case 11:
            case 10:
            case 9:
            case 8:
                __asm__ __volatile__ ("movq\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(long long*, p))
                        : "ai"(0));
                p += 8;
                n1 -= 8;
                continue;
            case 7:
            case 6:
            case 5:
            case 4:
                __asm__ __volatile__ ("movl\t%1, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(int*, p))
                        : "ai"(0));
                p += 4;
                n1 -= 4;
                continue;
            case 3:
            case 2:
                __asm__ __volatile__ ("movw\t$0, %0" : "=m"(*COMMON_MACROS_reinterpret_cast_helper(short*, p)));
                p += 2;
                n1 -= 2;
                continue;
            case 1:
                *p = 0;
                __asm__ __volatile__ ("" : : "m"(*p) );
                // fall through
            case 0:
                return;
        }
    }
}


#define EXPLICIT_BZERO(a,n) COMMON_MACROS_explicit_bzero(a,n)
