#pragma once

/*! @file type_mapping.hpp
 *  @brief Provides `Int2Type` and `Type2Type` mappings.
*/

namespace mosaic {

/*!
 *   @brief Struct for mapping an integer to a type.
 *   @tparam i Integer to map from.
 */
template <int i>
struct Int2Type
{
    enum
    {
        value = i
    };
};

/*!
 *  @brief Struct for mapping a type to a type.
 *  @tparam T Type to map from.
*/
template <typename T>
struct Type2Type
{
    using OriginalType = T;
};

} // end namespace mosaic