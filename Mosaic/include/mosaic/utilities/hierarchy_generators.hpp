#pragma once

/*! @file hierarchy_generators.hpp
 *  @brief Generate class inheritance hierarchies using typelists.
 */


#include "typelist.hpp"


namespace mosaic {

/*!
 * @brief Class used to uniquely tag each base (`Unit<Head>`) class of the scattered hierarchy.
 *
 * @details Without this tag inserted before each `Unit<Head>` end of inheritance,
 * it becomes impossible, in the case of multiple repeating types for `Unit<Head`>,
 * to cast derived type to repeating base class unambiguously.
 * For example `GenScatterHierarchy` with typelist of `int`, `float`, `int`, `char`.
 * 
 */
template <class Head, class Tail, template <class> class Unit>
class ScatterTag : public Unit<Head> {
public:
    using ResultType = Unit<Head>;
};


/*!
 * @brief Generate a scattered hierarchy from given typelist and `Unit` template class.
 * 
 * @details The generated hierarchy inherits from `Unit<T>`, where `T` is a type from the typelist,
 * scattered across multiple inheritance branches.
 * 
 * @sa GenLinearHierarchy
 */
template <class AtomicType, template <class> class Unit>
class GenScatterHierarchy
{
public:
    using LeftBase = Unit<AtomicType>;
};

template <class Head, class Tail, template <class> class Unit>
class GenScatterHierarchy<Typelist<Head, Tail>, Unit>
    : public ScatterTag<Head, Tail, Unit>,
      public GenScatterHierarchy<Tail, Unit>
{
public:
    using TL = Typelist<Head, Tail>;
    using RightBase = GenScatterHierarchy<Tail, Unit>;
    using LeftBase = GenScatterHierarchy<Head, Unit>;
    using LeftTag = ScatterTag<Head, Tail, Unit>;

    template <class U>
    struct UnitType {
        using Result = Unit<U>;
    };
};

template <template <class> class Unit>
class GenScatterHierarchy<NullType, Unit> {
public:
    template <class U>
    struct UnitType {
        using Result = Unit<U>;
    };
};


/**************************************************/


/*!
 * @brief Helper class to get reference to the `i`th base class subobject of a scattered hierarchy root object.
 * 
 * @details The ordering of base classes is same as the order of the types in the typelist
 * used to construct the scattered hierarchy.
 * 
 * @tparam SHierarchy The type for the scattered hierarchy.
 * @tparam i index of the base unit class type.
 * 
 * @sa GenScatterHierarchy
 */
template <class SHierarchy, unsigned i>
#if __cplusplus >= 202002L // Compiler supports C++20 standard and above.
    requires (Length<typename SHierarchy::TL>::value > i)
#endif
struct FieldHelper {
private:
    using RightBase = typename SHierarchy::RightBase;

public:
    using SubObjType = typename FieldHelper<RightBase, i-1>::SubObjType;
    using ResultType = typename SubObjType::ResultType;

    static ResultType& Get(SHierarchy& obj) {
        SubObjType& subobj = obj;
        return subobj;
    } 
};

template <class SHierarchy>
struct FieldHelper<SHierarchy, 0> {
private:
    using LeftTag = typename SHierarchy::LeftTag;

public:
    using SubObjType = LeftTag;
    using ResultType = typename LeftTag::ResultType;

    static ResultType& Get(SHierarchy& obj) {
        SubObjType& subobj = obj;
        return subobj;
    } 
};

#if __cplusplus < 202002L // No C++20 standard and above support.
    template <template <class> class Unit>
    struct FieldHelper<GenScatterHierarchy<NullType, Unit>, 0> {
        static_assert(sizeof(Unit<int>) == -1, "Out-of-bounds index provided to `Field::Get<i>()`");
    };
#endif

/**************************************************/


/*!
 * @brief Class to get reference to base class subobject of a scattered hierarchy root object.
 * 
 * @tparam SHierarchy The type for the scattered hierarchy.
 * 
 * @sa GenScatterHierarchy
 */
template <class SHierarchy>
struct Field {

    /*!
     * @brief Get reference to the `i`th base class subobject of a scattered hierarchy root object.
     * 
     * @details The ordering of base classes is same as the order of the types in the typelist
     * used to construct the scattered hierarchy.
     * 
     * @tparam i index of the base unit class type.
     * 
     */
    template <unsigned i>
    static
    typename FieldHelper<SHierarchy, i>::ResultType&
    Get(SHierarchy& obj) {
        return FieldHelper<SHierarchy, i>::Get(obj);
    }

    /*!
     * @brief Get reference to the `i`th base class subobject of a scattered hierarchy root object.
     * 
     * @tparam AtomicType Type of the unit base class from the typelist used to create the hierarchy object
     * 
     * @note This function will give ambiguity error if the requested type was repeated in the typelist.
     * Use the overloaded version that takes an index instead
     * 
     * @sa GenScatterHierarchy
     */
    template <class AtomicType>
    static
    typename SHierarchy::template UnitType<AtomicType>::Result&
    Get(SHierarchy& obj) {
        return obj;
    }

    template <class AtomicType>
    static
    const typename SHierarchy::template UnitType<AtomicType>::Result&
    Get(const SHierarchy& obj) {
        return obj;
    }

};

/**************************************************/


/*!
 * @brief Generate a linear hierarchy from given typelist and `ReinheritorUnit` template class.
 * 
 * @details The generated hierarchy inherits from `ReinheritorUnit<T, LH>`,
 * where `T` is a type from the typelist, in a single, linear inheritance branch.
 * 
 * @note The user provided `ReinheritorUnit` should inherit back from it second template parameter `LH`.
 * 
 * @sa GenScatterHierarchy
 */
template <class TL, template <class, class> class ReinheritorUnit, class Root=EmptyType>
class GenLinearHierarchy;

template <class Head, class Tail, template <class, class> class ReinheritorUnit, class Root>
class GenLinearHierarchy <Typelist<Head, Tail>, ReinheritorUnit, Root>
    : public ReinheritorUnit<Head, GenLinearHierarchy<Tail, ReinheritorUnit, Root>>
{};


template <class Atomic, template <class, class> class ReinheritorUnit, class Root>
class GenLinearHierarchy <Typelist<Atomic, NullType>, ReinheritorUnit, Root>
    : public ReinheritorUnit<Atomic, Root>
{};


} // end namespace `mosaic`