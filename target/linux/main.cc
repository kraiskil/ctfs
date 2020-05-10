#include "treefrog.h"
#include <chrono>
#include <thread>

void io_init(void);

int main(void)
{
	io_init();
	treefrog();
}


void sleep_for(void)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void debug_led_on(enum led_ids)
{}
void debug_led_off(enum led_ids)
{}
void wallclock_start(void)
{}
uint32_t wallclock_time_us(void)
{
	return 0;
}

