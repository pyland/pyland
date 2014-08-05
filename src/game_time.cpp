#include <chrono>

#include "accessor.hpp"
#include "game_time.hpp"

GameTime::GameTime():
    GameTime(1.0, duration(0), std::chrono::steady_clock::now())
    {}

GameTime::GameTime(const GameTime &other):
    GameTime(other.game_seconds_per_real_second, other.passed_time, other.time_at_last_tick)
    {}

GameTime::GameTime(double game_seconds_per_real_second,
                   duration passed_time,
                   std::chrono::steady_clock::time_point time_at_last_tick):

    game_seconds_per_real_second(
        game_seconds_per_real_second,
        Accessor<double>::default_getter,
        [this] (double value) { time(); return value; }
    ),
    passed_time(passed_time),
    time_at_last_tick(time_at_last_tick)
    {}

GameTime::time_point GameTime::time() {
    auto real_time = std::chrono::steady_clock::now();
    auto real_time_difference = real_time - time_at_last_tick;

    // Ignore the potential for inaccuracy;
    // the point is that everything will be inaccurate the
    // same way so actual (microsecond) jitter isn't really important.
    passed_time += std::chrono::duration_cast<duration>(
        real_time_difference * double(game_seconds_per_real_second)
    );
    time_at_last_tick = real_time;

    // Make it seem like this is a time.
    return time_point(passed_time);
}
