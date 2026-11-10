// example_04_ciekawostki.cpp
// Ciekawostki: rozmiar w pamieci, duplikaty typow, valueless_by_exception,
// monostate, visit na dwoch wariantach naraz, porownywanie.
// Kompilacja: g++ -std=c++17 -Wall -Wextra example_04_ciekawostki.cpp -o example_04

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

// Typ, ktorego kopiowanie rzuca wyjatkiem - do demonstracji valueless_by_exception.
struct Bomb {
    Bomb() = default;
    Bomb(const Bomb&) { throw std::runtime_error("exploded while copying"); }
};

// Typ bez konstruktora domyslnego - do demonstracji monostate.
struct NoDefault {
    int x;
    explicit NoDefault(int value) : x(value) {}
};

int main() {
    std::cout << std::boolalpha;

    // 1) Rozmiar: wartosc + znacznik aktywnej alternatywy, zero alokacji na stercie.
    std::cout << "sizeof(double)               = " << sizeof(double) << "\n";
    std::cout << "sizeof(variant<char>)        = " << sizeof(std::variant<char>) << "\n";
    std::cout << "sizeof(variant<int, double>) = " << sizeof(std::variant<int, double>) << "\n";
    std::cout << "sizeof(string)               = " << sizeof(std::string) << "\n";
    std::cout << "sizeof(variant<int, string>) = " << sizeof(std::variant<int, std::string>) << "\n";

    // 2) Duplikaty typow sa legalne; rozroznia sie je wtedy tylko indeksem.
    std::variant<int, int> twins(std::in_place_index<1>, 7);
    // std::get<int>(twins);  // blad kompilacji: niejednoznaczne
    std::cout << "active index: " << twins.index() << ", value: " << std::get<1>(twins) << "\n";

    // 3) valueless_by_exception: wyjatek w trakcie zmiany typu (stara wartosc juz
    //    zniszczona, nowa nie powstala) zostawia wariant "bez wartosci".
    std::variant<int, Bomb> v = 42;
    try {
        Bomb b;
        v = b;  // kopia rzuca w polowie zmiany typu int -> Bomb
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << "\n";
    }
    std::cout << "valueless_by_exception: " << v.valueless_by_exception() << "\n";
    v = 5;  // zwykle przypisanie przywraca normalny stan
    std::cout << "after repair holds: " << std::get<int>(v) << "\n";

    // 4) monostate: umozliwia domyslna konstrukcje, gdy pierwsza alternatywa jej nie ma.
    // std::variant<NoDefault> z1;  // blad kompilacji
    std::variant<std::monostate, NoDefault> z;
    std::cout << "monostate active: " << std::holds_alternative<std::monostate>(z) << "\n";

    // 5) visit na kilku wariantach naraz (multi-dispatch).
    std::variant<int, double> a = 2, b = 0.5;
    std::visit([](auto x, auto y) { std::cout << "a + b = " << x + y << "\n"; }, a, b);

    // 6) Porownywanie: najpierw indeks aktywnej alternatywy, potem wartosci.
    std::variant<int, std::string> p = 1, q = std::string("a");
    std::cout << "p < q: " << (p < q) << "\n";  // true, bo indeks 0 < indeks 1
}

/* Wynik uruchomienia (g++ 16.2, -std=c++17, x64):
sizeof(double)               = 8
sizeof(variant<char>)        = 2
sizeof(variant<int, double>) = 16
sizeof(string)               = 32
sizeof(variant<int, string>) = 40
active index: 1, value: 7
exception: exploded while copying
valueless_by_exception: true
after repair holds: 5
monostate active: true
a + b = 2.5
p < q: true
*/
