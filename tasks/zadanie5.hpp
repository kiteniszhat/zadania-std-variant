#pragma once

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// Zadanie 5: „wynik albo błąd” i kolejka poleceń - prosty bankomat
// Dlaczego praktyczne: funkcja zwraca wartość albo opis błędu (jak std::expected z C++23) i wołający
// musi jawnie sprawdzić, co dostał. Polecenia różnych rodzajów przechodzą przez jedną kolejkę jako wariant

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Error {
    std::string message;
};
template <typename T>
using Result = std::variant<T, Error>;

struct Deposit {
    int amount;
};
struct Withdraw {
    int amount;
};
struct Balance {};

using Command = std::variant<Deposit, Withdraw, Balance>;

// TODO 1: zamień linię tekstu na polecenie:
//           "deposit 100" -> Deposit{100},   "withdraw 30" -> Withdraw{30},   "balance" -> Balance{}
//         Błąd (Error z niepustym opisem): nieznane polecenie, brak kwoty, kwota niebędąca liczbą,
//         kwota <= 0, nadmiarowe znaki po poleceniu ("balance 5", "deposit 10 zl")
inline Result<Command> parse_command(const std::string& line) {
    throw std::logic_error("parse_command: nie zaimplementowano");
}

class Account {
public:
    explicit Account(int balance = 0) : balance_(balance) {}

    // TODO 2: wykonaj polecenie i zwróć saldo PO nim:
    //           Deposit  -> saldo rośnie o amount
    //           Withdraw -> saldo maleje o amount; gdy amount > saldo -> Error, a saldo bez zmian
    //           Balance  -> saldo bez zmian
    Result<int> execute(const Command& command) {
        throw std::logic_error("execute: nie zaimplementowano");
    }

    // TODO 3: wykonaj skrypt linia po linii: parse_command, a gdy się uda - execute.
    //         Każdy błąd (parsowania albo wykonania) dopisz do errors_ i przejdź do następnej linii.
    //         Zwróć liczbę poleceń wykonanych z powodzeniem
    std::size_t run(const std::vector<std::string>& script) {
        throw std::logic_error("run: nie zaimplementowano");
    }

    int balance() const { return balance_; }
    const std::vector<Error>& errors() const { return errors_; }

private:
    int balance_;
    std::vector<Error> errors_;
};
