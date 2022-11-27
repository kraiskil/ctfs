/* Common components for
 * the 'board support package'
 * for (simulation on) Linux/Posix
 * systems.
 * See also io_* for the different IO options.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "treefrog.h"

#include "io.h"

static std::chrono::time_point<std::chrono::steady_clock> start_time;

std::string led_to_string(enum led_ids led)
{
	switch (led)
	{
	case LED_CROAK: return "CROAK";
	case LED_SLEEP: return "SLEEP";
	case LED_PROCESSING: return "PROCESSING";
	case LED_LISTENING: return "LISTENING";
	default: return "UNKNOWN";
	}
}

void sleep_for(const milliseconds_t &time)
{
	debug_led_on(LED_SLEEP);
	std::this_thread::sleep_for(time);
	debug_led_off(LED_SLEEP);
}

void debug_led_on(enum led_ids led)
{
	auto time = std::chrono::steady_clock::now() - start_time;
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(time).count();
	std::cout << secs << "\tLED ON: " << led_to_string(led) << std::endl;
}
void debug_led_off(enum led_ids led)
{
	auto time = std::chrono::steady_clock::now() - start_time;
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(time).count();
	std::cout << secs << "\tLED OFF: " << led_to_string(led) << std::endl;
}
void wallclock_start(void)
{}
uint32_t wallclock_time_us(void)
{
	return 0;
}

float get_input_frequency_correction(void)
{
	return 1.0;
}

void board_init(void)
{
	start_time = std::chrono::steady_clock::now();
	io_init();
}

