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


// true for array arg, false for pointer
// used for static_assert
template<class T, decltype(sizeof 0) N>
constexpr bool
ARRAY_LENGTH_helper_is_array(T const (&)[N])
{
    return 1;
}

template<class T>
constexpr bool
ARRAY_LENGTH_helper_is_array(T)
{
    return 0;
}


#define ARRAY_LENGTH(a) ({ static_assert(ARRAY_LENGTH_helper_is_array(a),     \
                           "ARRAY_LENGTH is only for actual array");          \
                           sizeof(a) / sizeof((a)[0]); })

#endif


#define COMPILER_BARRIER() __asm__ __volatile__ ("":::"memory")


#ifndef __cplusplus

// use __auto_type to prevent self initialization
// every arg is evaluated once
// unfortunately this macro doesn't work for C++
#define MAX_MIN_macro_helper(a,b,op)  ({ __auto_type s =                     \
                (struct { __typeof__(a) _a; __typeof__(b) _b; })             \
                { ._a = (a), ._b = (b) };                                    \
                (s._a op s._b) ? s._b : s._a; })
#define MAX(a,b)  MAX_MIN_macro_helper(a,b,<)
#define MIN(a,b)  MAX_MIN_macro_helper(a,b,>)


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
        cond = (a < b);
    } else {
        cond = (a > b);
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
        cond = (a < b);
    } else {
        cond = (a > b);
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
        cond = (a < b);
    } else {
        cond = (a > b);
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
