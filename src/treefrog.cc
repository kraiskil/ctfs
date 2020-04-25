#include "config.h"
#include "datatype.h"
#include "fft.h"
#include "frog_tones.h"
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

#if 0
#include <iostream>
#include <iomanip>
#endif
bool should_I_croak(listen_buf_t &buffer)
{
	frequency_buf_t out;
	frog_tones      ft(buffer, out);

	ft.fft();

	ft.find_peaks();

	/* placeholder for more accurate harmonics detection */
	if (ft.get_num_peaks() == 1 && ft.get_peak_by_val(0).bin == 0)
		return false;
	return ft.get_num_peaks() > 0;
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

