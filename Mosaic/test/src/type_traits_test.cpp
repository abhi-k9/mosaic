/*! @file typelist_test.cpp
 *  @brief Tests for all type traits related functionality.
 */

#include "gtest/gtest.h"
#include "mosaic/utilities/type_traits.hpp"

using namespace mosaic;

TEST(TypeTraitsTest, PointerTraits) {

    using PType1 = int*;
    auto result1 = internal::PointerTraits<PType1>::result;
    EXPECT_TRUE(result1);
    
    using PType2 = const int*;
    auto result2 = internal::PointerTraits<PType2>::result;
    EXPECT_TRUE(result2);
    
    using PType3 = const int* const;
    auto result3 = internal::PointerTraits<PType3>::result;
    EXPECT_TRUE(result3);
    
    using PType4 = int (*)(int);
    auto result4 = internal::PointerTraits<PType4>::result;
    EXPECT_TRUE(result4);

    using NPType5 = int;
    auto result5 = internal::PointerTraits<NPType5>::result;
    EXPECT_FALSE(result5);
    using NPType6 = int&;
    auto result6 = internal::PointerTraits<NPType6>::result;
    EXPECT_FALSE(result6);

}


TEST(TypeTraitsTest, AddPointer) {

    using PType1 = int;
    using Result1 = typename internal::AddPointer<PType1>::Result;
    EXPECT_EQ(typeid(Result1), typeid(int*));

}


TEST(TypeTraitsTest, ReferenceTraits) {

    using PType1 = int&;
    using Temp1 = internal::ReferenceTraits<PType1>;
    bool result = Temp1::result;
    using Result1 = typename Temp1::ReferredType;
    
    ASSERT_TRUE(result);
    EXPECT_EQ(typeid(Result1), typeid(int));

    // NOTE: Test for `AddReference` uses `ReferenceTraits`.
    // Keep their tests togather in sequence.
    using PType2 = int;
    using Result2 = internal::AddLValReference<int>::Result;
    
    using Temp2 = internal::ReferenceTraits<Result2>;
    ASSERT_TRUE(Temp2::result);
    EXPECT_EQ(typeid(Result2), typeid(int));

}


TEST(TypeTraitsTest, IsFunctionPointerRaw) {

    using PType1 = void (*)();
    using Temp1 = internal::IsFunctionPointerRaw<PType1>;
    bool result1 = Temp1::result;
    int parameters1 = Temp1::parameters;
    
    ASSERT_TRUE(result1);
    EXPECT_EQ(parameters1, 0);

    using PType2 = int (*)(double, int);
    using Temp2 = internal::IsFunctionPointerRaw<PType2>;
    bool result2 = Temp2::result;
    int parameters2 = Temp2::parameters;
    
    ASSERT_TRUE(result2);
    EXPECT_EQ(parameters2, 2);

    using PType3 = int*;
    using Temp3 = internal::IsFunctionPointerRaw<PType3>;
    bool result3 = Temp3::result;
    
    ASSERT_FALSE(result3);

}


TEST(TypeTraitsTest, IsMemberFunctionPointerRaw) {

    using PType1 = void (NullType::*)();
    using Temp1 = internal::IsMemberFunctionPointerRaw<PType1>;
    bool result1 = Temp1::result;
    int parameters1 = Temp1::parameters;
    
    ASSERT_TRUE(result1);
    EXPECT_EQ(parameters1, 0);

    using PType2 = int (NullType::*)(double, int);
    using Temp2 = internal::IsMemberFunctionPointerRaw<PType2>;
    bool result2 = Temp2::result;
    int parameters2 = Temp2::parameters;
    
    ASSERT_TRUE(result2);
    EXPECT_EQ(parameters2, 2);

    using PType3 = void (NullType::*)(float) const;
    using Temp3 = internal::IsMemberFunctionPointerRaw<PType3>;
    bool result3 = Temp3::result;
    int parameters3 = Temp3::parameters;
    
    ASSERT_TRUE(result3);
    EXPECT_EQ(parameters3, 1);

    using PType4 = int (*) (int);
    using Temp4 = internal::IsMemberFunctionPointerRaw<PType4>;
    bool result4 = Temp4::result;
    
    ASSERT_FALSE(result4);

}


TEST(TypeTraitsTest, PToMTraits) {

    using PType1 = int NullType::*;
    using Temp1 = internal::PToMTraits<PType1>;
    using ResultRetT1 = typename Temp1::ReturnType;
    using ResultClassT1 = typename Temp1::ClassType;
    
    EXPECT_EQ(typeid(ResultRetT1), typeid(int));
    EXPECT_EQ(typeid(ResultClassT1), typeid(NullType));

    using PType2 = int*;
    using Temp2 = internal::PToMTraits<PType2>;
    using ResultRetT2 = typename Temp1::ReturnType;
    using ResultClassT2 = typename Temp1::ClassType;
    
    EXPECT_EQ(typeid(ResultRetT2), typeid(int));
    EXPECT_EQ(typeid(ResultClassT2), typeid(NullType));

}


TEST(TypeTraitsTest, Qualifiers) {

    using Type1 = int;
    
    using ResultCType1 = typename internal::AddConst<Type1>::Result;
    using NCType1 = typename internal::UnConst<ResultCType1>;
    using ResultNCType1 = typename NCType1::Result;

    ASSERT_TRUE(NCType1::isConst);
    EXPECT_EQ(typeid(ResultNCType1), typeid(Type1));

    using ResultVType1 = typename internal::AddVolatile<Type1>::Result;
    using NVType1 = typename internal::UnVolatile<ResultVType1>;
    using ResultNVType1 = typename NVType1::Result;

    ASSERT_TRUE(NVType1::isVolatile);
    EXPECT_EQ(typeid(ResultNVType1), typeid(Type1)); 

}


TEST(TypeTraitsTest, TypeTraits) {

    using Type1 = TypeTraits<int>;
    
    ASSERT_TRUE(Type1::isStdSignedInt);
    ASSERT_TRUE(Type1::isStdIntegral);
    ASSERT_TRUE(Type1::isStdArith);
    ASSERT_TRUE(Type1::isStdFundamental);

}