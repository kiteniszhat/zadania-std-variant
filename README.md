# Zadania - std::variant

| Zadanie | Temat |
| --- | --- |
| 1 | podstawy: `index`, `holds_alternative`, `get_if`, `get` i `bad_variant_access`, `emplace` |
| 2 | `std::visit`: lambda generyczna, idiom `overloaded`, wspólny typ zwracany |
| 3 | `std::visit` na dwóch wariantach naraz (multi-dispatch) |
| 4 | praktyczne: maszyna stanów - zamek szyfrowy w drzwiach |
| 5 | praktyczne: „wynik albo błąd” i kolejka poleceń - bankomat |

## Uruchomienie

Wymagania: CMake >= 3.14, kompilator z **C++17** (GCC 7+ / Clang 5+ / MSVC 2017 15.7+).

```bash
cmake -S . -B build
cmake --build build
```

```bash
./build/zadanie1                                       # testy zadania 1 (analogicznie 2-5)
./build/zadanie3 --gtest_filter='Zadanie3_Duel.*'      # tylko wybrane testy
cd build && ctest -L zadanie --output-on-failure       # wszystkie zadania
```
