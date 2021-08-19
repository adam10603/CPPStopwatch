# C++ Stopwatch ⏱️
![Version](https://img.shields.io/badge/Version-1.2.1-blue.svg) ![Tests](https://github.com/adam10603/CPPStopwatch/actions/workflows/tests.yml/badge.svg)

A portable and flexible header-only C++ stopwatch class compatible with [`std::chrono`](https://en.cppreference.com/w/cpp/header/chrono) clocks and types.

Just include [stopwatch.hpp](inc/stopwatch.hpp) in your project and you can use it right away.

You can use it on any platform with a C++17 compiler, as it doesn't rely on any platform-specific APIs.


## [API Reference Page 🔗](Reference.md)


## Purpose


A lot of C++ stopwatch classes I've seen online had a questionable implementation. Many have issues like using non-monotonic clocks ([`chrono::high_resolution_clock`](https://en.cppreference.com/w/cpp/chrono/high_resolution_clock) can be such), having non-standard code, unsafe conversions, [inefficient code generation](https://gfycat.com/YellowFrighteningBellsnake) and more. This is why I wanted to make a more ideal version. It was for myself at first, but I hope others will find it useful as well.

#### Highlights:
  * Simple but flexible API.
  * [Efficient code generation](https://i.imgur.com/HV8uicb.png). Most compilers can optimize the library away entirely.
  * Compatibility with [`chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration) types.
  * A custom `duration_components` type to help with formatted output.
  * Option to use any clock type via a template argument.
    * If you don't need this, `stopwatch` is a ready-to-go typedef that uses [`chrono::steady_clock`](https://en.cppreference.com/w/cpp/chrono/steady_clock).
  * Type-safety and correctness through using [`chrono`](https://en.cppreference.com/w/cpp/header/chrono) utilities (no hand-written arithmetic).


## Examples


### Measuring Execution Time

```cpp
auto timer = sw::stopwatch();

timer.start();

some_work();

auto elapsed = timer.get_elapsed<sw::d_seconds>();

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

    // start() doubles as a "lap" function.
    // It restarts the stopwatch and returns the elapsed time.
    // timer.start<T>() a shorthand for sw::convert_time<T>(timer.start())
    auto elapsed = timer.start<std::chrono::milliseconds>();

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

// timer.get_elapsed<T>() is a shorthand for sw::convert_time<T>(timer.get_elapsed())
auto elapsed = timer.get_elapsed<sw::duration_components>();

std::cout
    << elapsed.hours
    << "h "
    << elapsed.minutes
    << "m "
    << elapsed.seconds
    << "s "
    << elapsed.milliseconds
    << "ms\n";
```

### Using `convert_time()`

```cpp
using namespace std::literals::chrono_literals;

auto starting_time      = 1002ms;

auto components             = sw::convert_time<sw::duration_components>   (starting_time);
auto round_trip             = sw::convert_time<std::chrono::milliseconds> (components);
auto round_trip_double_sec  = sw::convert_time<sw::d_seconds>             (round_trip);

// Prints 1 2
std::cout << components.seconds << ' ' << components.milliseconds << '\n';

// Prints 1002
std::cout << round_trip.count() << '\n';

// Prints 1.002
std::cout << round_trip_double_sec.count() << '\n';
```


## Running Tests


Tests are automated with each code change (see [Actions](https://github.com/adam10603/CPPStopwatch/actions/workflows/c-cpp.yml)), so there's no need to run them yourself.

If you still want to, the tests can be executed either with `make` on Linux, or by building the Visual Studio 2019 solution on Windows. They include `-Werror` and `/WX` respectively, alongside with generous warning levels for correctness. The tests use [Catch2 v2](https://github.com/catchorg/Catch2/tree/v2.x).


## Version history


* v1.0
  * Initial release
* v1.1
  * Added support for negative values in `duration_components` which are now handled correctly in both directions by `convert_time()`.
* v1.1.1
  * Code improvements.
* v1.1.2
  * More code improvements.
* v1.2
  * Renamed the `get_time()` method to `get_elapsed()`. This better reflects that this is a duration.
  * Added `clock` member type to the class, which is an alias for the underlying clock type.
* v1.2.1
  * Removed a restriction in `convert_time()` that wouldn't allow a `chrono::duration` type to be converted to the same type.

_____________________
![MIT Logo](https://upload.wikimedia.org/wikipedia/commons/thumb/0/0c/MIT_logo.svg/32px-MIT_logo.svg.png) Distributed under the [MIT License](LICENSE).