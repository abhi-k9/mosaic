#pragma once

/*! @file typelist.hpp
 *  @brief Alternate implementation for a list for types! Includes operations on them.
 */


#include "markers.hpp"

namespace mosaic{

template <class, class ...> struct Split;

template <class ...Types>
struct Typelist {
    using Head = Split<Types...>::Head;
    using Tail = Split<Types...>::Tail;
};

template <class H, class ...T>
struct Split {
    using Head = H;
    using Tail = Typelist<T...>;
};

} // end namespace `mosaic`