#ifndef _A_STOPWATCH_HPP_
#define _A_STOPWATCH_HPP_

#include <type_traits>
#include <chrono>
#include <ratio>

namespace sw {

	using d_seconds			= std::chrono::duration<double, std::ratio<1>>;
	using d_milliseconds	= std::chrono::duration<double, std::milli>;
	using d_microseconds	= std::chrono::duration<double, std::micro>;
	using d_nanoseconds		= std::chrono::duration<double, std::nano>;

	struct duration_components {
		int days{};
		int hours{};
		int minutes{};
		int seconds{};
		int milliseconds{};
		int microseconds{};
		int nanoseconds{};
	};

	namespace _internal {

		// ================ Start of SFINAE hell

		template <class T, template <class...> class Template>
		struct is_template_instance_of : std::false_type {};

		template <template <class...> class Template, class... Args>
		struct is_template_instance_of<Template<Args...>, Template> : std::true_type {};

		template <class T, template <class...> class Template>
		inline constexpr bool is_template_instance_of_v = false;

		template <template <class...> class Template, class... Args>
		inline constexpr bool is_template_instance_of_v<Template<Args...>, Template> = true;

		template <typename T>
		struct is_trivial_clock :
			std::conditional_t<
			std::is_arithmetic_v<typename T::rep>&&
			std::is_integral_v<decltype(T::period::num)>&&
			std::is_integral_v<decltype(T::period::den)>&&
			std::is_same_v<typename T::duration, std::chrono::duration<typename T::rep, typename T::period>>&&
			is_template_instance_of_v<typename T::time_point, std::chrono::time_point>&&
			std::is_same_v<decltype(T::is_steady), const bool>&&
			std::is_nothrow_invocable_r_v<typename T::time_point, decltype(T::now)>,
			std::true_type,
			std::false_type>
		{};

		template <typename T>
		inline constexpr bool is_trivial_clock_v = is_trivial_clock<T>::value;

		template <typename T>
		struct is_chrono_duration : std::false_type {};

		template <typename Rep, typename Period>
		struct is_chrono_duration<std::chrono::duration<Rep, Period>> : std::true_type {};
		
		template <typename T>
		inline constexpr bool is_chrono_duration_v = is_chrono_duration<T>::value;

		// ================ End of SFINAE hell

		template <typename ExtractDuration, typename InputDuration, typename Integer>
		constexpr InputDuration extract_unit(Integer& dst, InputDuration t) noexcept {
			auto extracted = std::chrono::floor<ExtractDuration>(t);
			t -= std::chrono::duration_cast<InputDuration>(extracted);
			dst = static_cast<std::remove_reference_t<decltype(dst)>>(extracted.count());
			return t;
		}

		using chrono_days = std::chrono::duration<int, std::ratio<86400>>; // Why doesn't this exist?

	}

	template <typename ToDuration, typename Rep, typename Period>
	[[nodiscard]] constexpr ToDuration convert_time(std::chrono::duration<Rep, Period> t) {
		if constexpr (std::is_same_v<ToDuration, duration_components>) {
			duration_components ret{};

			t = _internal::extract_unit<_internal::chrono_days>		(ret.days, t);
			t = _internal::extract_unit<std::chrono::hours>			(ret.hours, t);
			t = _internal::extract_unit<std::chrono::minutes>		(ret.minutes, t);
			t = _internal::extract_unit<std::chrono::seconds>		(ret.seconds, t);
			t = _internal::extract_unit<std::chrono::milliseconds>	(ret.milliseconds, t);
			t = _internal::extract_unit<std::chrono::microseconds>	(ret.microseconds, t);
			ret.nanoseconds = static_cast<int>(t.count());

			return ret;
		} else {
			static_assert(_internal::is_chrono_duration_v<ToDuration>, "Invalid duration type");
			static_assert(!std::is_same_v<ToDuration, std::chrono::duration<Rep, Period>>, "A conversion to the same type is redundant");

			return std::chrono::duration_cast<ToDuration>(t);
		}
	}

	template <typename ToDuration>
	[[nodiscard]] constexpr ToDuration convert_time(const duration_components& t) {
		static_assert(!std::is_same_v<ToDuration, duration_components>, "A conversion to the same type is redundant");
		static_assert(_internal::is_chrono_duration_v<ToDuration>, "Invalid duration type");

		auto ret = ToDuration();

		ret += std::chrono::duration_cast<ToDuration>(_internal::chrono_days(t.days));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::hours(t.hours));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::minutes(t.minutes));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::seconds(t.seconds));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::milliseconds(t.milliseconds));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::microseconds(t.microseconds));
		ret += std::chrono::duration_cast<ToDuration>(std::chrono::nanoseconds(t.nanoseconds));

		return ret;
	}


	template <typename MonotonicTrivialClock>
	class basic_stopwatch {
		using Clock = MonotonicTrivialClock;

		static_assert(Clock::is_steady, "Only monotonic clocks can be used");
		static_assert(_internal::is_trivial_clock_v<Clock>, "Clock must satisfy the requirements of TrivialClock");

		typename Clock::time_point m_start{}, m_pause_start{};

		static constexpr typename Clock::time_point zero_time_point = typename Clock::time_point();

		static bool has_value(const typename Clock::time_point& t) noexcept {
			return t != zero_time_point;
		}

		static constexpr auto get_time_impl(const typename Clock::time_point& now, const typename Clock::time_point& start, const typename Clock::time_point& pause_start) noexcept {
			if (has_value(pause_start)) {
				return pause_start - start;
			}

			if (has_value(start)) {
				return now - start;
			}

			return typename Clock::duration();
		}

	public:

		auto start() noexcept {
			const auto now		= Clock::now();
			const auto snapshot	= get_time_impl(now, m_start, m_pause_start);

			if (has_value(m_pause_start)) {
				m_start += now - m_pause_start;
				m_pause_start = zero_time_point;
			} else {
				m_start = now;
			}

			return snapshot;
		}

		template <typename Duration>
		auto start() noexcept {
			return convert_time<Duration>(start());
		}

		void pause() noexcept {
			if (!has_value(m_pause_start) && has_value(m_start)) {
				m_pause_start = Clock::now();
			}
		}

		void reset() noexcept {
			m_start			= zero_time_point;
			m_pause_start	= zero_time_point;
		}

		[[nodiscard]] auto is_paused() const noexcept {
			return has_value(m_pause_start);
		}

		[[nodiscard]] auto get_time() const noexcept {
			return get_time_impl(Clock::now(), m_start, m_pause_start);
		}

		template <typename Duration>
		[[nodiscard]] auto get_time() const noexcept {
			return convert_time<Duration>(get_time_impl(Clock::now(), m_start, m_pause_start));
		}
	};

	using stopwatch = basic_stopwatch<std::chrono::steady_clock>;
}

#endif