#include "treefrog.h"
#include "target_adaptation_api.h"

int main(int argc, char *argv[])
{
	board_init();

	while (true) {
		debug_led_on(LED_CROAK);
		sleep_for();
		debug_led_off(LED_CROAK);
		sleep_for();
	}
}

