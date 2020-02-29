#include "treefrog.h"

void treefrog(void)
{
	while(true)
	{
		int16_t buffer[croak_buffer_size_samples];
		listen_for_croaks(buffer);

		if( should_I_croak(buffer) )
			croak();
		else
			sleep_a_bit();
	}
}

bool should_I_croak(int16_t *buffer)
{
	(void) buffer;
	static bool rv=true;
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

