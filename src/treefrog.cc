#include "config.h"
#include "datatype.h"
#include "fft.h"
#include "frog_tones.h"
#include "harmonics.h"
#include "treefrog.h"


void treefrog(void)
{
	while (true) {
		listen_buf_t buffer;
		listen_for_croaks(buffer);

		if (should_I_croak(buffer))
			croak();
		else
			sleep_a_bit();
	}
}

bool should_I_croak(listen_buf_t &buffer)
{
	debug_led_on(LED_PROCESSING);
	frequency_buf_t out;
	frog_tones      ft(buffer, out);

	ft.dc_blocker();
	ft.fft();
	ft.find_peaks();

	uint16_t harmonics[3];
	find_harmonics(ft, harmonics);

	debug_led_off(LED_PROCESSING);
	if (harmonics[0] > 200)
		return true;
	else
		return false;

}

void croak(void)
{
	// probably could do some generic
	// croaking selection

	play_croak(/*TODO: select which croak */);
}

// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void)
{
	// TODO: determine how long to sleep

	sleep_for(/*TODO pass value */);
}

