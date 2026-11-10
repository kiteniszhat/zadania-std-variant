#include <gtest/gtest.h>

#include <string>
#include <variant>
#include <vector>

#include "zadanie5.hpp"

namespace {

const Command* command_of(const Result<Command>& r) { return std::get_if<Command>(&r); }

void expect_parse_error(const std::string& line) {
    const Result<Command> r = parse_command(line);
    const Error* e = std::get_if<Error>(&r);
    ASSERT_NE(e, nullptr) << "\"" << line << "\" ma dać Error";
    EXPECT_FALSE(e->message.empty()) << "opis błędu dla \"" << line << "\" nie może być pusty";
}

}

TEST(Zadanie5_ParseCommand, Wplata) {
    const Result<Command> r = parse_command("deposit 100");
    const Command* c = command_of(r);
    ASSERT_NE(c, nullptr) << "poprawna linia ma dać Command, a nie Error";
    const Deposit* d = std::get_if<Deposit>(c);
    ASSERT_NE(d, nullptr) << "polecenie ma być typu Deposit";
    EXPECT_EQ(d->amount, 100);
}

TEST(Zadanie5_ParseCommand, Wyplata) {
    const Result<Command> r = parse_command("withdraw 30");
    const Command* c = command_of(r);
    ASSERT_NE(c, nullptr) << "poprawna linia ma dać Command, a nie Error";
    const Withdraw* w = std::get_if<Withdraw>(c);
    ASSERT_NE(w, nullptr) << "polecenie ma być typu Withdraw";
    EXPECT_EQ(w->amount, 30);
}

TEST(Zadanie5_ParseCommand, Saldo) {
    const Result<Command> r = parse_command("balance");
    const Command* c = command_of(r);
    ASSERT_NE(c, nullptr) << "poprawna linia ma dać Command, a nie Error";
    EXPECT_TRUE(std::holds_alternative<Balance>(*c));
}

TEST(Zadanie5_ParseCommand, DodatkoweSpacje) {
    const Result<Command> r = parse_command("  deposit   25  ");
    const Command* c = command_of(r);
    ASSERT_NE(c, nullptr) << "spacje wokół słów są dozwolone";
    ASSERT_TRUE(std::holds_alternative<Deposit>(*c));
    EXPECT_EQ(std::get<Deposit>(*c).amount, 25);
}

TEST(Zadanie5_ParseCommand, NieznanePolecenie) {
    expect_parse_error("transfer 10");
    expect_parse_error("");
    expect_parse_error("DEPOSIT 10");
}

TEST(Zadanie5_ParseCommand, ZlaKwota) {
    expect_parse_error("deposit");
    expect_parse_error("withdraw abc");
    expect_parse_error("deposit 0");
    expect_parse_error("withdraw -5");
}

TEST(Zadanie5_ParseCommand, NadmiaroweZnaki) {
    expect_parse_error("deposit 10 zl");
    expect_parse_error("deposit 10abc");
    expect_parse_error("balance 5");
}

TEST(Zadanie5_Execute, Wplata) {
    Account acc(50);
    const Result<int> r = acc.execute(Deposit{100});
    ASSERT_TRUE(std::holds_alternative<int>(r)) << "wpłata zawsze się udaje";
    EXPECT_EQ(std::get<int>(r), 150) << "wynik to saldo po operacji";
    EXPECT_EQ(acc.balance(), 150);
}

TEST(Zadanie5_Execute, Wyplata) {
    Account acc(100);
    const Result<int> r = acc.execute(Withdraw{100});
    ASSERT_TRUE(std::holds_alternative<int>(r)) << "można wypłacić całe saldo";
    EXPECT_EQ(std::get<int>(r), 0);
    EXPECT_EQ(acc.balance(), 0);
}

TEST(Zadanie5_Execute, BrakSrodkow) {
    Account acc(30);
    const Result<int> r = acc.execute(Withdraw{31});
    const Error* e = std::get_if<Error>(&r);
    ASSERT_NE(e, nullptr) << "nie da się wypłacić więcej, niż jest na koncie";
    EXPECT_FALSE(e->message.empty());
    EXPECT_EQ(acc.balance(), 30) << "nieudana wypłata nie zmienia salda";
}

TEST(Zadanie5_Execute, Saldo) {
    Account acc(70);
    const Result<int> r = acc.execute(Balance{});
    ASSERT_TRUE(std::holds_alternative<int>(r));
    EXPECT_EQ(std::get<int>(r), 70);
    EXPECT_EQ(acc.balance(), 70);
}

TEST(Zadanie5_Run, PoprawnySkrypt) {
    Account acc;
    const std::size_t done = acc.run({"deposit 100", "withdraw 40", "balance", "deposit 5"});
    EXPECT_EQ(done, 4u);
    EXPECT_EQ(acc.balance(), 65);
    EXPECT_TRUE(acc.errors().empty());
}

TEST(Zadanie5_Run, BledyNiePrzerywajaSkryptu) {
    Account acc;
    const std::size_t done = acc.run({
        "deposit 100",
        "withdraw 500",  // błąd wykonania: brak środków
        "fly 10",        // błąd parsowania: nieznane polecenie
        "withdraw 30",
        "deposit -1",    // błąd parsowania: kwota <= 0
        "balance",
    });
    EXPECT_EQ(done, 3u) << "udały się: deposit 100, withdraw 30, balance";
    EXPECT_EQ(acc.balance(), 70);
    ASSERT_EQ(acc.errors().size(), 3u) << "każdy błąd (parsowania i wykonania) ma trafić do errors()";
    for (const Error& e : acc.errors()) {
        EXPECT_FALSE(e.message.empty());
    }
}

TEST(Zadanie5_Run, PustySkrypt) {
    Account acc(10);
    EXPECT_EQ(acc.run({}), 0u);
    EXPECT_EQ(acc.balance(), 10);
    EXPECT_TRUE(acc.errors().empty());
}
