#ifndef GAME_TIME_H
#define GAME_TIME_H

#include <chrono>

#include "accessor.hpp"

class GameTime {
	public:
		using duration = std::chrono::steady_clock::duration;
		using time_point = std::chrono::steady_clock::time_point;

		GameTime();
		Accessor<float> game_seconds_per_real_second;

		duration time();

	private:
		// Prevent mutation through assinment, which could allow time to go backwards,
		// but don't prevent copying elsewhere.
		GameTime &operator=(const GameTime&) = delete;

		duration passed_time;
		time_point time_at_last_tick;
};

#endif
