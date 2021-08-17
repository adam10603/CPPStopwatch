## C++ Stopwatch API Reference


Note that everything lives inside the `sw` namespace, but the namespace is being omitted here for simplicity.

#### Table of contents:
  * [Standalone Types and Functions](#standalone-types-and-functions)
    * [`std::chrono::duration` types](#stdchronoduration-types)
    * [`duration_components` struct](#duration_components-struct)
    * [`convert_time()` utility](#convert_time-utility)
  * [The Stopwatch Class](#the-stopwatch-class)
    * [Constructor](#constructor)
    * [`start()` method](#start-method)
    * [`pause()` method](#pause-method)
    * [`reset()` method](#reset-method)
    * [`is_paused()` method](#is_paused-method)
    * [`get_time()` method](#get_time-method)


### Standalone Types and Functions

#### `std::chrono::duration` types
```cpp
using d_seconds         = /* ... */;
using d_milliseconds    = /* ... */;
using d_microseconds    = /* ... */;
using d_nanoseconds     = /* ... */;
```
These are handy type definitions for the units of time indicated by the names. They are all specializations of `std::chrono::duration` with `double` as the representation type.

A stopwatch object can return the time as one of these types (among others).
___

#### `duration_components` struct
```cpp
struct duration_components {
    int days;           // [(depends on the clock)]
    int hours;          // [-23, 23]
    int minutes;        // [-59, 59]
    int seconds;        // [-59, 59]
    int milliseconds;   // [-999, 999]
    int microseconds;   // [-999, 999]
    int nanoseconds;    // [-999, 999]
};
```
This represents a time interval broken down into its components. This is useful for printing time in a formatted manner for example.

A stopwatch object can return the time as this type (among others).
___

#### `convert_time()` utility
```cpp
template <typename To, typename From>
[[nodiscard]] constexpr To convert_time(From t);
```
This is for converting between time duration types. There are three ways of using it:

 * `To` is [`duration_components`](#duration_components-struct) and `From` is a version of `std::chrono::duration` (such as the time from a stopwatch object).

 * `To` is a version of `std::chrono::duration` and `From` is [`duration_components`](#duration_components-struct).

 * Both `To` and `From` are versions of `std::chrono::duration` with different periods, such as seconds and milliseconds.

In all three cases the expected conversion takes place.

The last use case is equivalent to calling `std::chrono::duration_cast()`.

*Note: the implementation looks different from the declaration here, but the resulting interface is functionally the same.*
___


### The Stopwatch Class

#### `basic_stopwatch` and `stopwatch` classes
```cpp
template <typename MonotonicTrivialClock>
class basic_stopwatch;

using stopwatch = basic_stopwatch<std::chrono::steady_clock>;
```
`basic_stopwatch` is a template class that can be instantiated to use any clock source of your choosing.

`MonotonicTrivialClock` has to be a clock type that's monotonic and satisfies [*TrivialClock*](https://en.cppreference.com/w/cpp/named_req/TrivialClock) requirements. Monotonic means it's non-decreasing, indicated by its `is_steady` member.

`stopwatch` is a specialization that uses `std::chrono::steady_clock`. **For all intents and purposes `stopwatch` is what you'll want to use**, unless you have specific needs regarding the underlying clock.
___

#### Constructor
```cpp
/* constructor */ = default;
```
The stopwatch class has no constructor defined. After creating an instance, you have to start it manually by calling its `start()` method.
___

#### `start()` method
```cpp
MonotonicTrivialClock::duration start();

template <typename Duration>
Duration start();
```
Starts or restarts the stopwatch, and returns the elapsed time up until that point.

If the stopwatch is already running, it resets it to 0 and restarts it. This can be used as a "lap" function as well.

If the stopwatch is paused, it resumes it.

The templated version returns the time as `Duration`, which can be [`duration_components`](#duration_components-struct) or a version of `std::chrono::duration`. The non-template version uses the clock's own duration type.

The templated version is a shorthand for `sw::convert_time<Duration>(MySW.start())`.
___

#### `pause()` method
```cpp
void pause();
```
Pauses the stopwatch. The elapsed time will be frozen until the stopwatch is resumed or reset.
___

#### `reset()` method
```cpp
void reset();
```
Resets the stopwatch. Regardless of its current state, the state after resetting will be the same as a newly constructed instance.
___

#### `is_paused()` method
```cpp
[[nodiscard]] bool is_paused();
```
Indicates if the stopwatch is paused. While paused, the elapsed time is frozen until the stopwatch is resumed or reset.
___

#### `get_time()` method
```cpp
[[nodiscard]] MonotonicTrivialClock::duration get_time();

template <typename Duration>
[[nodiscard]] Duration get_time();
```
Returns the elapsed time.

The templated version returns the time as `Duration`, which can be [`duration_components`](#duration_components-struct) or a version of `std::chrono::duration`. The non-template version uses the clock's own duration type.

The templated version is a shorthand for [`sw::convert_time`](#convert_time-utility)`<Duration>(MySW.get_time())`.