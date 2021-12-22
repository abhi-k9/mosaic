#pragma once

/*! @file select.hpp
 *  @brief Provides `Select` template for type selection.
*/

namespace mosaic {

/*!
 *  @brief Struct for easy type selection
 *  
 *  @details Defines an internal type `Result` which is set to either
 *  `T` or `U` depending on the value given for `flag`.
 * 
 *  @tparam flag Boolean to switch between types provided.
 *  @tparam T Type exposed if `flag` is `true`.
 *  @tparam U Type exposed if `flag` is `false`. 
*/
template <bool flag, typename T, typename U>
struct Select
{
    using Result = T;
};

template <typename T, typename U>
struct Select<false, T, U>
{
    using Result = U;
};

} // end namespace mosaic