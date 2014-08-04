#ifndef GAME_TIME_H
#define GAME_TIME_H

#include <chrono>

#include "accessor.hpp"

class GameTime {
    public:
        // Basically makes this a clock class
        // without Clock::now
        using duration   = std::chrono::duration<double>;
        using rep        = duration::rep;
        using period     = duration::period;

        // I'm not sure if I'm really allowed to do this
        // without a declaration of GameTime::now,
        // but it is what it is.
        using time_point = std::chrono::time_point<GameTime>;

        GameTime();
        GameTime(const GameTime &other);

        Accessor<double> game_seconds_per_real_second;
        time_point time();

    private:
        // This exists to unify the two constructors
        GameTime(double game_seconds_per_real_second,
                 duration passed_time,
                 std::chrono::steady_clock::time_point time_at_last_tick);

        // Prevent mutation through assinment, which could allow time to go backwards,
        // but don't prevent copying elsewhere.
        GameTime &operator=(const GameTime &) = delete;

        // Internally we're only keeping track of a delta, but
        // we'll return time_point instances as if we had
        // a real reference for time.
        duration passed_time;
        std::chrono::steady_clock::time_point time_at_last_tick;
};

#endif
