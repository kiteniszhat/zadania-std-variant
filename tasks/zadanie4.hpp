#pragma once

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// Zadanie 4: maszyna stanów - zamek szyfrowy w drzwiach
// Dlaczego praktyczne: każdy stan to osobny typ z tylko swoimi danymi (kod i licznik prób istnieją
// tylko w stanie Locked), więc nie da się np. odczytać kodu drzwi, które są otwarte

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// Stany. operator== jest potrzebny do porównywania całych wariantów
struct Closed {};
struct Open {};
struct Locked {
    std::string code;
    int failed_attempts = 0;
};
struct Alarm {};

inline bool operator==(Closed, Closed) { return true; }
inline bool operator==(Open, Open) { return true; }
inline bool operator==(Alarm, Alarm) { return true; }
inline bool operator==(const Locked& a, const Locked& b) {
    return a.code == b.code && a.failed_attempts == b.failed_attempts;
}

using DoorState = std::variant<Closed, Open, Locked, Alarm>;

// Zdarzenia
struct OpenDoor {};
struct CloseDoor {};
struct Lock {
    std::string code;
};
struct Unlock {
    std::string code;
};

using DoorEvent = std::variant<OpenDoor, CloseDoor, Lock, Unlock>;

inline constexpr int max_failed_attempts = 3;

// TODO 1: opis stanu:  "closed",   "open",   "locked (failed: N)",   "alarm"
//         describe(Locked{"1234", 2}) == "locked (failed: 2)"
inline std::string describe(const DoorState& state) {
    return "";
}

// TODO 2: nowy stan po zdarzeniu. Przejścia:
//           Closed + OpenDoor   -> Open
//           Open   + CloseDoor  -> Closed
//           Closed + Lock{c}    -> Locked{c, 0}
//           Locked + Unlock{c}  -> c zgodny z kodem: Closed;
//                                  błędny: failed_attempts + 1, a po max_failed_attempts błędach -> Alarm
//           każda inna para     -> stan bez zmian (np. Lock przy otwartych drzwiach, cokolwiek przy Alarm)
inline DoorState next_state(const DoorState& state, const DoorEvent& event) {
    throw std::logic_error("next_state: nie zaimplementowano");
}

class Door {
public:
    // TODO 3: obsłuż zdarzenie przez next_state. Jeśli stan się zmienił (porównaj całe warianty przez ==),
    //         zapisz go, dopisz describe(nowy stan) do history_ i zwróć true; zdarzenie zignorowane -> false
    //         Błędna próba otwarcia zamka też zmienia stan (inny failed_attempts)
    bool handle(const DoorEvent& event) {
        throw std::logic_error("handle: nie zaimplementowano");
    }

    const DoorState& state() const { return state_; }
    const std::vector<std::string>& history() const { return history_; }

private:
    DoorState state_ = Closed{};
    std::vector<std::string> history_;
};
