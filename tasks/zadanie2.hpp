#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

// Zadanie 2: std::visit - lambda generyczna, idiom overloaded, zwracanie wartości
// Rozwiązuj przez std::visit - bez index(), holds_alternative, get i get_if

using Value = std::variant<int, double, std::string>;

// Gotowe: idiom overloaded - skleja kilka lambd w jeden obiekt z wieloma operator()
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// TODO 1: wartość jako tekst:  42 -> "42",   3.5 -> "3.5",   std::string("abc") -> "abc"
inline std::string to_text(const Value& v) {
    return "";
}

// TODO 2: podwój wartość w miejscu, typ się nie zmienia:  21 -> 42,   1.25 -> 2.5,   "ab" -> "abab"
inline void double_it(Value& v) {
}

// TODO 3: „waga” wartości jako double:  int i -> i,   double d -> d,   napis -> jego długość.
//         weight(7) == 7.0,   weight(2.5) == 2.5,   weight(std::string("abcd")) == 4.0
inline double weight(const Value& v) {
    throw std::logic_error("weight: nie zaimplementowano");
}
