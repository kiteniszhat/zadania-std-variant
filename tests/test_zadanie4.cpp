#include <gtest/gtest.h>

#include <string>
#include <variant>
#include <vector>

#include "zadanie4.hpp"

TEST(Zadanie4_Describe, KazdyStan) {
    EXPECT_EQ(describe(Closed{}), "closed");
    EXPECT_EQ(describe(Open{}), "open");
    EXPECT_EQ(describe(Locked{"1234", 0}), "locked (failed: 0)");
    EXPECT_EQ(describe(Locked{"1234", 2}), "locked (failed: 2)");
    EXPECT_EQ(describe(Alarm{}), "alarm");
}

TEST(Zadanie4_NextState, OtwieranieIZamykanie) {
    EXPECT_TRUE(std::holds_alternative<Open>(next_state(Closed{}, OpenDoor{})));
    EXPECT_TRUE(std::holds_alternative<Closed>(next_state(Open{}, CloseDoor{})));
}

TEST(Zadanie4_NextState, ZamykanieNaKod) {
    const DoorState s = next_state(Closed{}, Lock{"1234"});
    const Locked* locked = std::get_if<Locked>(&s);
    ASSERT_NE(locked, nullptr) << "zamknięte drzwi da się zamknąć na kod";
    EXPECT_EQ(locked->code, "1234");
    EXPECT_EQ(locked->failed_attempts, 0);
}

TEST(Zadanie4_NextState, OdblokowanieDobrymKodem) {
    EXPECT_TRUE(std::holds_alternative<Closed>(next_state(Locked{"1234", 0}, Unlock{"1234"})));
    EXPECT_TRUE(std::holds_alternative<Closed>(next_state(Locked{"1234", 2}, Unlock{"1234"})))
        << "dobry kod działa także po wcześniejszych błędach";
}

TEST(Zadanie4_NextState, ZlyKodZwiekszaLicznik) {
    const DoorState s = next_state(Locked{"1234", 0}, Unlock{"0000"});
    const Locked* locked = std::get_if<Locked>(&s);
    ASSERT_NE(locked, nullptr) << "po błędnym kodzie drzwi zostają zamknięte na kod";
    EXPECT_EQ(locked->code, "1234") << "kod się nie zmienia";
    EXPECT_EQ(locked->failed_attempts, 1);
}

TEST(Zadanie4_NextState, TrzeciBladToAlarm) {
    const DoorState s = next_state(Locked{"1234", max_failed_attempts - 1}, Unlock{"0000"});
    EXPECT_TRUE(std::holds_alternative<Alarm>(s)) << "po " << max_failed_attempts << " błędach ma być alarm";
}

TEST(Zadanie4_NextState, NiedozwoloneZdarzeniaIgnorowane) {
    EXPECT_EQ(next_state(Open{}, Lock{"1234"}), DoorState(Open{})) << "otwartych drzwi nie da się zamknąć na kod";
    EXPECT_EQ(next_state(Open{}, OpenDoor{}), DoorState(Open{}));
    EXPECT_EQ(next_state(Closed{}, CloseDoor{}), DoorState(Closed{}));
    EXPECT_EQ(next_state(Closed{}, Unlock{"1234"}), DoorState(Closed{}));
    EXPECT_EQ(next_state(Locked{"1234", 1}, OpenDoor{}), DoorState(Locked{"1234", 1}))
        << "drzwi zamkniętych na kod nie da się otworzyć bez Unlock";
    EXPECT_EQ(next_state(Locked{"1234", 1}, Lock{"9999"}), DoorState(Locked{"1234", 1}));
}

TEST(Zadanie4_NextState, AlarmIgnorujeWszystko) {
    const DoorEvent events[] = {OpenDoor{}, CloseDoor{}, Lock{"1"}, Unlock{"1234"}};
    for (const DoorEvent& e : events) {
        EXPECT_TRUE(std::holds_alternative<Alarm>(next_state(Alarm{}, e))) << "zdarzenie o indeksie " << e.index();
    }
}

TEST(Zadanie4_Door, ZmianaStanuTrafiaDoHistorii) {
    Door door;
    EXPECT_TRUE(door.handle(OpenDoor{}));
    EXPECT_TRUE(door.handle(CloseDoor{}));
    EXPECT_TRUE(door.handle(Lock{"42"}));
    EXPECT_TRUE(door.handle(Unlock{"42"}));
    EXPECT_TRUE(std::holds_alternative<Closed>(door.state()));
    const std::vector<std::string> expected = {"open", "closed", "locked (failed: 0)", "closed"};
    EXPECT_EQ(door.history(), expected);
}

TEST(Zadanie4_Door, ZignorowaneZdarzenieNieTrafiaDoHistorii) {
    Door door;
    EXPECT_FALSE(door.handle(CloseDoor{})) << "drzwi już są zamknięte";
    EXPECT_TRUE(door.handle(OpenDoor{}));
    EXPECT_FALSE(door.handle(Lock{"42"})) << "otwartych drzwi nie da się zamknąć na kod";
    EXPECT_FALSE(door.handle(OpenDoor{}));
    const std::vector<std::string> expected = {"open"};
    EXPECT_EQ(door.history(), expected);
}

TEST(Zadanie4_Door, Wlamanie) {
    Door door;
    door.handle(Lock{"1234"});
    EXPECT_TRUE(door.handle(Unlock{"0000"})) << "błędna próba zmienia licznik, więc zmienia stan";
    EXPECT_TRUE(door.handle(Unlock{"1111"}));
    EXPECT_TRUE(door.handle(Unlock{"2222"}));
    EXPECT_FALSE(door.handle(Unlock{"1234"})) << "alarm ignoruje nawet dobry kod";
    EXPECT_TRUE(std::holds_alternative<Alarm>(door.state()));
    const std::vector<std::string> expected = {"locked (failed: 0)", "locked (failed: 1)", "locked (failed: 2)",
                                               "alarm"};
    EXPECT_EQ(door.history(), expected);
}
