# `std::variant`: bezpieczna typowo unia (C++17)

> Każdy fragment kodu poniżej to kompletny program, który kompiluje się samodzielnie.
> Dłuższe przykłady są w plikach `example_01_podstawy.cpp` … `example_04_ciekawostki.cpp`
> (opis w sekcji 4). Wszystko sprawdzono kompilatorem g++ (`-std=c++17 -Wall -Wextra`);
> na końcu każdego pliku jest wynik uruchomienia.

## 1. Po co to jest i jakie problemy rozwiązuje

**`std::variant<T1, T2, …>`** (nagłówek `<variant>`, od C++17) to typ, który w danej
chwili przechowuje **dokładnie jedną** wartość spośród listy typów (tzw. *alternatyw*)
ustalonej w czasie kompilacji. Wariant **zawsze wie, która alternatywa jest aktywna**.
Taka konstrukcja nazywa się *tagged union* (unia ze znacznikiem) albo *sum type*
(typ sumaryczny); odpowiada `enum` z Rusta czy typom algebraicznym z języków funkcyjnych.

```cpp
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::variant<int, double, std::string> v;   // trzyma int{0} (pierwsza alternatywa)
    v = 3.14;                                   // teraz trzyma double
    v = std::string("text");                    // teraz trzyma std::string

    std::cout << std::get<std::string>(v) << '\n';   // text
}
```

### Punkt wyjścia: czym zawodzi `union`

Klasyczna unia z C pozwala kilku polom dzielić tę samą pamięć, ale:

| Problem z `union` | Rozwiązanie w `std::variant` |
| --- | --- |
| nie pamięta, które pole jest aktywne, więc znacznik trzeba dopisywać ręcznie | `index()`, `std::holds_alternative`, `std::get_if` |
| odczyt nieaktywnego pola to *undefined behavior*, a kod i tak się kompiluje | błędny odczyt to wyjątek `std::bad_variant_access` (albo `nullptr` z `get_if`) |
| przy typach nietrywialnych (`std::string`, `std::vector`) konstruktory i destruktory trzeba wołać ręcznie | przy każdej zmianie typu poprawnie niszczy starą wartość i konstruuje nową |
| po dopisaniu nowego pola trzeba samemu znaleźć każde miejsce, które sprawdza znacznik; zapomniany przypadek kompiluje się bez błędu | kod obsługujący wariant przez `std::visit` przestaje się kompilować, dopóki nowy typ nie zostanie obsłużony (przykład i wyjątki od tej reguły w sekcji 3) |

### 

Zmienną, która może przechowywać wartości różnych typów, da się w C++ zrobić na kilka
sposobów. Różnią się tym, czy lista typów jest ustalona z góry, jak odczytuje się
wartość i gdzie ta wartość leży w pamięci.

Dziedziczenie z funkcjami wirtualnymi to rozwiązanie obiektowe: klasa bazowa
i osobna klasa pochodna dla każdego przypadku. Lista typów jest otwarta, bo nową klasę
pochodną można dopisać później, nawet w innej bibliotece, bez zmieniania istniejącego
kodu. Obiekty różnych klas mają jednak różne rozmiary, więc kontener przechowuje
wskaźniki do bazy (zwykle `std::unique_ptr<Base>`), a same obiekty powstają na stercie.
Wariant ma stały rozmiar (największa alternatywa plus znacznik), więc wartość leży
bezpośrednio w nim, np. na stosie albo w ciągłej pamięci `std::vector`, bez alokacji.

| Zmiana w programie | dziedziczenie + `virtual` | `std::variant` + `visit` |
| --- | --- | --- |
| nowy typ, np. `Triangle` | jedna nowa klasa, reszta kodu bez zmian | trzeba uzupełnić każdy wizytor, a kompilator wskaże które |
| nowa operacja, np. `perimeter()` | nowa metoda wirtualna w bazie i w każdej klasie pochodnej | jedna nowa funkcja z wizytorem, typy bez zmian |

`std::any` (nagłówek `<any>`) to pojemnik na wartość **dowolnego** typu (byle dało się
ją kopiować). Nie podaje się żadnej listy typów, więc ta sama zmienna może trzymać raz
liczbę, raz napis, raz własną strukturę. Przy odczycie trzeba za to podać typ, którego
się spodziewamy; jeśli w środku jest coś innego, poleci wyjątek:

