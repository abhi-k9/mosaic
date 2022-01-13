#pragma once

/*! @file typelist.hpp
 *  @brief Alternate implementation for a list for types! Includes operations on them.
 */


#include "markers.hpp"

namespace mosaic{

template <class, class ...> struct Split;

template <class ...Types>
struct ParameterPack {
    using HTSplit = Split<Types...>;
    using Head = typename HTSplit::Head;
    using Tail = typename HTSplit::Tail;
};

template <class H, class ...T>
struct Split {
    using Head = H;
    using Tail = ParameterPack<T...>;
};

} // end namespace `mosaic`