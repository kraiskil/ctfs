#include "tones.h"

enum tone find_tone(const frequency_t frequency)
{
	/* TODO: how about a smarter search algorithm? */
	for (int i = A3; i < LAST_TONE; i++) {
		enum tone e = (enum tone)i;
		if (tone_freq[e] == frequency)
			return e;
	}

	return NOT_A_TONE;
}

