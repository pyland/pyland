#ifndef TIME_H
#define TIME_H

#include <chrono>

#include "accessor.hpp"

class Time {
	public:
		using duration = std::chrono::steady_clock::duration;
		using time_point = std::chrono::steady_clock::time_point;

		Time();
		Accessor<float> game_seconds_per_real_second;

		duration time();

	private:
		duration passed_time;
		time_point time_at_last_tick;
};

#endif
