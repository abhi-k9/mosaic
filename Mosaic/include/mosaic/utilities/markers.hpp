#pragma once

/*! @file markers.hpp
 *  @brief Provides marker types to mark certain boundary cases.
 *  @details Defines: 
 *  - `NullType` class that serves as a null marker for types.
 *  - `EmptyType` struct that can be used as a default filler type for template.
 *  - `AlwaysFalse` template struct that provides `false` value on any instantiation.
 *  - `AlwaysTrue` template struct that provides `true` value on any instantiation.
 */

namespace mosaic {

    class NullType {};

    struct EmptyType {};


    /*! @brief Template that provides `false` value on any instantiation.
     */
    template <typename T>
    struct AlwaysFalse {
        enum { value = false };
    };

    /*! @brief Template that provides `true` value on any instantiation.
     */
    template <typename T>
    struct AlwaysTrue {
        enum { value = true };
    };

} // end namespace `mosaic`