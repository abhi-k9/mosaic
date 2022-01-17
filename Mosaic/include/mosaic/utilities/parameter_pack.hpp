#pragma once

/*! @file typelist.hpp
 *  @brief Alternate implementation for a list for types! Includes operations on them.
 */


#include "markers.hpp"

namespace mosaic{

    // Forward declaration.
    template <class, class ...> struct Split;

    /**************************************************/


    /*! @brief Structure to store a type parameter pack.
    */ 
    template <class ...Types>
    struct ParameterPack {
        using HTSplit = Split<Types...>;
        using Head = typename HTSplit::Head;
        using Tail = typename HTSplit::Tail;
    };

    /**************************************************/


    /*! @brief Split parameter pack into head (first type in the pack) 
    *  and tail (rest of the types).
    */ 
    template <class H, class ...T>
    struct Split {
        using Head = H;
        using Tail = ParameterPack<T...>;
    };

    /**************************************************/


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

    /**************************************************/


    /** @brief Get the type at a given index in the `ParmeterPack`.
     *  @tparam i Index.
     *  @tparam PP ParamterPack.
     *  @note Throws error if index is out-of-bounds.
     *  @sa TypeAtNonStrict
     */ 
    template <int i, typename... Params>
    struct TypeAt {
        using Result = typename TypeAt<i, ParameterPack<Params...>>::Result;    
    };

    template <int i, typename... Params>
    struct TypeAt <i, ParameterPack<Params...>> {

    static_assert(i < sizeof...(Params), "Given index is out of range.");

    private:
        using S = Split<Params...>;
        using Head = typename S::Head;
        using Tail = typename S::Tail;

    public:
        using Result = typename TypeAt<i-1, Tail>::Result;

    };

    template <typename... Params>
    struct TypeAt <0, ParameterPack<Params...>> {

    private:
        using S = Split<Params...>;
        using Head = typename S::Head;

    public:
        using Result = Head;

    };

    /**************************************************/

} // end namespace `mosaic`