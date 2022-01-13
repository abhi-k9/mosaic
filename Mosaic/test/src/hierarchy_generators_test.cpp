/*! @file hierarchy_generators_test.cpp
 *  @brief Tests for all hierarchy generators related functionality.
 */

#include "gtest/gtest.h"
#include "mosaic/utilities/hierarchy_generators.hpp"
#include "mosaic/utilities/conversion.hpp"

template <class T>
struct Unit {
    int i;
};


TEST(HierarchyGen, ScatterHierarchyNoRepeatTypes) {

    using TL = typename mosaic::MakeTL<int, float, double, char>::TL;
    using SH = mosaic::GenScatterHierarchy<TL, Unit>;

    bool result1 = mosaic::SUPERSUBCLASS_STRICT<Unit<int>, SH>();
    EXPECT_TRUE(result1);
    
    bool result2 = mosaic::SUPERSUBCLASS_STRICT<Unit<float>, SH>();
    EXPECT_TRUE(result2);
    
    bool result3 = mosaic::SUPERSUBCLASS_STRICT<Unit<char>, SH>();
    EXPECT_TRUE(result3);   

}


template <class T>
struct SpecialUnit {

    SpecialUnit(): i(s) {
        ++s;
    };

    static int s;
    int i;
};

template<class T> int SpecialUnit<T>::s = 0; 

TEST(HierarchyGen, ScatterHierarchyRepeatAmbiguityWorkaround) {

    using TL = typename mosaic::MakeTL<int, float, int, char>::TL;
    using SH = mosaic::GenScatterHierarchy<TL, SpecialUnit>;
    using F = mosaic::Field<SH>;

    auto sh_obj = SH();

    int result0 = F::Get<0>(sh_obj).i;
    EXPECT_EQ(result0, 0);
    
    int result1 = F::Get<1>(sh_obj).i;
    EXPECT_EQ(result1, 0);

    int result2 = F::Get<2>(sh_obj).i;
    EXPECT_EQ(result2, 1);

    int result3 = F::Get<3>(sh_obj).i;
    EXPECT_EQ(result3, 0);

}


TEST(HierarchyGen, ScatterTags) {

    using TL = typename mosaic::MakeTL<int, float, int, char>::TL;
    using SH = mosaic::GenScatterHierarchy<TL, Unit>;

    using Tag1 = mosaic::ScatterTag<int, typename mosaic::MakeTL<float, int ,char>::TL, Unit>;
    bool result1 = mosaic::SUPERSUBCLASS_STRICT<Tag1, SH>();
    EXPECT_TRUE(result1);
    
    using Tag2 = mosaic::ScatterTag<float, typename mosaic::MakeTL<int ,char>::TL, Unit>;
    bool result2 = mosaic::SUPERSUBCLASS_STRICT<Tag2, SH>();
    EXPECT_TRUE(result2);
    
    using Tag3 = mosaic::ScatterTag<int, typename mosaic::MakeTL<char>::TL, Unit>;
    bool result3 = mosaic::SUPERSUBCLASS_STRICT<Tag3, SH>();
    EXPECT_TRUE(result3);   

}


template <class T, class LH>
struct ReinheritorUnit: public LH {
    int i;
};

TEST(HierarchyGen, LinearHierarchy) {

    using TL = typename mosaic::MakeTL<int, float, char>::TL;
    using LH = mosaic::GenLinearHierarchy<TL, ReinheritorUnit>;

    using TempTL1 = typename mosaic::MakeTL<float, char>::TL;
    using TempLH1 = mosaic::GenLinearHierarchy<TempTL1, ReinheritorUnit>;
    bool result1 = mosaic::SUPERSUBCLASS_STRICT<ReinheritorUnit<int, TempLH1>, LH>();
    EXPECT_TRUE(result1);
    
    using TempTL2 = typename mosaic::MakeTL<char>::TL;
    using TempLH2 = mosaic::GenLinearHierarchy<TempTL2, ReinheritorUnit>;
    bool result2 = mosaic::SUPERSUBCLASS_STRICT<ReinheritorUnit<float, TempLH2>, LH>();
    EXPECT_TRUE(result2);

}