#include <gtest/gtest.h>

#include <string>
#include <variant>

#include "zadanie1.hpp"

TEST(Zadanie1_TypeName, RozpoznajeAlternatywe) {
    EXPECT_EQ(type_name(Value(42)), "int");
    EXPECT_EQ(type_name(Value(2.5)), "double");
    EXPECT_EQ(type_name(Value(std::string("abc"))), "string");
}

TEST(Zadanie1_TypeName, DomyslnieInt) {
    Value v;
    EXPECT_EQ(type_name(v), "int") << "konstruktor domyślny tworzy pierwszą alternatywę: int{0}";
}

TEST(Zadanie1_TypeName, PoPrzypisaniuZmieniaTyp) {
    Value v = 1;
    v = std::string("tekst");
    EXPECT_EQ(type_name(v), "string") << "przypisanie zmienia aktywną alternatywę";
    v = 0.5;
    EXPECT_EQ(type_name(v), "double");
}

TEST(Zadanie1_IntOr, ZwracaInt) {
    EXPECT_EQ(int_or(Value(7), -1), 7);
    EXPECT_EQ(int_or(Value(0), -1), 0);
}

TEST(Zadanie1_IntOr, InnyTypDajeFallback) {
    EXPECT_EQ(int_or(Value(2.5), -1), -1) << "w środku jest double, a nie int - bez konwersji";
    EXPECT_EQ(int_or(Value(std::string("7")), -1), -1) << "napis \"7\" to nie int";
}

TEST(Zadanie1_Text, ZwracaReferencjeDoWnetrza) {
    Value v = std::string("ala");
    text(v) += " ma kota";
    EXPECT_EQ(std::get<std::string>(v), "ala ma kota") << "text ma zwrócić referencję do napisu w wariancie, nie kopię";
    EXPECT_EQ(&text(v), std::get_if<std::string>(&v));
}

TEST(Zadanie1_Text, ZlyTypRzucaWyjatek) {
    Value i = 5;
    Value d = 1.5;
    EXPECT_THROW(text(i), std::bad_variant_access);
    EXPECT_THROW(text(d), std::bad_variant_access);
    EXPECT_EQ(std::get<int>(i), 5) << "nieudany odczyt nie zmienia wariantu";
}

TEST(Zadanie1_SetRepeated, ZmieniaTypIWartosc) {
    Value v = 42;
    std::string& s = set_repeated(v, 3, 'x');
    ASSERT_TRUE(std::holds_alternative<std::string>(v));
    EXPECT_EQ(std::get<std::string>(v), "xxx");
    EXPECT_EQ(&s, std::get_if<std::string>(&v)) << "set_repeated ma zwrócić referencję do napisu w wariancie";
    EXPECT_EQ(v.index(), 2u);
}

TEST(Zadanie1_SetRepeated, ZastepujeIstniejacyNapis) {
    Value v = std::string("stary");
    set_repeated(v, 0, '?');
    EXPECT_EQ(std::get<std::string>(v), "");
    set_repeated(v, 2, 'a') += "b";
    EXPECT_EQ(std::get<std::string>(v), "aab");
}
