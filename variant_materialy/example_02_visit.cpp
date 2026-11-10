// example_02_visit.cpp
// std::visit: wywolanie wlasciwego kodu dla aktywnej alternatywy.
// Kompilacja: g++ -std=c++17 -Wall -Wextra example_02_visit.cpp -o example_02

#include <iostream>
#include <string>
#include <variant>
#include <vector>

using Value = std::variant<int, double, std::string>;

// Idiom "overloaded": sklejenie kilku lambd w jeden obiekt-wizytor.
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;  // od C++20 zbedne

int main() {
    std::vector<Value> values = {7, 2.5, std::string("abc"), 42};

    // 1) Wizytor jako lambda generyczna: jeden kod dla wszystkich typow.
    for (const auto& v : values) {
        std::visit([](const auto& x) { std::cout << x << " "; }, v);
    }
    std::cout << "\n";

    // 2) Wizytor "overloaded": osobna obsluga kazdego typu.
    //    Usuniecie lambdy dla std::string = blad kompilacji. Usuniecie lambdy dla int
    //    albo double NIE da bledu: int i double konwertuja sie na siebie niejawnie.
    for (const auto& v : values) {
        std::visit(overloaded{
                       [](int i) { std::cout << "int: " << i << "\n"; },
                       [](double d) { std::cout << "double: " << d << "\n"; },
                       [](const std::string& s) { std::cout << "string: \"" << s << "\"\n"; },
                   },
                   v);
    }

    // 3) visit moze zwracac wartosc: wszystkie galezie musza miec wspolny typ wyniku.
    double sum = 0.0;
    for (const auto& v : values) {
        sum += std::visit(overloaded{
                              [](int i) { return static_cast<double>(i); },
                              [](double d) { return d; },
                              [](const std::string& s) { return static_cast<double>(s.size()); },
                          },
                          v);
    }
    std::cout << "sum (numbers + string lengths): " << sum << "\n";
}

/* Wynik uruchomienia (g++ 16.2, -std=c++17, x64):
7 2.5 abc 42
int: 7
double: 2.5
string: "abc"
int: 42
sum (numbers + string lengths): 54.5
*/
