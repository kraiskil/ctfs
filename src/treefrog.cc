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

	frog_fft(buffer, out);

	frog_tones ft(out);
	ft.find_peaks();

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

