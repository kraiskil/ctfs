#include "tones.h"

enum tone tones::find_tone(const frequency_t frequency)
{
	/* TODO: how about a smarter search algorithm? */
	for (int i = A3; i < LAST_TONE; i++) {
		enum tone e = (enum tone)i;
		if (tone_freq[e] == frequency)
			return e;
	}

	return NOT_A_TONE;
}

/* check if peak at base is a croak:
 * The fine-tuning parameters here depend on the croak
 * wavelet used */
bool tones::has_harmonics(struct peak &base)
{
	frequency_t comp_freq = (float)base.freq * 2 - 2.45;
	for (auto p : peaks) {
		float diff = (float)p.freq - (float)comp_freq;
		if (fabs(diff) < 3)
			return true;
	}
	return false;
}



bool tones::has_croak()
{
	for (auto p: peaks)
		if (p.freq == 0)
			continue;
		else if (has_harmonics(p) )
			return true;
	return false;
}

