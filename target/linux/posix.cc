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

std::string led_to_string(enum led_ids led)
{
	switch (led)
	{
	case LED_CROAK: return "CROAK";
	case LED_SLEEP: return "SLEEP";
	case LED_PROCESSING: return "PROCESSING";
	default: return "UNKNOWN";
	}
}

void sleep_for(const std::chrono::duration<int> &time)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(time);
}

void debug_led_on(enum led_ids led)
{
	std::cout << "LED ON: " << led_to_string(led) << std::endl;
}
void debug_led_off(enum led_ids led)
{
	std::cout << "LED OFF: " << led_to_string(led) << std::endl;
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
	io_init();
}

