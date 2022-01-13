#pragma once

/*! @file type_traits.hpp
 *  @brief Query different type traits. Allows transformation of types.
 */


#include "typelist.hpp"
#include "conversion.hpp"


namespace mosaic {

    /*! @brief Template to specialize for registering custom types as unsigned integer types.
     *  Set enum member `value` to 1 in the specialization 
     */
    template <typename T>
    struct IsCustomUnsignedInt
    {
        enum { value = 0 };
    };

    /*! @brief Template to specialize for registering custom types as signed integer types.
     *  Set enum member `value` to 1 in the specialization 
     */
    template <typename T>
    struct IsCustomSignedInt
    {
        enum { value = 0 };
    };

    /*! @brief Template to specialize for registering custom types as floating point types.
     *  Set enum member `value` to 1 in the specialization 
     */
    template <typename T>
    struct IsCustomFloat
    {
        enum { value = 0 };
    };


    namespace internal {

        /*! @brief Typelist of unsigned integer types.
         */
        using UnsignedInts =
            MakeTL<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::TL;
        
        /*! @brief Typelist of signed integer types.
         */
        using SignedInts =
            MakeTL<signed char, short, int, long, long long>::TL;

        /*! @brief Typelist of integer types other than usual signed and unsigned ones. 
         */
        using OtherInts =
            MakeTL<bool, char, wchar_t>::TL;

        /*! @brief Typelist of floating point types.
         */
        using Floats =
            MakeTL<float, double, long double>::TL;


        /*! @brief Detect if `T` is a pointer. Make pointee type available.
         */
        template <class T>
        struct PointerTraits {
            enum {result = false};
            using PointeeType = NullType;
        };

        template <class T>
        struct PointerTraits<T*> {
            enum {result = true};
            using PointeeType = T;
        };

        template <class T>
        struct PointerTraits<T* const> {
            enum {result = true};
            using PointeeType = T;
        };


        /*! @brief Provide pointer type for `T`. 
         */
        template <class T>
        struct AddPointer {
            using Result = T*;
        };

        template <class T>
        struct AddPointer <T&> {
            using Result = T*;
        };


        /*! @brief Detect if `T` is a reference. Make referee type available.
         */
        template <class T>
        struct ReferenceTraits {
            enum {result = false};
            using ReferredType = T;
        };

        template <class T>
        struct ReferenceTraits<T&> {
            enum {result = true};
            using ReferredType = T;
        };

        template <class T>
        struct ReferenceTraits<T&&> {
            enum {result = true};
            using ReferredType = T;
        };


        /*! @brief Provide lvalue reference type for `T`.
         */
        template <class T>
        struct AddLValReference {
            using Result = T&;
        };


        /*! @brief Provide lvalue reference type for `T`.
         */
        template <class T>
        struct AddRValReference {
            using Result = T&&;
        };


        /*! @brief Detect raw function pointers. Provide the number of parameters it takes.
         */
        template <typename T>
        struct IsFunctionPointerRaw {
            enum { result = 0 };
            enum { parameters = 0 };
        };

        template <typename T, typename... Params>
        struct IsFunctionPointerRaw <T (*)(Params...)> {
            enum { result = 1 };
            enum { parameters = sizeof...(Params) };
        };


        /*! @brief Traits for raw function pointers.
         */
        template <typename T>
        struct FunctionPointerTraits {
            enum { result = IsFunctionPointerRaw<T>::result };
        };


        /*! @brief Detect class member function pointers. Provide the number of parameters it takes.
         */
        template <typename T>
        struct IsMemberFunctionPointerRaw {
            enum { result = 0 };
        };

        template <typename T, typename S, typename... Params>
        struct IsMemberFunctionPointerRaw <T (S::*)(Params...)> {
            enum { result = 1 };
            enum { parameters = sizeof...(Params) };
        };

        template <typename T, typename S, typename... Params>
        struct IsMemberFunctionPointerRaw <T (S::*)(Params...) const> {
            enum { result = 1 };
            enum { parameters = sizeof...(Params) };
        };

        template <typename T, typename S, typename... Params>
        struct IsMemberFunctionPointerRaw <T (S::*)(Params...) volatile> {
            enum { result = 1 };
            enum { parameters = sizeof...(Params) };
        };

        template <typename T, typename S, typename... Params>
        struct IsMemberFunctionPointerRaw <T (S::*)(Params...) const volatile> {
            enum { result = 1 };
            enum { parameters = sizeof...(Params) };
        };


        /*! @brief Traits for class member function pointers.
         */
        template <typename T>
        struct MemberFunctionPointerTraits {
            enum { result = IsMemberFunctionPointerRaw<T>::result };
        };


        /*! @brief Check if `T` is a pointer to a class member. Provides class and member types.
         */
        template <class T>
        struct PToMTraits {
            enum {result = false};

            using ReturnType = NullType;
            using ClassType = NullType;
        };

        template <class T, class U>
        struct PToMTraits <T U::*> {
            enum {result = true};

            using ReturnType = T;
            using ClassType = U;
        };


        /*! @brief Detect and remove `const` from type.
         */
        template <class T>
        struct UnConst {
            using Result = T;
            enum { isConst = 0 };
        };

        template <class T>
        struct UnConst <const T> {
            using Result = T;
            enum { isConst = 1 };
        };

        template <class T>
        struct UnConst <const T&> {
            using Result = T;
            enum { isConst = 1 };
        };