```cpp
#include <any>
#include <iostream>
#include <string>

int main() {
    std::any a = 42;                          // w środku int
    a = std::string("text");                  // teraz std::string

    auto s = std::any_cast<std::string>(a);   // OK: "text"
    std::cout << s << '\n';

    try {
        auto i = std::any_cast<int>(a);       // wyjątek std::bad_any_cast
        std::cout << i << '\n';
    } catch (const std::bad_any_cast&) {
        std::cout << "caught std::bad_any_cast\n";
    }
}
```

Kompilator nie wie, co może siedzieć w `any`. nie wskaże
nieobsłużonego typu, a każda pomyłka wyjdzie dopiero w czasie działania programu.

`any` trzyma w sobie tylko małe wartości, większe alokuje na stercie.

`std::optional<T>`  oznacza „wartość typu `T` albo nic”.
Typowe użycie to funkcja, która nie zawsze ma co zwrócić, np. wyszukiwanie. Zamiast
umownej wartości (`-1`, pusty napis) albo wyjątku zwraca wtedy pusty `optional`:

```cpp
#include <iostream>
#include <map>
#include <optional>
#include <string>

std::map<std::string, std::string> phone_book{{"Mathura.pl", "533 675 751"}};

std::optional<std::string> find_phone(const std::string& name) {
    auto it = phone_book.find(name);
    if (it == phone_book.end()) return std::nullopt;   // nie znaleziono: pusty optional
    return it->second;                                 // znaleziono: optional z wartością
}

int main() {
    if (auto t = find_phone("Mathura.pl")) {   // czy jest wartość?
        std::cout << *t << '\n';               // *t odczytuje numer do szefa firmy mathura
    }
    std::cout << find_phone("Jarosina.pl").value_or("no number") << '\n';   // no number
}
```

`std::variant` pasuje więc wtedy, gdy wszystkie możliwe typy są znane w chwili pisania
kodu (rodzaje tokenów w parserze, stany automatu, rodzaje komunikatów) i zależy nam,
żeby kompilator pilnował ich obsługi. Gdy nowe typy mają dochodzić później, np. jako
wtyczki albo klasy pisane przez użytkowników biblioteki, lepiej sprawdza się
dziedziczenie. `std::any` zostaje na sytuacje, w których typu nie zna nawet autor
kodu przechowującego wartość, np. dowolne dane użytkownika doczepiane do zdarzenia
w bibliotece GUI.

### Typowe zastosowania

- **wynik albo błąd** z funkcji, zamiast wyjątków i kodów błędów (poprzednik `std::expected` z C++23);
- **maszyny stanów**: każdy stan to osobny typ przechowujący tylko swoje dane;
- **parsery i drzewa wyrażeń**: węzeł to liczba *albo* nazwa *albo* operacja (tak działa np. wartość w JSON-ie);
- **niejednorodne kontenery bez dziedziczenia**, np. `std::vector<std::variant<…>>`;
- **komunikaty / zdarzenia** różnych rodzajów przechodzące przez jedną kolejkę.

## 2. Podstawy użycia

### Tworzenie i zmiana wartości

```cpp
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::variant<int, double, std::string> v;  // domyślnie: pierwsza alternatywa, int{0}
    std::variant<int, double> a = 2.5;         // konwersja wybiera double
    v = 42;                                    // przypisanie zmienia aktywny typ
    v.emplace<std::string>(3, 'x');            // konstrukcja w miejscu: "xxx"

    std::cout << a.index() << ' ' << std::get<std::string>(v) << '\n';   // 1 xxx
}
```

Domyślny konstruktor wariantu tworzy wartość **pierwszej** alternatywy, więc musi ona
mieć konstruktor domyślny (jeśli nie ma, patrz `std::monostate` niżej).

### Sprawdzanie zawartości

```cpp
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::variant<int, double, std::string> v = 3.14;

    std::cout << v.index() << '\n';                         // numer aktywnej alternatywy: 1
    std::cout << std::boolalpha
              << std::holds_alternative<double>(v) << '\n'; // czy aktywny typ to double? true
}
```

### Trzy sposoby odczytu

| Sposób | Przy złym typie | 
| --- | --- |
| `std::get<T>(v)`, `std::get<I>(v)` | wyjątek `std::bad_variant_access` 
| `std::get_if<T>(&v)` → wskaźnik | zwraca `nullptr` (uwaga: przyjmuje **wskaźnik** na wariant) 
| `std::visit(visitor, v)` | nie dotyczy, wizytor obsługuje wszystkie typy 

