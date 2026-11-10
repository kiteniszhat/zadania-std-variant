#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <variant>

// Zadanie 1: podstawy - sprawdzanie zawartości, odczyt i zmiana wartości

using Value = std::variant<int, double, std::string>;

// TODO 1: nazwa aktywnej alternatywy:  42 -> "int",   2.5 -> "double",   std::string("a") -> "string"
inline std::string type_name(const Value& v) {
    return "";
}

// TODO 2: wartość int, a gdy w środku jest coś innego - fallback.   int_or(7, -1) == 7,   int_or(2.5, -1) == -1
inline int int_or(const Value& v, int fallback) {
    throw std::logic_error("int_or: nie zaimplementowano");
}

// TODO 3: referencja do napisu przechowywanego w v; gdy w środku nie ma napisu - wyjątek std::bad_variant_access
//         text(v) += "!" ma zmienić napis wewnątrz wariantu
inline std::string& text(Value& v) {
    throw std::logic_error("text: nie zaimplementowano");
}

// TODO 4: zastąp zawartość v napisem złożonym z n znaków c (bez tworzenia tymczasowego std::string)
//         i zwróć referencję do niego.   set_repeated(v, 3, 'x') -> v trzyma "xxx"
inline std::string& set_repeated(Value& v, std::size_t n, char c) {
    throw std::logic_error("set_repeated: nie zaimplementowano");
}
