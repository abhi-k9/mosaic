#pragma once

/*! @file typelist.hpp
 *  @brief A list for types!. Includes operations on them.
 */


#include "markers.hpp"


namespace mosaic {

template <class H, class T>
struct Typelist {
    using Head = H;
    using Tail = T;
};

/**************************************************/

// Primary multiparameter template
template <class Head, class... Rest>
struct MakeTL {
    using Tail = MakeTL<Rest...>::TL;
    using TL = Typelist<Head, Tail>;
};

// Single parameter base case
template <class Head>
struct MakeTL<Head> {
    using TL = Typelist<Head, NullType>;
};

/**************************************************/

// Namespace to hold the operations on `TypeLists`.
namespace tl {

/**************************************************/

// Primary template
template <class TList> struct Length;

// Base case
template <>
struct Length<NullType>{
    enum { value = 0 };
};

// Recursion template
template <class H, class T>
struct Length<Typelist<H, T>> {
    enum { value = 1 + Length<T>::value };
};


/**************************************************/

// Primary template
template <class TL, int i> struct TypeAt;

template <class H, class T>
struct TypeAt<Typelist<H, T>, 0> {
    using Result = H;    
};

template <class H, class T, int i>
struct TypeAt<Typelist<H, T>, i> {
    using Result = TypeAt<T, i-1>::Result;    
};


template <class TL, int i> struct TypeAtNonStrict;

template <>
struct TypeAtNonStrict <NullType, 0> {
    using Result = NullType;
};

template <class H, class T>
struct TypeAtNonStrict <Typelist<H, T>, 0> {
    using Result = H;
};

template <class H, class T, int i>
struct TypeAtNonStrict<Typelist<H, T>, i>{
    using Result = TypeAtNonStrict<T, i-1>;
};

/**************************************************/


} // end namespace `tl`

} // end namespace `mosaic`