        template <class T>
        struct UnConst <const T&&> {
            using Result = T;
            enum { isConst = 1 };
        };


        /*! @brief Detect and remove `volatile` from type.
         */
        template <class T>
        struct UnVolatile {
            using Result = T;
            enum { isVolatile = 0 };
        };

        template <class T>
        struct UnVolatile <volatile T> {
            using Result = T;
            enum { isVolatile = 1 };
        };

        template <class T>
        struct UnVolatile <volatile T&> {
            using Result = T;
            enum { isVolatile = 1 };
        };

        template <class T>
        struct UnVolatile <volatile T&&> {
            using Result = T;
            enum { isVolatile = 1 };
        };


        /*! @brief Add `const` to type.
         */ 
        template <class T>
        struct AddConst {
            using Result = const T;
        };

        /*! @brief Add `volatile` to type.
         */
        template <class T>
        struct AddVolatile {
            using Result = volatile T;
        };


    } // end namespace `internal`


    /*! 
     * @brief Allows querying traits for a given type `T`, and manipulating it.
     *  
     * @details
     * The class allows the following checks:
     * - `isConst`
     * - `isVolatile`
     * - `isReference`
     * - `isFunction`
     * - `isFunctionPointer`
     * - `isMemberFunctionPointer`
     * - `isMemberPointer`
     * - `isPointer`
     * 
     * - `isUnsignedInt`
     * - `isSignedInt`
     * - `isIntegral`
     * - `isFloat`
     * - `isArithmetic`: Integral or float type.
     * - `isFundamental`: Arithmetic or `void` type. 
     * 
     * 
     * The class allows the following type manipulations:
     * - Get `NonConstType`
     * - Get `NonVolatileType`
     * - Get `UnqualifiedType`: Remove cv-qualifications if any.
     * - Get `ConstType`
     * - Get `VolatileType`
     * - Get `FullyQualifiedType`: Adds both cv-qualifications.
     * - Get `PointerType`
     * - Get `PointeeType`
     * - Get `LValReferenceType`
     * - Get `RValReferenceType`
     * - Get `ReferredType`
     * - Get `UnqualifiedRefferedType`: `ReferredType` without any cv-qualifications.
     * 
     * @sa isCustomUnsignedInt, isCustomSignedInt, isCustomFloat
     */
    template <class T>
    class TypeTraits {

    public:

        using NonConstType = typename internal::UnConst<T>::Result;
        using NonVolatileType = typename internal::UnVolatile<T>::Result;
        using UnqualifiedType = typename internal::UnVolatile<NonConstType>::Result;

        using ConstType = typename internal::AddConst<T>::Result;
        using VolatileType = typename internal::AddVolatile<T>::Result;
        using FullyQualifiedType = typename internal::AddVolatile<ConstType>::Result;
        using PointerType = typename internal::AddPointer<T>::Result;
        using PointeeType = typename internal::PointerTraits<T>::PointeeType;
        using LValReferenceType = typename internal::AddLValReference<T>::Result;
        using RValReferenceType = typename internal::AddRValReference<T>::Result;
        using ReferredType = typename internal::ReferenceTraits<T>::ReferredType;
        using UnqualifiedReferredType = typename internal::ReferenceTraits<UnqualifiedType>::ReferredType;

        enum { isConst = internal::UnConst<T>::isConst };
        enum { isVolatile = internal::UnVolatile<T>::isVolatile };
        enum { isReference = internal::ReferenceTraits<UnqualifiedType>::result };
        enum { isFunction = internal::FunctionPointerTraits<PointerType>::result };
        enum { isFunctionPointer = internal::FunctionPointerTraits<UnqualifiedReferredType>::result };
        enum { isMemberFunctionPointer = internal::MemberFunctionPointerTraits<UnqualifiedReferredType>::result };
        enum { isMemberPointer = internal::PToMTraits<UnqualifiedReferredType>::result };
        enum { isPointer = internal::PointerTraits<UnqualifiedReferredType>::result || isFunctionPointer };

        enum { isStdUnsignedInt = tl::IndexOf<internal::UnsignedInts, UnqualifiedType>::value >= 0 
                                  || tl::IndexOf<internal::UnsignedInts, UnqualifiedReferredType>::value >= 0 };
        enum { isStdSignedInt = tl::IndexOf<internal::SignedInts, UnqualifiedType>::value >= 0  
                                || tl::IndexOf<internal::SignedInts, UnqualifiedReferredType>::value >= 0 };
        enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt || tl::IndexOf<internal::OtherInts, T>::value >= 0
                               || tl::IndexOf<internal::OtherInts, UnqualifiedReferredType>::value >= 0 };
        enum { isStdFloat = tl::IndexOf<internal::Floats, T>::value >= 0 
                            || tl::IndexOf<internal::Floats, UnqualifiedReferredType>::value >= 0 };
        enum { isStdArith = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || Conversion<T, void>::sameType };
        
        enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<UnqualifiedReferredType>::value };
        enum { isSignedInt = isStdSignedInt  || IsCustomSignedInt<UnqualifiedReferredType>::value };
        enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat = isStdFloat || IsCustomFloat<UnqualifiedReferredType>::value };
        enum { isArith = isIntegral || isFloat };
        enum { isFundamental = isStdFundamental || isArith };

    };

} // end namespace `mosaic`
