# C++ Stopwatch
![Version](https://img.shields.io/badge/Version-1.0-green.svg) ![License](https://img.shields.io/badge/License-WTFPL%20v2-blue.svg)

A portable and flexible C++17 stopwatch class compatible with `std::chrono`.

It doesn't rely on platform-specific functionality, and it will work on any platform with a C++17 compiler.

The stopwatch class itself is a template class that can use any clock source of your choosing. For convenience however, there's a specialization that uses `std::chrono::steady_clock`, which should be all you need.

The code has been tested on GCC and Clang with `-Wall -Wpedantic -Wextra`, as well as Visual Studio 2019 with `/Wall` for the sake of correctness.


## Examples


### Measuring Execution Time

```cpp
auto timer = sw::stopwatch();

timer.start();

some_work();

auto elapsed = timer.get_time<sw::d_seconds>();

std::cout
    << "Elapsed time: "
    << elapsed.count()
    << " seconds\n";
```

### "Lap" Timing

```cpp
auto timer = sw::stopwatch();

timer.start();

for (int i{}; i < 5; i++) {
    some_work(i);

    // start() restarts the stopwatch and returns the elapsed time.
    // The template type determines what unit of time it returns.
    // The templated version is just a shorthand for sw::convert_time<T>(timer.start())
    auto elapsed = timer.start<sw::d_milliseconds>();

    std::cout
        << "Iteration "
        << i
        << " : "
        << elapsed.count()
        << " ms\n";
}
```

### Getting Time Components

```cpp
auto timer = sw::stopwatch();

timer.start();

some_work();

// The templated version is a shorthand for sw::convert_time<T>(timer.get_time())
auto elapsed = timer.get_time<sw::duration_components>();

std::cout
    << elapsed.hours
    << "h "
    << elapsed.minutes
    << "m "
    << elapsed.seconds
    << "s "
    << elapsed.milliseconds
    << "ms ";
```

### Using `convert_time`

```cpp
using namespace std::literals::chrono_literals;

auto starting_time      = 1002ms;

auto components         = sw::convert_time<sw::duration_components>(starting_time);
auto round_trip         = sw::convert_time<std::chrono::milliseconds>(components);
auto round_trip_d_sec   = sw::convert_time<sw::d_seconds>(round_trip);

// Prints 1 2
std::cout << components.seconds << ' ' << components.milliseconds << '\n';

// Prints 1002
std::cout << round_trip.count() << '\n';

// Prints 1.002
std::cout << round_trip_d_sec.count() << '\n';
```


## Reference


### Standalone Types and Functions
```cpp
using d_seconds         = /* ... */;
using d_milliseconds    = /* ... */;
using d_microseconds    = /* ... */;
using d_nanoseconds     = /* ... */;
```
> These are handy type definitions for the units of time indicated by the names. They are all specializations of `std::chrono::duration` with `double` as the representation type.
> 
> A stopwatch object can return the time as one of these types (among others).

___
```cpp
struct duration_components {
    int days;           // [0 - (depends on the clock)]
    int hours;          // [0 - 23]
    int minutes;        // [0 - 59]
    int seconds;        // [0 - 59]
    int milliseconds;   // [0 - 999]
    int microseconds;   // [0 - 999]
    int nanoseconds;    // [0 - 999]
};
```
> This represents a time interval broken down into its components. This is useful for printing time in a formatted manner for example.
> 
> A stopwatch object can return the time as this type (among others).

___
```cpp
template <typename To, typename From>
[[nodiscard]] constexpr To convert_time(From t);
```
> This is for converting between time duration types. There are three ways of using it:
> 
> (1) `To` is `duration_components` and `From` is a version of `std::chrono::duration` (such as the time from a stopwatch object).
> 
> (2) `To` is a version of `std::chrono::duration` and `From` is `duration_components`.
> 
> (3) Both `To` and `From` are versions of `std::chrono::duration` with different periods, such as seconds and milliseconds.
> 
> In all three cases the expected conversion takes place. (3) is equivalent to calling `std::chrono::duration_cast<To>(t)`.
> 
> *Note: the implementation looks different from the declaration here, but the resulting interface is functionally the same.*

### The Stopwatch Class
```cpp
template <typename MonotonicTrivialClock>
class basic_stopwatch;

using stopwatch = basic_stopwatch<std::chrono::steady_clock>;
```
> `basic_stopwatch` is a template class that can be instantiated to use any clock source of your choosing.
> 
> `MonotonicTrivialClock` has to be a clock type that's monotonic and satisfies [*TrivialClock*](https://en.cppreference.com/w/cpp/named_req/TrivialClock) requirements. Monotonic means it's non-decreasing, indicated by its `is_steady` member.
> 
> `stopwatch` is a specialization that uses `std::chrono::steady_clock`. **For all intents and purposes `stopwatch` is what you'll want to use**, unless you have specific needs regarding the underlying clock.

___
```cpp
/* constructor */ = default;
```
> The stopwatch class has no constructor defined. After creating an instance, you have to start it by calling its `start()` method.

___
```cpp
MonotonicTrivialClock::duration start();

template <typename Duration>
Duration start();
```
> Starts or restarts the stopwatch, and returns the elapsed time (taken before the restart).
> 
> If the stopwatch is already running, it resets it to 0 and restarts it. This works as a "lap" function since it returns the elapsed time at the point of resetting.
> 
> If the stopwatch is paused, it resumes it.
> 
> The templated version returns the time as `Duration`, which can be `duration_components` or a version of `std::chrono::duration`. The non-template version uses the clock's own duration type.
> 
> The templated version is a shorthand for `sw::convert_time<Duration>(MySW.start())`.

___
```cpp
void pause();
```
> Pauses the stopwatch. The elapsed time will be frozen until the stopwatch is resumed or reset.

___
```cpp
void reset();
```
> Resets the stopwatch. Regardless of its current state, the state after resetting will be the same as a newly constructed instance.

___
```cpp
[[nodiscard]] bool is_paused();
```
> Indicates if the stopwatch is paused. While paused, the elapsed time is frozen until the stopwatch is resumed or reset.

___
```cpp
[[nodiscard]] MonotonicTrivialClock::duration get_time();

template <typename Duration>
[[nodiscard]] Duration get_time();
```
> Returns the elapsed time.
> 
> The templated version returns the time as `Duration`, which can be `duration_components` or a version of `std::chrono::duration`. The non-template version uses the clock's own duration type.
> 
> The templated version is a shorthand for `sw::convert_time<Duration>(MySW.get_time())`.


## Version history


* v1.0
  * Initial release

_____________________
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png) Licensed under WTFPL v2 (see [COPYING](COPYING)).