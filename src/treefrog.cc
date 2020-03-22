#include "config.h"
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

#if 0
#include <iostream>
#include <iomanip>
#endif
bool should_I_croak(listen_buf_t &buffer)
{
	listen_buf_t out;
	uint16_t     tones[3];

	frog_fft(buffer, out);
	find_tones(out, tones);

#if 0
	std::cout << "tones: " << std::endl;
	for (int i = 0; i < 3; i++) {
		std::cout << i;
		std::cout <<  "\tbin: " << std::setfill(' ') << std::setw(4) << tones[i];
		std::cout << "\tampl: " << std::setfill(' ') << std::setw(4) << out[tones[i]];
		std::cout << "\tHz: " << index_to_frequency(tones[i], config_fs, buffer.size()) << std::endl;
	}
#endif
	/* Greatest component is DC - should be no croak here */
	if (tones[0] == 0)
		return false;
	/* TODO: heuristic limit on what is a croak */
	if (out[tones[0]] < 3000)
		return false;

	return true;
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

