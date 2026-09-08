#include <unity.h>
#include "EchoCapture.h"
void setUp() {}
void tearDown() {}
void test_ordered_pulse_and_consumption() {
    EchoCapture capture;
    std::optional<std::uint32_t> result = 77;
    TEST_ASSERT_FALSE(capture.takePulse(result));
    TEST_ASSERT_EQUAL_UINT32(77, *result);
    capture.onEdge(true, 10);
    capture.start(100);
    capture.onEdge(false, 150);
    capture.onEdge(true, 200);
    capture.onEdge(false, 780);
    capture.expire(100000);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_EQUAL_UINT32(580, *result);
    TEST_ASSERT_FALSE(capture.takePulse(result));
    TEST_ASSERT_EQUAL_UINT32(580, *result);
}
void test_timeout_boundaries() {
    for (std::uint32_t end : {29999u, 30000u, 30001u}) {
        EchoCapture capture;
        capture.start(100);
        capture.onEdge(true, 200);
        capture.onEdge(false, 100 + end);
        std::optional<std::uint32_t> result;
        TEST_ASSERT_TRUE(capture.takePulse(result));
        TEST_ASSERT_EQUAL(end < 30000, result.has_value());
    }
}
void test_partial_missing_late_and_old_edges() {
    EchoCapture capture;
    std::optional<std::uint32_t> result = 77;
    capture.start(1000);
    capture.onEdge(true, 999);
    capture.onEdge(false, 1100);
    capture.expire(30999);
    TEST_ASSERT_FALSE(capture.takePulse(result));
    capture.expire(31000);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_FALSE(result.has_value());
    capture.onEdge(true, 32000);
    capture.onEdge(false, 33000);
    TEST_ASSERT_FALSE(capture.takePulse(result));
    capture.start(100000);
    capture.onEdge(true, 100100);
    capture.onEdge(false, 100050);
    capture.expire(130000);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_FALSE(result.has_value());
}
void test_pending_protection_and_wrap() {
    EchoCapture capture;
    const std::uint32_t start = UINT32_MAX - 100;
    capture.start(start);
    capture.onEdge(true, start + 50);
    capture.onEdge(false, start + 630);
    capture.start(900);
    std::optional<std::uint32_t> result;
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_EQUAL_UINT32(580, *result);
    capture.start(start);
    capture.onEdge(true, start + 1);
    capture.expire(start + 30000);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_FALSE(result.has_value());
}
void test_rise_at_deadline_and_duplicate_edges() {
    EchoCapture capture;
    std::optional<std::uint32_t> result;
    capture.start(100);
    capture.onEdge(true, 30100);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_FALSE(result.has_value());
    capture.start(100000);
    capture.onEdge(true, 100100);
    capture.onEdge(true, 100200);
    capture.onEdge(false, 100100);
    TEST_ASSERT_FALSE(capture.takePulse(result));
    capture.onEdge(false, 100680);
    TEST_ASSERT_TRUE(capture.takePulse(result));
    TEST_ASSERT_EQUAL_UINT32(580, *result);
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ordered_pulse_and_consumption);
    RUN_TEST(test_timeout_boundaries);
    RUN_TEST(test_partial_missing_late_and_old_edges);
    RUN_TEST(test_pending_protection_and_wrap);
    RUN_TEST(test_rise_at_deadline_and_duplicate_edges);
    return UNITY_END();
}
