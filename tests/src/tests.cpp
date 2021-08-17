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
#include <tuple>

namespace {

	void case_1(sw::stopwatch timer = sw::stopwatch()) {
		timer.start();

		std::this_thread::sleep_for(100ms);

		auto t1 = timer.start();
		auto t2 = timer.start();

		REQUIRE((t1 > 50ms && t1 < 150ms));
		REQUIRE((t2 < 50ms));
	}

	void case_2(sw::stopwatch timer = sw::stopwatch()) {
		timer.start();

		std::this_thread::sleep_for(100ms);

		timer.pause();

		std::this_thread::sleep_for(100ms);

		auto t1 = timer.start();

		REQUIRE((t1 > 50ms && t1 < 150ms));
	}

	void case_3(sw::stopwatch timer = sw::stopwatch()) {
		auto ret1 = timer.is_paused();

		timer.start();

		auto ret2 = !timer.is_paused();

		timer.pause();

		auto ret3 = timer.is_paused();

		REQUIRE(ret1);
		REQUIRE(ret2);
		REQUIRE(ret3);
	}

	void case_4(sw::stopwatch timer = sw::stopwatch()) {
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

	void case_5(sw::stopwatch timer = sw::stopwatch()) {
		timer.start();

		std::this_thread::sleep_for(100ms);

		auto t1 = timer.get_time();
		auto t2 = timer.get_time();

		REQUIRE((t1 > 50ms && t1 < 150ms));
		REQUIRE((t2 >= t1));
		REQUIRE(((t2 - t1) < 50ms));
	}

	void run_all_with_dirty_stopwatch(sw::stopwatch& timer) {
		case_1(timer);
		case_2(timer);
		case_3(timer);
		case_4(timer);
		case_5(timer);
	}

}

TEST_CASE("start() when idle and running") {
	case_1();
}

TEST_CASE("pause()") {
	case_2();
}

TEST_CASE("is_paused()") {
	case_3();
}

TEST_CASE("reset()") {
	case_4();
}

TEST_CASE("get_time()") {
	case_5();
}

TEST_CASE("Successive method calls") {

	SECTION("Method spam 1") {
		auto timer = sw::stopwatch();

		timer.pause();
		timer.pause();

		run_all_with_dirty_stopwatch(timer);
	}

	SECTION("Method spam 2") {
		auto timer = sw::stopwatch();

		timer.start();
		timer.pause();
		timer.pause();

		run_all_with_dirty_stopwatch(timer);
	}

	SECTION("Method spam 3") {
		auto timer = sw::stopwatch();

		timer.pause();
		timer.start();
		timer.pause();

		run_all_with_dirty_stopwatch(timer);
	}

	SECTION("Method spam 4") {
		auto timer = sw::stopwatch();

		timer.pause();
		timer.start();
		timer.pause();
		timer.pause();

		run_all_with_dirty_stopwatch(timer);
	}
}