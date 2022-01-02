/*! @file typelist_test.cpp
 *  @brief Tests for all `Typelist` related functionality.
 */

#include "gtest/gtest.h"
#include "mosaic/utilities/typelist.hpp"

TEST(MakeTLTest, CreateCorrectTypelist) {

    auto &created_typeinfo = typeid(typename mosaic::MakeTL<bool, char, int, double>::TL);
    auto &expected_typeinfo = typeid(mosaic::Typelist<bool, mosaic::Typelist<char, mosaic::Typelist<int, mosaic::Typelist<double, mosaic::NullType>>>>);
    
    ASSERT_EQ(
        created_typeinfo,
        expected_typeinfo
    )
    << "MakeTL failed, created " << created_typeinfo.name()
    << " instead of " << expected_typeinfo.name();

}
