#pragma once

/*! @file typelist.hpp
 *  @brief A list for types!. Includes operations on them.
 */


#include "markers.hpp"
#include "select.hpp"
#include "conversion.hpp"


namespace mosaic {


/** @brief Base node type for building typlists.*/

template <class H, class T>
struct Typelist {
    using Head = H;
    using Tail = T;
};

/**************************************************/


/** @brief Generate a typelist from given types.*/
template <class Head, class... Rest>
struct MakeTL {
    using Tail = typename MakeTL<Rest...>::TL;
    using TL = Typelist<Head, Tail>;
};

// Single parameter base case
template <class Head>
struct MakeTL <Head> {
    using TL = Typelist<Head, NullType>;
};

/**************************************************/


// Namespace to hold the operations on `TypeLists`
namespace tl {


/** @brief Compute length of the typelist.
 *  @tparam TList Typelist to compute length for.
 */ 
template <class TList> struct Length;

// Base case
template <>
struct Length <NullType>{
    enum { value = 0 };
};

// Recursion template
template <class H, class T>
struct Length <Typelist<H, T>> {
    enum { value = 1 + Length<T>::value };
};

/**************************************************/


/** @brief Get the type at a given index in the typelist.
 *  @tparam TL Typelist.
 *  @tparam i Index.
 *  @note Throws error if index is out-of-bounds.
 *  @sa TypeAtNonStrict
 */ 
template <class TL, int i> struct TypeAt;

template <class H, class T>
struct TypeAt <Typelist<H, T>, 0> {
    using Result = H;    
};

template <class H, class T, int i>
struct TypeAt <Typelist<H, T>, i> {
    using Result = typename TypeAt<T, i-1>::Result;    
};

/**************************************************/


/** @brief Get the type at a given index in the typelist.
 *  @tparam TL Typelist.
 *  @tparam i Index.
 *  @tparam Default Default type to return if index is out-of-bounds.
 *  @note Result is set to given default if index is out-of-bounds.
 *  @sa TypeAt
 */ 
template <class TL, int i, class Default> struct TypeAtNonStrict;

template <class Default, int i>
struct TypeAtNonStrict <NullType, i, Default> {
    using Result = Default;
};

template <class H, class T, class Default>
struct TypeAtNonStrict <Typelist<H, T>, 0, Default> {
    using Result = H;
};

template <class H, class T, int i, class Default>
struct TypeAtNonStrict <Typelist<H, T>, i, Default>{
    using Result = typename TypeAtNonStrict<T, i-1, Default>::Result;
};

/**************************************************/


/** @brief Get the index for a given type in the typelist.
 *  @tparam TL Typelist.
 *  @tparam T Type to get the index for.
 */ 
template <class TL, class T> struct IndexOf;

template <class T>
struct IndexOf <NullType, T> {
    enum { value = -1 };
};

template <class Tail, class T>
struct IndexOf <Typelist<T, Tail>, T>
{
    enum { value = 0 };
};

template <class Head, class Tail, class T>
struct IndexOf <Typelist<Head, Tail>, T> {
private:
    enum {temp = IndexOf<Tail, T>::value};
public:
    enum { value = temp == -1 ? -1 : 1 + temp };
};

/**************************************************/


/** @brief Append a given type to the typelist.
 *  @tparam TL Typelist to append to.
 *  @tparam T Type or typelist to append.
 */ 
template <class TL, class T> struct Append;

template <> struct Append <NullType, NullType> {
    using Result = NullType;
};

template <class T>
struct Append <NullType, T>{
    using Result = typename MakeTL<T>::TL;
};

template <class Head, class Tail>
struct Append <NullType, Typelist<Head, Tail>> {
    using Result = Typelist<Head, Tail>;
};

template <class Head, class Tail, class T>
struct Append <Typelist<Head, Tail>, T> {
    using Result = Typelist<Head, typename Append<Tail, T>::Result>;
};

template <class Head1, class Tail1, class Head2, class Tail2>
struct Append <Typelist<Head1, Tail1>, Typelist<Head2, Tail2>>{

    using Result =  
        typename Append <
            typename Append <
                Typelist<Head1, Tail1>, Head2
            >::Result,
            Tail2
        >::Result;

};

/**************************************************/


/** @brief Erase a given type from the typelist.
 *  @tparam TL Typelist.
 *  @tparam T Type to erase.
 *  @note Erases the first match only.
 *  @sa EraseAll
 */ 
template <class TL, class T> struct Erase;

template <class T>
struct Erase <NullType, T> {
    using Result = NullType;
};

template <class Tail, class T>
struct Erase <Typelist<T, Tail>, T> {
    using Result = Tail;
};

template <class Head, class Tail, class T>
struct Erase <Typelist<Head, Tail>, T> {

