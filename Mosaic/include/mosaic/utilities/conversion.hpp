#pragma once

/*! @file conversion.hpp
 *  @brief Provides `Conversion` template class for detecting type conversions.
 */

namespace mosaic {

template <class T, class U>
class Conversion
{
    using Small = char;
    class Big
    {
        char dummy[2];
    };

    static Small Test(U);
    static Big Test(...);

    static T MakeT();

public:
    enum
    {
        exists =
            sizeof(Test(MakeT())) == sizeof(Small),

        sameType = false
    };
};

template <class T>
class Conversion<T, T>
{
public:
    enum
    {
        exists = 1,
        sameType = 1,
    };
};

// Handle template parameter deduced as `void`.
// `void` cannot be a fuction parameter type hence
// the primary template gives compile error.
template <class T>
class Conversion<T, void>
{
public:
    enum
    {
        exists = 0,
        sameType = 0,
    };
};

template <class T>
class Conversion<void, T>
{
public:
    enum
    {
        exists = 0,
        sameType = 0,
    };
};

template <>
class Conversion<void, void>
{
public:
    enum
    {
        exists = 1,
        sameType = 1,
    };
};


#if __cplusplus >= 202002L // Compiler supports C++20 standard and above.

    template <class T, class U>
    consteval bool SUPERSUBCLASS() {
        
        using cpT = const T *;
        using cpU = const U *;
        using cpv = const void *;

        return (Conversion<cpU, cpT>::exists && !Conversion<cpT, cpv>::sameType);
    }

    template <class T, class U>
    consteval bool SUPERSUBCLASS_STRICT() {
        
        using cpT = const T *;
        using cpU = const U *;

        using cT = const T;
        using cU = const U;

        return (SUPERSUBCLASS<T, U>() && !Conversion<cT, cU>::sameType);
    }

    #define SUPERSUBCLASS(T, U) \
        @"C++20 conforming compiler detected, please use `SUPERSUBCLASS` function instead of the macro. `Usage: SUPERCLASS<T, U>()`."

    #define SUPERSUBCLASS_STRICT(T, U) \
        @"C++20 conforming compiler detected, please use `SUPERSUBCLASS` function instead of the macro. Usage: `SUPERCLASS_STRICT<T, U>()`."

/* Note: 
 *      `constexpr` NOT guaranteed to be evaluated at compile time at all places.
 */
#else

    template <class T, class U>
    constexpr bool SUPERSUBCLASS() {
        
        using cpT = const T *;
        using cpU = const U *;
        using cpv = const void *;

        return (Conversion<cpU, cpT>::exists && !Conversion<cpT, cpv>::sameType);
    }

    template <class T, class U>
    constexpr bool SUPERSUBCLASS_STRICT() {
        
        using cpT = const T *;
        using cpU = const U *;

        using cT = const T;
        using cU = const U;

        return (SUPERSUBCLASS<T, U>() && !Conversion<cT, cU>::sameType);
    }

    #define SUPERSUBCLASS(T, U) \
        @"Please use `SUPERSUBCLASS` function instead of the macro. `Usage: SUPERCLASS<T, U>()`."

    #define SUPERSUBCLASS_STRICT(T, U) \
        @"Please use `SUPERSUBCLASS` function instead of the macro. Usage: `SUPERCLASS_STRICT<T, U>()`."

#endif


// Deprecated alternative
#ifdef MACRO_SUPERSUBCLASS

    template <class T, class U>
    struct SuperSubClass {
        enum {
            value = (mosaic::Conversion<const U*, const T*>::exists && \
                     ! mosaic::Conversion<const T*, const void*>::sameType)
        };
    };

    template <class T, class U>
    struct SuperSubClass_Strict {
        enum {
            value = (SuperSubClass<T, U>::value && \
                     ! mosaic::Conversion<const T, const U>::sameType)
        };
    };

    #define SUPERSUBCLASS(T, U) mosaic::SuperSubClass<T, U>::value
    
    #define SUPERSUBCLASS_STRICT(T, U) mosaic::SuperSubClass_Strict<T, U>::value

#endif

} // end namespace mosaic