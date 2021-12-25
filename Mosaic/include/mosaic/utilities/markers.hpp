#pragma once

/*! @file markers.hpp
 *  @brief Provides marker types to mark certain boundary cases.
 *  @details Defines: 
 *  - `NullType` class that serves as a null marker for types.
 *  - `EmptyType` struct that can be used as a default filler type for template.
 */

namespace mosaic {

    class NullType {};

    struct EmptyType {};

} // end namespace `mosaic`