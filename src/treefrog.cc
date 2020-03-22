#include "datatype.h"
#include "fft.h"
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
	listen_buf_t out;
	uint16_t     tones[3];


	frog_fft(buffer, out);
	find_tones(out, tones);

	static bool rv = true;
	rv = !rv;
	return rv;
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