    using Result = 
        Typelist<
                 Head,
                 typename Erase<Tail, T>::Result
                >;

};

/**************************************************/


/** @brief Erase a given type from the typelist.
 *  @tparam TL Typelist.
 *  @tparam T Type to erase.
 *  @note Erases the all matches.
 *  @sa Erase
 */ 
template <class TL, class T> struct EraseAll;

template <class T>
struct EraseAll <NullType, T> {
    using Result = NullType;
};

template <class Tail, class T>
struct EraseAll <Typelist<T, Tail>, T> {
    using Result = typename EraseAll<Tail, T>::Result;
};


template <class Head, class Tail, class T>
struct EraseAll <Typelist<Head, Tail>, T> {
    using Result = Typelist<
                            Head,
                            typename EraseAll<Tail, T>::Result
                           >;
};

/**************************************************/


/** @brief Remove duplicate types from typlist.
 *  @tparam TL Typelist.
 */
template <class TL> struct NoDuplicates;


template <class T>
struct NoDuplicates<Typelist<T, NullType>> {
    using Result = Typelist<T, NullType>;
};


template <class Head, class Tail>
struct NoDuplicates<Typelist<Head, Tail>> {

private:
    using TailNoDup = typename NoDuplicates<Tail>::Result;
    using TailNoDupHead = typename Erase<TailNoDup, Head>::Result;

public:
    using Result = Typelist<Head, TailNoDupHead>;

};

/**************************************************/


/** @brief Replace a type in the typelist with another.
 *  @tparam TL Typelist.
 *  @tparam Replacee Type to replace.
 *  @tparam Replacer Type to replace with.
 *  @note Replaces the first match only.
 *  @sa ReplaceAll
 */
template <class TL, class Replacee, class Replacer> struct Replace;


template <class T, class U>
struct Replace<NullType, T, U> {
    using Result = NullType;
};


template <class Tail, class Replacee, class Replacer>
struct Replace <Typelist<Replacee, Tail>, Replacee, Replacer> {
    using Result = Typelist<Replacer, Tail>;
};


template <class Head, class Tail, class Replacee, class Replacer>
struct Replace <Typelist<Head, Tail>, Replacee, Replacer> {
    using T = typename Replace<Tail, Replacee, Replacer>::Result;
    using Result = Typelist<Head, T>;
};

/**************************************************/


/** @brief Replace a type in the typelist with another.
 *  @tparam TL Typelist.
 *  @tparam Replacee Type to replace.
 *  @tparam Replacer Type to replace with.
 *  @sa Replace
 */
template <class TL, class Replacee, class Replacer> struct ReplaceAll;


template <class T, class Replacee, class Replacer>
struct ReplaceAll <Typelist<T, NullType>, Replacee, Replacer> {
    using Result = Typelist<T, NullType>;
};


template <class Tail, class Replacee, class Replacer>
struct ReplaceAll <Typelist<Replacee, Tail>, Replacee, Replacer> {
    using T = typename ReplaceAll<Tail, Replacee, Replacer>::Result;
    using Result = Typelist<Replacer, T>;
};


template <class Head, class Tail, class Replacee, class Replacer>
struct ReplaceAll <Typelist<Head, Tail>, Replacee, Replacer> {
    using T = typename ReplaceAll<Tail, Replacee, Replacer>::Result;
    using Result = Typelist<Head, T>;
};

/**************************************************/


/** @brief Find one of the most derived types in a typelist.
 *  @tparam TL Typelist.
 *  @tparam T Initial guess.
 *  @note Initial guess can be arbitrary.
 */
template <class TL, class T> struct MostDerived;


template <class T>
struct MostDerived<NullType, T> {
    using Result = T;
};


template <class Head, class Tail, class T>
struct MostDerived <Typelist<Head, Tail>, T> {
private:
    using Candidate = typename MostDerived<Tail, T>::Result;

public:
    using Result = typename Select<
        SUPERSUBCLASS<Candidate, Head>(),
        Head, Candidate
    >::Result;

};

/**************************************************/


/** @brief Move more derived types to front of the typelist.
 *  @tparam TL Typelist.
 */
template <class TL> struct DerivedToFront;

template <>
struct DerivedToFront <NullType> {
    using Result = NullType;
};

template <class Head, class Tail>
struct DerivedToFront <Typelist<Head, Tail>> {
private:
    using TheMostDerived = typename MostDerived<Tail, Head>::Result;

    using SwappedInTail = typename Replace<Tail, TheMostDerived, Head>::Result;
    using SortedTail = typename DerivedToFront<SwappedInTail>::Result;

public:
    using Result = Typelist<TheMostDerived, SortedTail>;

};

/**************************************************/


} // end namespace `tl`

} // end namespace `mosaic`