#include <chrono>
#include <iostream>
#include <thread>

#include "treefrog.h"

void sleep_for(void)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void debug_led_on(enum led_ids led)
{
	std::cout << "LED ON: " << led << std::endl;
}
void debug_led_off(enum led_ids led)
{
	std::cout << "LED OFF: " << led << std::endl;
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
{}

