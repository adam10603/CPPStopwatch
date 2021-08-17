# C++ Stopwatch ⏱️
![Version](https://img.shields.io/badge/Version-1.1.2-blue.svg) ![Tests](https://github.com/adam10603/CPPStopwatch/actions/workflows/c-cpp.yml/badge.svg)

A portable and flexible header-only C++17 stopwatch class compatible with [`std::chrono`](https://en.cppreference.com/w/cpp/header/chrono) clocks and types.

It doesn't rely on platform-specific functionality, so it will work on any platform with a C++17 compiler.

The stopwatch class itself is a template class that can use any underlying clock, as long as it's a compatible clock type. For convenience, there's a type definition that uses [`std::chrono::steady_clock`](https://en.cppreference.com/w/cpp/chrono/steady_clock), which should be all you need.


## [API Reference Page 🔗](Reference.md)


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
    // timer.start<T>() a shorthand for sw::convert_time<T>(timer.start())
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

// timer.get_time<T>() is a shorthand for sw::convert_time<T>(timer.get_time())
auto elapsed = timer.get_time<sw::duration_components>();

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


## Running Tests


Tests are automatically executed upon every commit, so there's no need to run them yourself.

If you still want to, the tests can be executed either with `make` on Linux, or by building the Visual Studio 2019 solution on Windows. They include `-Werror` and `/WX` respectively, alongside with generous warning levels for the sake of correctness. The tests use [Catch2 v2](https://github.com/catchorg/Catch2/tree/v2.x).


## Version history


* v1.0
  * Initial release
* v1.1
  * Added support for negative values in `duration_components` which are now handled correctly in both directions by `convert_time()`.
* v1.1.1
  * Code improvements.
* v1.1.2
  * More code improvements.

_____________________
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png) Licensed under WTFPL v2 (see [COPYING](COPYING)).