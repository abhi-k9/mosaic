#pragma once

/*! @file typelist.hpp
 *  @brief Alternate implementation for a list for types! Includes operations on them.
 */


#include "markers.hpp"

namespace mosaic{

// Forward declaration.
template <class, class ...> struct Split;


/*! @brief Structure to store a type parameter pack.
 */ 
template <class ...Types>
struct ParameterPack {
    using HTSplit = Split<Types...>;
    using Head = typename HTSplit::Head;
    using Tail = typename HTSplit::Tail;
};


/*! @brief Split parameter pack into head (first type in the pack) 
 *  and tail (rest of the types).
 */ 
template <class H, class ...T>
struct Split {
    using Head = H;
    using Tail = ParameterPack<T...>;
};


/*! @brief Compute the length of given `ParameterPack`.
 */
template <typename... Pack>
struct Length {
    enum { value = sizeof...(Pack) };
};

template <typename... Pack>
struct Length <ParameterPack<Pack...>> {
    enum { value = sizeof...(Pack) };
};

} // end namespace `mosaic`