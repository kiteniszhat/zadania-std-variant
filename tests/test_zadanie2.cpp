#include <gtest/gtest.h>

#include <string>
#include <variant>

#include "zadanie2.hpp"

TEST(Zadanie2_ToText, KazdyTyp) {
    EXPECT_EQ(to_text(Value(42)), "42");
    EXPECT_EQ(to_text(Value(-7)), "-7");
    EXPECT_EQ(to_text(Value(3.5)), "3.5");
    EXPECT_EQ(to_text(Value(std::string("abc"))), "abc");
}

TEST(Zadanie2_ToText, NieZmieniaWariantu) {
    const Value v = std::string("stała");
    EXPECT_EQ(to_text(v), "stała");
    EXPECT_EQ(std::get<std::string>(v), "stała");
}

TEST(Zadanie2_DoubleIt, Int) {
    Value v = 21;
    double_it(v);
    ASSERT_TRUE(std::holds_alternative<int>(v)) << "typ ma zostać ten sam";
    EXPECT_EQ(std::get<int>(v), 42);
}

TEST(Zadanie2_DoubleIt, Double) {
    Value v = 1.25;
    double_it(v);
    ASSERT_TRUE(std::holds_alternative<double>(v)) << "typ ma zostać ten sam";
    EXPECT_DOUBLE_EQ(std::get<double>(v), 2.5);
}

TEST(Zadanie2_DoubleIt, Napis) {
    Value v = std::string("ab");
    double_it(v);
    ASSERT_TRUE(std::holds_alternative<std::string>(v)) << "typ ma zostać ten sam";
    EXPECT_EQ(std::get<std::string>(v), "abab");
    double_it(v);
    EXPECT_EQ(std::get<std::string>(v), "abababab");
}

TEST(Zadanie2_Weight, LiczbyBezZmian) {
    EXPECT_DOUBLE_EQ(weight(Value(7)), 7.0);
    EXPECT_DOUBLE_EQ(weight(Value(2.5)), 2.5) << "double nie może zostać obcięty do int";
    EXPECT_DOUBLE_EQ(weight(Value(-0.75)), -0.75);
}

TEST(Zadanie2_Weight, NapisToDlugosc) {
    EXPECT_DOUBLE_EQ(weight(Value(std::string("abcd"))), 4.0);
    EXPECT_DOUBLE_EQ(weight(Value(std::string())), 0.0);
}
