#include "datatype.h"
#include "treefrog.h"
#include "fft.h"

void treefrog(void)
{
	while (true) {
		croak_buf_t buffer;
		listen_for_croaks(buffer);

		if (should_I_croak(buffer))
			croak();
		else
			sleep_a_bit();
	}
}

bool should_I_croak(croak_buf_t& buffer)
{
	croak_buf_t out;
	frog_fft(buffer, out);
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

