#pragma once

/*! @file type_mapping.h
 *  @brief Provides `Int2Type` and `Type2Type` mappings.
*/

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
