// example_03_praktyczne.cpp
// Praktyczne wzorce: (1) wynik albo blad, (2) maszyna stanow oparta na typach.
// Kompilacja: g++ -std=c++17 -Wall -Wextra example_03_praktyczne.cpp -o example_03

#include <iostream>
#include <string>
#include <variant>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// ---------- 1) Wynik albo blad ----------
// Funkcja zwraca poprawna wartosc ALBO opis bledu; wywolujacy musi jawnie
// sprawdzic, co dostal. Ten sam pomysl trafil do C++23 jako std::expected<T, E>.

struct Error {
    std::string message;
};
using Result = std::variant<int, Error>;

Result parse_int(const std::string& text) {
    try {
        std::size_t consumed = 0;
        int n = std::stoi(text, &consumed);
        if (consumed != text.size()) return Error{"trailing characters after number in '" + text + "'"};
        return n;
    } catch (const std::exception&) {
        return Error{"'" + text + "' is not a number"};
    }
}

// ---------- 2) Maszyna stanow ----------
// Kazdy stan to osobny typ, ktory przechowuje tylko te dane, ktore maja sens
// w tym stanie (numer proby istnieje tylko podczas laczenia itd.).

struct Disconnected {};
struct Connecting {
    int attempt;
};
struct Connected {
    std::string address;
};
using State = std::variant<Disconnected, Connecting, Connected>;

State step(const State& s) {
    return std::visit(overloaded{
                          [](Disconnected) -> State { return Connecting{1}; },
                          [](Connecting c) -> State {
                              if (c.attempt >= 3) return Connected{"10.0.0.1:443"};
                              return Connecting{c.attempt + 1};
                          },
                          [](const Connected& c) -> State { return c; },
                      },
                      s);
}

std::string describe(const State& s) {
    return std::visit(overloaded{
                          [](Disconnected) { return std::string("disconnected"); },
                          [](Connecting c) { return "connecting (attempt " + std::to_string(c.attempt) + ")"; },
                          [](const Connected& c) { return "connected to " + c.address; },
                      },
                      s);
}

int main() {
    // 1) wynik albo blad
    for (const std::string t : {"42", "-7", "13abc", "xyz"}) {
        Result r = parse_int(t);
        if (auto* number = std::get_if<int>(&r)) {
            std::cout << t << " -> " << *number << "\n";
        } else {
            std::cout << t << " -> ERROR: " << std::get<Error>(r).message << "\n";
        }
    }

    // 2) maszyna stanow
    State s = Disconnected{};
    for (int i = 0; i < 6; ++i) {
        std::cout << "state: " << describe(s) << "\n";
        s = step(s);
    }
}

/* Wynik uruchomienia (g++ 16.2, -std=c++17, x64):
42 -> 42
-7 -> -7
13abc -> ERROR: trailing characters after number in '13abc'
xyz -> ERROR: 'xyz' is not a number
state: disconnected
state: connecting (attempt 1)
state: connecting (attempt 2)
state: connecting (attempt 3)
state: connected to 10.0.0.1:443
state: connected to 10.0.0.1:443
*/
