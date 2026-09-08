#include <Arduino.h>
#include <unity.h>

void setup();
void loop();
void setUp() { fake::reset(); }
void tearDown() { TEST_ASSERT_EQUAL(0, fake::lockDepth); }

void assertPins(int red, int yellow, int green) {
    TEST_ASSERT_EQUAL(red, fake::levels[25]);
    TEST_ASSERT_EQUAL(yellow, fake::levels[26]);
    TEST_ASSERT_EQUAL(green, fake::levels[27]);
}

void test_firmware_start_timeout_blink_and_recovery() {
    setup();
    assertPins(HIGH, HIGH, HIGH);
    TEST_ASSERT_EQUAL(OUTPUT, fake::modes[18]);
    TEST_ASSERT_EQUAL(INPUT, fake::modes[19]);
    loop();
    fake::nowUs = 30000;
    loop();
    fake::nowUs = 250000;
    loop();
    assertPins(LOW, LOW, LOW);
    fake::edge(true, 250100);
    fake::edge(false, 251260);
    loop();
    assertPins(LOW, HIGH, LOW);
    fake::nowUs = 350000;
    loop();
    fake::nowUs = 380000;
    loop();
    assertPins(HIGH, HIGH, HIGH);
    fake::nowUs = 630000;
    loop();
    assertPins(LOW, LOW, LOW);
    fake::edge(true, 630100);
    fake::edge(false, 630390);
    loop();
    assertPins(HIGH, LOW, LOW);
    TEST_ASSERT_FALSE(fake::delayLocked);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_firmware_start_timeout_blink_and_recovery);
    return UNITY_END();
}
