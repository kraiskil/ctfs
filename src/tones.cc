#include "tones.h"

enum tones find_tone(uint16_t frequency)
{
	/* TODO: how about a smarter search algorithm? */
	for (int i = A3; i < LAST_TONE; i++) {
		enum tones e = (enum tones)i;
		if (tone_freqs.get_freq(e) == frequency)
			return e;
	}

	return NOT_A_TONE;
}

