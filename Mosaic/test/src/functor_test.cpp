/*! @file functor_test.cpp
 *  @brief Tests for functor.
 */

#include "gtest/gtest.h"
#include "mosaic/utilities/functor.hpp"


struct TestStruct{

    int f(int i){
        return 1;
    }

    int operator ()(int i){
        return 1;
    }

    int operator ()(int i) const{
        return 1;
    }

};

TEST(FunctorTest, FunctorObjTest) {

    auto test_obj1 = TestStruct();
    auto fun1 = mosaic::Functor<int, int>(test_obj1);
    EXPECT_EQ(fun1(10), 1);

    const auto test_obj2 = TestStruct();
    auto fun2 = mosaic::Functor<int, int>(test_obj2);
    EXPECT_EQ(fun2(10), 1);

    auto test_obj3 = TestStruct();
    auto fun3 = mosaic::Functor<int, int>(std::move(test_obj3));
    EXPECT_EQ(fun3(10), 1);

}

TEST(FunctorTest, RetAndParamConversionTest) {

    auto test_obj1 = TestStruct();
    auto fun1 = mosaic::Functor<double, double>(test_obj1);

    ASSERT_EQ(typeid(double), typeid(fun1(10)));
    EXPECT_EQ(fun1(10), 1.0);

}


TEST(FunctorTest, LambdaTest) {

    int param1 = 10;

    auto lamda1 = [](){};
    auto fun1 = mosaic::Functor<void>(lamda1);
    fun1();


    auto lamda2 = [](int , int&, double&&){ return 1; };
    auto fun2 = mosaic::Functor<int, int, int&, double&&>(lamda2);
    EXPECT_EQ(fun2(10, param1, 2.0), 1);


    auto lamda3 = [](const int , const int&, const double&&){ return 1; };
    auto fun3 = mosaic::Functor<int, const int, const int&, const double&&>(lamda3);
    EXPECT_EQ(fun3(10, param1, 2.0), 1);

}


void f1() {
}

int f2(double, int&, int&&) {
    return 1;
}

int f3(const double, const int&, const int&&) {
    return 1;
}

TEST(FunctorTest, FunctionTest) {

    int param1 = 10;

    auto fun1 = mosaic::Functor<void>(f1);
    fun1();

    auto fun2 = mosaic::Functor<int, double, int&, int&&>(f2);
    EXPECT_EQ(fun2(2.0, param1, 10), 1);


    auto fun3 = mosaic::Functor<int, const double, const int&, const int&&>(f3);
    EXPECT_EQ(fun2(2.0, param1, 10), 1);

}


TEST(FunctorTest, MemberFunctionTest) {

    using MemFunc = int (TestStruct::*)(int);

    MemFunc mf = &TestStruct::f;

    auto test_obj1 = TestStruct();
    auto fun1 = mosaic::Functor<int, int>(&test_obj1, mf);
    EXPECT_EQ(fun1(10), 1);

}