```cpp
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::variant<int, double, std::string> v = std::string("text");

    if (auto* s = std::get_if<std::string>(&v)) {
        std::cout << "string of length " << s->size() << '\n';    // string of length 4
    }
    if (auto* d = std::get_if<double>(&v)) {                       // nullptr: w środku nie ma double
        std::cout << "double " << *d << '\n';
    }
}
```

### Zaślepka `std::monostate`

```cpp
#include <iostream>
#include <string>
#include <utility>
#include <variant>

struct Connection {
    explicit Connection(std::string addr) : address(std::move(addr)) {}   // brak konstruktora domyślnego
    std::string address;
};

int main() {
    std::variant<std::monostate, Connection> p;   // OK: startuje jako monostate
    // std::variant<Connection> q;                // błąd kompilacji: Connection nie ma konstruktora domyślnego

    std::cout << p.index() << '\n';                           // 0
    p.emplace<Connection>("localhost:8080");
    std::cout << std::get<Connection>(p).address << '\n';     // localhost:8080
}
```

`std::monostate` to pusty typ-wypełniacz. Jeżeli jednak chodzi tylko o „wartość albo
nic”, czytelniejszy jest `std::optional<T>`.

**Ograniczenia:** wariant nie może przechowywać referencji, tablic ani `void`,
a lista alternatyw nie może być pusta. Nigdy nie jest też „pusty”: zawsze trzyma
jakąś wartość (jedyny wyjątek opisuje sekcja 5, pkt 3).

## 3. Najważniejsze narzędzie: `std::visit`

`std::visit(visitor, v)` wywołuje wizytor z wartością aktualnie przechowywaną w `v`.
Wizytorem może być dowolny obiekt wywoływalny: lambda, obiekt z `operator()` albo
zestaw lambd sklejonych idiomem `overloaded` (niżej).

```cpp
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::variant<int, double, std::string> v = 3.14;

    // lambda generyczna: jeden kod dla wszystkich typów
    std::visit([](const auto& x) { std::cout << x << '\n'; }, v);   // 3.14
}
```

Gdy każdy typ wymaga innej obsługi, stosuje się popularny idiom **overloaded**, czyli
sklejenie kilku lambd w jeden obiekt z wieloma `operator()`:

```cpp
#include <iostream>
#include <string>
#include <variant>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;  // od C++20 zbędne

int main() {
    std::variant<int, double, std::string> v = std::string("text");

    std::visit(overloaded{
        [](int i)                { std::cout << "int: "    << i << '\n'; },
        [](double d)             { std::cout << "double: " << d << '\n'; },
        [](const std::string& s) { std::cout << "string: " << s << '\n'; },
    }, v);   // string: text
}
```

`visit` może też zwracać wartość. Wszystkie gałęzie muszą wtedy zwracać dokładnie
ten sam typ: jeśli jedna lambda zwraca `int`, a inna `double`, kompilacja się nie powiedzie
(czasem trzeba dopisać `-> T` przy lambdach).

### Brakujący przypadek to błąd kompilacji

Który typ siedzi w wariancie, wiadomo dopiero w czasie działania programu. `std::visit`
jest jednak szablonem i już przy kompilacji generuje osobne wywołanie wizytora dla
każdej alternatywy; w czasie działania wybiera tylko jedno z nich według `index()`.
Każde z tych wywołań musi się skompilować, także dla typów, których w danej chwili
w wariancie nie ma.

```cpp
#include <iostream>
#include <variant>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Circle    { double r; };
struct Rectangle { double a, b; };
struct Triangle  { double a, h; };

using Shape = std::variant<Circle, Rectangle>;
// using Shape = std::variant<Circle, Rectangle, Triangle>;   // podmień linię wyżej: area() przestanie się kompilować

double area(const Shape& s) {
    return std::visit(overloaded{
        [](const Circle& c)    { return 3.14159 * c.r * c.r; },
        [](const Rectangle& r) { return r.a * r.b; },
    }, s);
}

int main() {
    std::cout << area(Circle{1}) << '\n';         // 3.14159
    std::cout << area(Rectangle{2, 3}) << '\n';   // 6
}
```

