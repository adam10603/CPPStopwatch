#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "stopwatch.hpp"

using namespace std::literals::chrono_literals;



// ========================= Testing internal SFINAE things



static_assert( sw::detail::is_chrono_time_point_v<std::chrono::steady_clock::time_point>);
static_assert(!sw::detail::is_chrono_time_point_v<int>);
static_assert( sw::detail::is_chrono_duration_v<std::chrono::steady_clock::duration>);
static_assert(!sw::detail::is_chrono_duration_v<int>);
static_assert( sw::detail::is_ratio_v<std::ratio<1>>);
static_assert(!sw::detail::is_ratio_v<int>);
static_assert( sw::detail::is_trivial_clock_v<std::chrono::steady_clock>);
// static_assert(!sw::detail::is_trivial_clock_v<int>);



// ========================= Compile-time tests



// Positive nanoseconds
void static_test_1() {
	constexpr auto input_ns = 1ns + 2us + 3ms + 4s + 5min + 6h + 168h;

	constexpr auto test_components = sw::convert_time<sw::duration_components>(input_ns);

	static_assert(test_components.nanoseconds	== 1);
	static_assert(test_components.microseconds	== 2);
	static_assert(test_components.milliseconds	== 3);
	static_assert(test_components.seconds		== 4);
	static_assert(test_components.minutes		== 5);
	static_assert(test_components.hours			== 6);
	static_assert(test_components.days			== 7);

	constexpr auto test_roundtrip_ns = sw::convert_time<std::chrono::nanoseconds>(test_components);

	static_assert(test_roundtrip_ns.count() == input_ns.count());
}

// Negative nanoseconds
void static_test_2() {
	constexpr auto input = -1ns - 2us - 3ms - 4s - 5min - 6h - 168h;

	constexpr auto test_components = sw::convert_time<sw::duration_components>(input);

	static_assert(test_components.nanoseconds	== -1);
	static_assert(test_components.microseconds	== -2);
	static_assert(test_components.milliseconds	== -3);
	static_assert(test_components.seconds		== -4);
	static_assert(test_components.minutes		== -5);
	static_assert(test_components.hours			== -6);
	static_assert(test_components.days			== -7);

	constexpr auto test_roundtrip = sw::convert_time<std::chrono::nanoseconds>(test_components);

	static_assert(test_roundtrip.count() == input.count());
}

// Positive seconds
void static_test_3() {
	constexpr auto input = 1s + 2min + 3h + 96h;

	constexpr auto test_components = sw::convert_time<sw::duration_components>(input);

	static_assert(test_components.nanoseconds	== 0);
	static_assert(test_components.microseconds	== 0);
	static_assert(test_components.milliseconds	== 0);
	static_assert(test_components.seconds		== 1);
	static_assert(test_components.minutes		== 2);
	static_assert(test_components.hours			== 3);
	static_assert(test_components.days			== 4);

	constexpr auto test_roundtrip = sw::convert_time<std::chrono::seconds>(test_components);

	static_assert(test_roundtrip.count() == input.count());
}

// Negative seconds
void static_test_4() {
	constexpr auto input = - 1s - 2min - 3h - 96h;

	constexpr auto test_components = sw::convert_time<sw::duration_components>(input);

	static_assert(test_components.nanoseconds	==  0);
	static_assert(test_components.microseconds	==  0);
	static_assert(test_components.milliseconds	==  0);
	static_assert(test_components.seconds		== -1);
	static_assert(test_components.minutes		== -2);
	static_assert(test_components.hours			== -3);
	static_assert(test_components.days			== -4);

	constexpr auto test_roundtrip = sw::convert_time<std::chrono::seconds>(test_components);

	static_assert(test_roundtrip.count() == input.count());
}

// chrono::duration conversions
void static_test_5() {
	static_assert(sw::convert_time<sw::d_nanoseconds>(1ns)		== std::chrono::duration_cast<sw::d_nanoseconds>(1ns));
	static_assert(sw::convert_time<sw::d_nanoseconds>(-1ns)		== std::chrono::duration_cast<sw::d_nanoseconds>(-1ns));
	static_assert(sw::convert_time<sw::d_seconds>(1s)			== std::chrono::duration_cast<sw::d_seconds>(1s));
	static_assert(sw::convert_time<sw::d_seconds>(-1s)			== std::chrono::duration_cast<sw::d_seconds>(-1s));
}



// ========================= Test cases



#include <thread>

TEST_CASE("start() when idle and running + multiple start() calls") {
	auto timer = sw::stopwatch();

	timer.start();

	std::this_thread::sleep_for(100ms);

	auto t1 = timer.start();
	auto t2 = timer.start();

	REQUIRE((t1 > 50ms && t1 < 150ms));
	REQUIRE((t2 < 50ms));
}

TEST_CASE("pause() + start() when paused + multiple start() calls") {
	auto timer = sw::stopwatch();

	timer.start();

	std::this_thread::sleep_for(100ms);

	timer.pause();

	std::this_thread::sleep_for(100ms);

	auto t1 = timer.start();

	std::this_thread::sleep_for(100ms);

	auto t2 = timer.start();

	REQUIRE((t1 > 50ms && t1 < 150ms));
	REQUIRE((t2 > 150ms && t2 < 250ms));
}

TEST_CASE("is_paused()") {
	auto timer = sw::stopwatch();

	auto ret1 = timer.is_paused();

	timer.start();

	auto ret2 = !timer.is_paused();

	timer.pause();

	auto ret3 = timer.is_paused();

	REQUIRE(ret1);
	REQUIRE(ret2);
	REQUIRE(ret3);
}

TEST_CASE("reset()") {
	auto timer = sw::stopwatch();

	auto t1 = timer.start();

	std::this_thread::sleep_for(100ms);

	timer.reset();
	auto t2 = timer.start();

	std::this_thread::sleep_for(100ms);

	timer.pause();
	timer.reset();
	auto t3 = timer.start();

	std::this_thread::sleep_for(100ms);

	auto t4 = timer.start();

	REQUIRE((t1 >= t2));
	REQUIRE((t2 == t3));
	REQUIRE((t4 > 50ms && t4 < 150ms));
}

TEST_CASE("get_elapsed()") {
	auto timer = sw::stopwatch();

	timer.start();

	std::this_thread::sleep_for(100ms);

	auto t1 = timer.get_elapsed();
	auto t2 = timer.get_elapsed();

	REQUIRE((t1 > 50ms && t1 < 150ms));
	REQUIRE((t2 >= t1));
	REQUIRE(((t2 - t1) < 50ms));
}

TEST_CASE("Multiple pause() calls") {
	auto timer = sw::stopwatch();

	timer.start();

	std::this_thread::sleep_for(100ms);

	auto t1 = timer.get_elapsed();

	timer.pause();

	std::this_thread::sleep_for(100ms);

	auto t2 = timer.get_elapsed();

	timer.pause();

	std::this_thread::sleep_for(100ms);

	auto t3 = timer.get_elapsed();

	timer.start();

	std::this_thread::sleep_for(100ms);

	auto t4 = timer.get_elapsed();

	REQUIRE((t1 > 50ms && t1 < 150ms));
	REQUIRE((t2 > 50ms && t2 < 150ms));
	REQUIRE((t3 == t2));
	REQUIRE((t4 > t3));
}