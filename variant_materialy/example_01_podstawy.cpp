// example_01_podstawy.cpp
// Podstawy std::variant: tworzenie, sprawdzanie i odczyt zawartosci.
// Kompilacja: g++ -std=c++17 -Wall -Wextra example_01_podstawy.cpp -o example_01

#include <iostream>
#include <string>
#include <variant>

int main() {
    std::cout << std::boolalpha;

    // Domyslnie wariant przechowuje pierwsza alternatywe: int{0}.
    std::variant<int, double, std::string> v;
    std::cout << "initial index: " << v.index() << "\n";

    v = 3.14;                  // teraz aktywny jest double
    v = std::string("text");   // teraz std::string

    // 1) holds_alternative: czy aktywny jest dany typ?
    std::cout << "holds string? " << std::holds_alternative<std::string>(v) << "\n";
    std::cout << "holds int?    " << std::holds_alternative<int>(v) << "\n";

    // 2) get<T>: odczyt; przy zlym typie wyjatek bad_variant_access.
    std::cout << "value: " << std::get<std::string>(v) << "\n";
    try {
        std::cout << std::get<int>(v) << "\n";    // aktywny jest string!
    } catch (const std::bad_variant_access& e) {
        std::cout << "exception: " << e.what() << "\n";
    }

    // 3) get_if: wskaznik do wartosci albo nullptr, bez wyjatkow.
    //    Uwaga: przyjmuje WSKAZNIK na wariant.
    if (auto* s = std::get_if<std::string>(&v)) {
        std::cout << "get_if: \"" << *s << "\", length " << s->size() << "\n";
    }
    if (std::get_if<int>(&v) == nullptr) {
        std::cout << "get_if<int> returned nullptr\n";
    }

    // 4) index() i get<I>: dostep po numerze alternatywy.
    v = 42;
    std::cout << "index: " << v.index() << ", get<0>: " << std::get<0>(v) << "\n";

    // 5) emplace: konstrukcja nowej wartosci w miejscu (tu: string "xxxxx").
    v.emplace<std::string>(5, 'x');
    std::cout << "after emplace: " << std::get<std::string>(v) << "\n";
}

/* Wynik uruchomienia (g++ 16.2, -std=c++17, x64):
initial index: 0
holds string? true
holds int?    false
value: text
exception: std::get: wrong index for variant
get_if: "text", length 4
get_if<int> returned nullptr
index: 0, get<0>: 42
after emplace: xxxxx
*/