Po dopisaniu trzeciego typu, `using Shape = std::variant<Circle, Rectangle, Triangle>;`,
funkcja `area` przestaje się kompilować: `visit` próbuje wywołać wizytor
z `const Triangle&`, a żadna lambda takiego argumentu nie przyjmuje. g++ zgłasza to
m.in. jako `no matching function for call to '__invoke(overloaded<...>, const Triangle&)'`,
wskazując linię z `std::visit`. Jeśli w programie jest dziesięć takich funkcji, błąd
pojawi się w każdej z nich i żadnej nie da się przeoczyć.

Ta sama funkcja napisana przez `get_if` po dodaniu `Triangle` kompiluje się bez
ostrzeżenia i dla trójkąta po cichu zwraca 0:

```cpp
#include <iostream>
#include <variant>

struct Circle    { double r; };
struct Rectangle { double a, b; };
struct Triangle  { double a, h; };

using Shape = std::variant<Circle, Rectangle, Triangle>;   // Triangle już dopisany

double area(const Shape& s) {
    if (auto* c = std::get_if<Circle>(&s))    return 3.14159 * c->r * c->r;
    if (auto* r = std::get_if<Rectangle>(&s)) return r->a * r->b;
    return 0;   // tu trafia Triangle
}

int main() {
    std::cout << area(Rectangle{2, 3}) << '\n';   // 6
    std::cout << area(Triangle{2, 3}) << '\n';    // 0, a powinno być 3
}
```


### Kiedy ta kontrola nie działa

Kompilator sprawdza wyłącznie, czy wizytor da się wywołać z każdą alternatywą, a nie
czy dla każdej napisano osobną gałąź. Dwie sytuacje przepuszczają nowy typ bez błędu:

- **lambda generyczna** `[](const auto& x)` wewnątrz `overloaded` pasuje do każdego
  typu, więc nowa alternatywa trafi do niej;
- **niejawne konwersje**: lambda `[](double d)` przyjmie też `int`, `float`, `char`
  i `bool`. Dla `std::variant<int, double, std::string>` z lambdami tylko dla `double`
  i `std::string` wartość `int` zostanie po cichu zamieniona na `double`.

Na drugi przypadek pomaga dopisanie na końcu `overloaded` lambdy, która zawsze
zatrzymuje kompilację:

```cpp
#include <iostream>
#include <string>
#include <variant>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <class> inline constexpr bool always_false = false;

int main() {
    std::variant<int, double, std::string> v = 3.14;

    std::visit(overloaded{
        [](int i)                { std::cout << "int: "    << i << '\n'; },   // zakomentuj: błąd kompilacji
        [](double d)             { std::cout << "double: " << d << '\n'; },
        [](const std::string& s) { std::cout << "string: " << s << '\n'; },
        [](const auto& x) { static_assert(always_false<decltype(x)>, "unhandled alternative"); },
    }, v);   // double: 3.14
}
```

Po zakomentowaniu lambdy dla `int` program przestaje się kompilować: lambda generyczna
przyjmuje `int` bez żadnej konwersji, więc kompilator woli ją od
`[](double d)` i trafia na `static_assert`. Typy, które mają własną lambdę z dokładnie
pasującym parametrem, nadal idą do niej, bo przy równie dobrym dopasowaniu zwykła
funkcja wygrywa z szablonem. Warunek `always_false<decltype(x)>` zależy od typu `x`,
dlatego asercja sprawdza się dopiero wtedy, gdy kompilator naprawdę wybierze tę lambdę.


## 4. Przykłady do uruchomienia

| Plik | Co pokazuje |
| --- | --- |
| `example_01_podstawy.cpp` | tworzenie, `index`, `holds_alternative`, `get`, `get_if`, `emplace`, wyjątek `bad_variant_access` |
| `example_02_visit.cpp` | `std::visit`: lambda generyczna, idiom `overloaded`, zwracanie wartości |
| `example_03_praktyczne.cpp` | wzorzec „wynik albo błąd” oraz maszyna stanów oparta na typach |
| `example_04_ciekawostki.cpp` | rozmiary w pamięci, duplikaty typów, `valueless_by_exception`, `monostate`, `visit` dwóch wariantów, porównywanie |

Kompilacja z wiersza poleceń:

```
g++ -std=c++17 -Wall -Wextra example_01_podstawy.cpp -o example_01
```

albo przez dołączony `CMakeLists.txt` (np. otwierając katalog w CLionie).
Na końcu każdego pliku, w komentarzu, znajduje się wynik działania programu.

