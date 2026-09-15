#include <unity.h>
#include "UltrasonicSensor.h"
#include "LedDriver.h"
void setUp() { fake::reset(); }
void tearDown() { TEST_ASSERT_EQUAL(0, fake::lockDepth); }
void test_conversion_and_no_overwrite() {
    fake::modes[18] = 99;
    fake::modes[19] = 99;
    fake::levels[18] = HIGH;
    UltrasonicSensor sensor(18, 19);
    sensor.begin();
    TEST_ASSERT_EQUAL(OUTPUT, fake::modes[18]);
    TEST_ASSERT_EQUAL(INPUT, fake::modes[19]);
    TEST_ASSERT_EQUAL(LOW, fake::levels[18]);
    TEST_ASSERT_EQUAL(19, fake::interruptPin);
    TEST_ASSERT_EQUAL(CHANGE, fake::interruptMode);
    TEST_ASSERT_NOT_NULL(fake::callback);
    TEST_ASSERT_NOT_NULL(fake::context);
    Reading result{77};
    TEST_ASSERT_FALSE(sensor.takeReading(result));
    TEST_ASSERT_EQUAL_FLOAT(77, *result.distanceCm);
    sensor.update();
    TEST_ASSERT_EQUAL(1, fake::triggerCount);
    TEST_ASSERT_EQUAL_UINT32(10, fake::nowUs);
    TEST_ASSERT_FALSE(fake::delayLocked);
    TEST_ASSERT_EQUAL(LOW, fake::levels[18]);
    fake::edge(true, 100);
    fake::edge(false, 680);
    fake::nowUs = 500000;
    sensor.update();
    TEST_ASSERT_EQUAL(1, fake::triggerCount);
    TEST_ASSERT_TRUE(sensor.takeReading(result));
    TEST_ASSERT_EQUAL_FLOAT(10, *result.distanceCm);
    TEST_ASSERT_FALSE(sensor.takeReading(result));
    TEST_ASSERT_EQUAL_FLOAT(10, *result.distanceCm);
    sensor.update();
    sensor.update();
    TEST_ASSERT_EQUAL(2, fake::triggerCount);
    TEST_ASSERT_EQUAL_UINT32(500000, fake::pulses[1]);
    TEST_ASSERT_GREATER_THAN(0, fake::taskMutexes.size());
    TEST_ASSERT_GREATER_THAN(0, fake::isrMutexes.size());
    portMUX_TYPE* const expectedMutex = fake::taskMutexes.front();
    for (auto* mutex : fake::taskMutexes) TEST_ASSERT_EQUAL_PTR(expectedMutex, mutex);
    for (auto* mutex : fake::isrMutexes) TEST_ASSERT_EQUAL_PTR(expectedMutex, mutex);
}
void test_echo_high_and_period() {
    UltrasonicSensor sensor(18, 19);
    sensor.begin();
    fake::levels[19] = HIGH;
    sensor.update();
    Reading result{77};
    TEST_ASSERT_EQUAL(0, fake::triggerCount);
    TEST_ASSERT_TRUE(sensor.takeReading(result));
    TEST_ASSERT_FALSE(result.distanceCm.has_value());
    fake::levels[19] = LOW;
    fake::nowUs = 99999;
    sensor.update();
    TEST_ASSERT_EQUAL(0, fake::triggerCount);
    fake::nowUs = 100000;
    sensor.update();
    TEST_ASSERT_EQUAL(1, fake::triggerCount);
}
void test_timeout_and_wrap() {
    UltrasonicSensor sensor(18, 19);
    sensor.begin();
    const std::uint32_t start = UINT32_MAX - 1000;
    fake::nowUs = start;
    sensor.update();
    fake::nowUs = start + 29999;
    sensor.update();
    Reading result{77};
    TEST_ASSERT_FALSE(sensor.takeReading(result));
    fake::nowUs = start + 30000;
    sensor.update();
    TEST_ASSERT_TRUE(sensor.takeReading(result));
    TEST_ASSERT_FALSE(result.distanceCm.has_value());
    TEST_ASSERT_FALSE(sensor.takeReading(result));
    fake::nowUs = start + 99999;
    sensor.update();
    TEST_ASSERT_EQUAL(1, fake::triggerCount);
    fake::nowUs = start + 100000;
    sensor.update();
    TEST_ASSERT_EQUAL(2, fake::triggerCount);
}
void test_led_gpio() {
    LedDriver leds(25, 26, 27);
    leds.begin();
    TEST_ASSERT_EQUAL(OUTPUT, fake::modes[25]);
    TEST_ASSERT_EQUAL(OUTPUT, fake::modes[26]);
    TEST_ASSERT_EQUAL(OUTPUT, fake::modes[27]);
    leds.apply({true, false, false});
    TEST_ASSERT_EQUAL(HIGH, fake::levels[25]);
    TEST_ASSERT_EQUAL(LOW, fake::levels[26]);
    TEST_ASSERT_EQUAL(LOW, fake::levels[27]);
    leds.apply({false, true, true});
    TEST_ASSERT_EQUAL(LOW, fake::levels[25]);
    TEST_ASSERT_EQUAL(HIGH, fake::levels[26]);
    TEST_ASSERT_EQUAL(HIGH, fake::levels[27]);
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_conversion_and_no_overwrite);
    RUN_TEST(test_echo_high_and_period);
    RUN_TEST(test_timeout_and_wrap);
    RUN_TEST(test_led_gpio);
    return UNITY_END();
}
