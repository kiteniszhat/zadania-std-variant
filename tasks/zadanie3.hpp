#pragma once

#include <stdexcept>
#include <type_traits>
#include <variant>

// Zadanie 3: std::visit na dwóch wariantach naraz 
// Rozwiązuj przez std::visit - bez index(), holds_alternative, get i get_if

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using Number = std::variant<int, double>;

// TODO 1: suma, której typ zależy od typów składników:  int + int -> int,   gdy choć jeden jest double -> double
//         add(2, 3) trzyma int 5,   add(2, 0.5) trzyma double 2.5
inline Number add(const Number& a, const Number& b) {
    throw std::logic_error("add: nie zaimplementowano");
}

// TODO 2: iloraz:  int / int dzielące się bez reszty -> int (6 / 3 -> 2),
//                  int / int z resztą -> double (7 / 2 -> 3.5),   gdy choć jeden jest double -> double
//         Dzielenie int przez 0 nie będzie testowane
inline Number divide(const Number& a, const Number& b) {
    throw std::logic_error("divide: nie zaimplementowano");
}

// Kamień, papier, nożyce: każdy ruch to osobny typ.
struct Rock {};
struct Paper {};
struct Scissors {};
using Hand = std::variant<Rock, Paper, Scissors>;

// TODO 3: wynik pojedynku:  1 gdy wygrywa a,  -1 gdy wygrywa b,  0 przy remisie
//         duel(Rock{}, Scissors{}) == 1
inline int duel(const Hand& a, const Hand& b) {
    throw std::logic_error("duel: nie zaimplementowano");
}