## 5. Ciekawostki dla dociekliwych

1. **Zero alokacji na stercie.** Wariant trzyma wartość we własnej pamięci:
   `sizeof(std::variant<A, B, …>)` ≈ rozmiar największej alternatywy + znacznik indeksu.
   `std::any` dla większych typów alokuje na stercie. Dzięki temu
   `std::vector<std::variant<…>>` leży w pamięci ciasno (przyjaźnie dla cache),

2. **`visit` kontra funkcje wirtualne.** `visit` wybiera kod do wykonania według
   `index()`: przez tablicę wskaźników funkcji albo, przy małej liczbie alternatyw,
   przez zwykły `switch` (libstdc++ robi tak dla jednego wariantu z najwyżej 11
   alternatywami), który kompilator może zinline'ować. Koszt jest porównywalny
   z wywołaniem wirtualnym, a przy `switch` często mniejszy. Obiekty nie noszą też
   w sobie wskaźnika na vtable. `std::visit(f, v1, v2)` przyjmuje też
   **kilka wariantów naraz** i rozprowadza po kombinacji typów (*multi-dispatch*),
   czego funkcje wirtualne wprost nie potrafią.

3. **Jedyny „pusty” stan: `valueless_by_exception`.** Jeśli podczas zmiany typu
   (stara wartość już zniszczona, nowa jeszcze nie powstała) konstruktor rzuci
   wyjątkiem, wariant zostaje „bez wartości”: `valueless_by_exception()` zwraca
   `true`, `index()` zwraca `std::variant_npos`, a `get`/`visit` rzucają
   `bad_variant_access`. Stan jest rzadki i przejściowy: zwykłe przypisanie go
   naprawia (demonstracja w `example_04`).

4. **Duplikaty typów są legalne.** `std::variant<int, int>` się kompiluje;
   alternatywy rozróżnia się wtedy wyłącznie indeksem: `std::get<1>(v)`,
   `std::in_place_index<1>`. `std::get<int>(v)` byłby niejednoznaczny i się nie skompiluje.

5. **Pułapka konwersji (naprawiona).** W pierwotnym C++17
   `std::variant<std::string, bool> v = "text";` wybierał… `bool`, bo konwersja
   wskaźnika na `bool` „wygrywała” z konwersją do `std::string`. W C++20 naprawiły to
   P0608R3 (konstruktor konwertujący pomija konwersje zawężające) i P1957R2
   (konwersja wskaźnika na `bool` liczy się jako zawężająca), więc `"text"` trafia
   teraz do `std::string`. g++ (libstdc++) stosuje obie poprawki także w trybie
   `-std=c++17`.

6. **Porównywanie i haszowanie.** Warianty wspierają `==`, `<` itd. Porównywany
   jest najpierw indeks, potem wartości tego samego typu. Dzięki `<` wariant może być
   kluczem w `std::map`, a dzięki `std::hash` (dostępnemu, gdy mają go wszystkie
   alternatywy) także w `std::unordered_map`.

7. **Metafunkcje.** `std::variant_size_v<V>` podaje liczbę alternatyw,
   a `std::variant_alternative_t<I, V>` to typ o indeksie `I`. Przydatne w kodzie
   szablonowym.

8. **Co dalej w standardzie.** C++23 dodał `std::expected<T, E>`, dedykowany typ
   „wynik albo błąd”. C++26 dodaje `visit` jako metodę: `v.visit(visitor)` (P2637).


## 6. Źródła

- cppreference, `std::variant` i funkcje towarzyszące: https://en.cppreference.com/w/cpp/utility/variant (w tym `std::visit`: https://en.cppreference.com/w/cpp/utility/variant/visit)
- Bartłomiej Filipek (C++ Stories), *Everything You Need to Know About std::variant from C++17*: https://www.cppstories.com/2018/06/variant/
- Microsoft Learn, dokumentacja klasy `variant`: https://learn.microsoft.com/en-us/cpp/standard-library/variant-class
- Propozycje do standardu ISO C++: P0088R3 (dodanie `variant` do C++17), P0608R3 (poprawka konstruktora konwertującego), P1957R2 (konwersja wskaźnika na `bool` jako zawężająca), P2637R3 (`visit` jako metoda, C++26), P2688 (pattern matching)
- Przykłady skompilowano i uruchomiono lokalnie: g++ 16.2 (MSYS2), `-std=c++17 -Wall -Wextra`.
