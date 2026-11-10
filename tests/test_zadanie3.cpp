#include <gtest/gtest.h>

#include <variant>

#include "zadanie3.hpp"

TEST(Zadanie3_Add, IntPlusIntToInt) {
    const Number r = add(2, 3);
    ASSERT_TRUE(std::holds_alternative<int>(r)) << "int + int ma dać int";
    EXPECT_EQ(std::get<int>(r), 5);
}

TEST(Zadanie3_Add, ZDoubleToDouble) {
    const Number r1 = add(2, 0.5);
    ASSERT_TRUE(std::holds_alternative<double>(r1)) << "int + double ma dać double";
    EXPECT_DOUBLE_EQ(std::get<double>(r1), 2.5);

    const Number r2 = add(0.25, 1);
    ASSERT_TRUE(std::holds_alternative<double>(r2)) << "double + int ma dać double";
    EXPECT_DOUBLE_EQ(std::get<double>(r2), 1.25);

    const Number r3 = add(1.5, 1.5);
    ASSERT_TRUE(std::holds_alternative<double>(r3)) << "double + double to double, nawet gdy wynik jest całkowity";
    EXPECT_DOUBLE_EQ(std::get<double>(r3), 3.0);
}

TEST(Zadanie3_Divide, IntBezResztyToInt) {
    const Number r = divide(6, 3);
    ASSERT_TRUE(std::holds_alternative<int>(r));
    EXPECT_EQ(std::get<int>(r), 2);

    const Number neg = divide(-8, 2);
    ASSERT_TRUE(std::holds_alternative<int>(neg));
    EXPECT_EQ(std::get<int>(neg), -4);
}

TEST(Zadanie3_Divide, IntZResztaToDouble) {
    const Number r = divide(7, 2);
    ASSERT_TRUE(std::holds_alternative<double>(r)) << "7 / 2 nie dzieli się bez reszty - wynik ma być double";
    EXPECT_DOUBLE_EQ(std::get<double>(r), 3.5) << "dzielenie całkowite obcięłoby wynik do 3";

    const Number neg = divide(-7, 2);
    ASSERT_TRUE(std::holds_alternative<double>(neg));
    EXPECT_DOUBLE_EQ(std::get<double>(neg), -3.5);
}

TEST(Zadanie3_Divide, ZDoubleToDouble) {
    const Number r1 = divide(2, 0.5);
    ASSERT_TRUE(std::holds_alternative<double>(r1)) << "int / double ma dać double, nawet gdy wynik jest całkowity";
    EXPECT_DOUBLE_EQ(std::get<double>(r1), 4.0);

    const Number r2 = divide(1.5, 3);
    ASSERT_TRUE(std::holds_alternative<double>(r2));
    EXPECT_DOUBLE_EQ(std::get<double>(r2), 0.5);
}

TEST(Zadanie3_Duel, Remisy) {
    EXPECT_EQ(duel(Rock{}, Rock{}), 0);
    EXPECT_EQ(duel(Paper{}, Paper{}), 0);
    EXPECT_EQ(duel(Scissors{}, Scissors{}), 0);
}

TEST(Zadanie3_Duel, WygrywaPierwszy) {
    EXPECT_EQ(duel(Rock{}, Scissors{}), 1) << "kamień tępi nożyce";
    EXPECT_EQ(duel(Scissors{}, Paper{}), 1) << "nożyce tną papier";
    EXPECT_EQ(duel(Paper{}, Rock{}), 1) << "papier owija kamień";
}

TEST(Zadanie3_Duel, WygrywaDrugi) {
    EXPECT_EQ(duel(Scissors{}, Rock{}), -1);
    EXPECT_EQ(duel(Paper{}, Scissors{}), -1);
    EXPECT_EQ(duel(Rock{}, Paper{}), -1);
}

TEST(Zadanie3_Duel, Antysymetria) {
    const Hand hands[] = {Rock{}, Paper{}, Scissors{}};
    for (const Hand& a : hands) {
        for (const Hand& b : hands) {
            EXPECT_EQ(duel(a, b), -duel(b, a)) << "zamiana graczy odwraca wynik (indeksy " << a.index() << ", "
                                                << b.index() << ")";
        }
    }
}
