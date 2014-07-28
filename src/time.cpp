#include <chrono>

#include "accessor.hpp"
#include "time.hpp"

Time::Time():
	game_seconds_per_real_second(
		1,
		Accessor<float>::default_getter,
		[this] (float value) { time(); return value; }
	) {}

Time::duration Time::time() {
	auto real_time = std::chrono::steady_clock::now();
	auto real_time_difference = real_time - time_at_last_tick;

	passed_time += std::chrono::duration_cast<duration>(
		real_time_difference * float(game_seconds_per_real_second)
	);
	time_at_last_tick = real_time;

	return passed_time;
}
