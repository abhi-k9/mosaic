#pragma once

/*! @file conversion.hpp
 *  @brief Provides `Conversion` template class for detecting type conversions.
 */

namespace mosaic {

template <class T, class U>
class Conversion
{
    using Small = char;
    class Big
    {
        char dummy[2];
    };

    static Small Test(U);
    static Big Test(...);

    static T MakeT();

public:
    enum
    {
        exists =
            sizeof(Test(MakeT())) == sizeof(Small),

        sameType = false
    };
};

template <class T>
class Conversion<T, T>
{
public:
    enum
    {
        exists = 1,
        sameType = 1,
    };
};

} // end namespace mosaic