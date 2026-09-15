#include <unity.h>
#include <limits>
#include "DistanceIndicator.h"
void setUp() {}
void tearDown() {}
void assertLights(LedOutput output, bool r, bool y, bool g) {
    TEST_ASSERT_EQUAL(r, output.red);
    TEST_ASSERT_EQUAL(y, output.yellow);
    TEST_ASSERT_EQUAL(g, output.green);
}
void test_boundaries_and_exclusion() {
    DistanceIndicator indicator;
    const float distances[] = {2, 9.99f, 10, 29.99f, 30, 400, 2};
    for (unsigned i = 0; i < 7; ++i) {
        indicator.accept(distances[i], 0);
        assertLights(indicator.output(0), i < 2 || i == 6, i == 2 || i == 3, i == 4 || i == 5);
    }
}
void test_invalid_values() {
    const std::optional<float> values[] = {std::nullopt, -1, 1.99f, 400.01f,
        std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity()};
    for (auto value : values) {
        DistanceIndicator indicator;
        indicator.accept(20, 0);
        indicator.accept(value, 100);
        assertLights(indicator.output(100), false, true, false);
        indicator.accept(value, 200);
        assertLights(indicator.output(200), true, true, true);
        assertLights(indicator.output(450), false, false, false);
    }
}
void test_phase_and_repeated_invalid() {
    DistanceIndicator indicator;
    assertLights(indicator.output(0), true, true, true);
    indicator.accept(std::nullopt, 100);
    indicator.accept(std::nullopt, 200);
    assertLights(indicator.output(249), true, true, true);
    assertLights(indicator.output(250), false, false, false);
    indicator.accept(std::nullopt, 300);
    assertLights(indicator.output(499), false, false, false);
    assertLights(indicator.output(500), true, true, true);
    assertLights(indicator.output(1750), false, false, false);
    assertLights(indicator.output(2250), false, false, false);
}
void test_recovery_and_new_error() {
    DistanceIndicator indicator;
    indicator.output(250);
    indicator.accept(20, 270);
    assertLights(indicator.output(270), false, true, false);
    indicator.accept(std::nullopt, 290);
    assertLights(indicator.output(290), false, true, false);
    indicator.accept(std::nullopt, 390);
    assertLights(indicator.output(390), true, true, true);
    assertLights(indicator.output(639), true, true, true);
    assertLights(indicator.output(640), false, false, false);
}
void test_wrap() {
    const std::uint32_t start = UINT32_MAX - 100;
    DistanceIndicator indicator(start);
    assertLights(indicator.output(start + 249), true, true, true);
    assertLights(indicator.output(start + 250), false, false, false);
    assertLights(indicator.output(start + 500), true, true, true);
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_boundaries_and_exclusion);
    RUN_TEST(test_invalid_values);
    RUN_TEST(test_phase_and_repeated_invalid);
    RUN_TEST(test_recovery_and_new_error);
    RUN_TEST(test_wrap);
    return UNITY_END();
}
