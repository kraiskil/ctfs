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
 * currently it means it has a harmonic at twice the base frequency.
 * This probably needs to be changed */
bool tones::has_harmonics(struct peak &base)
{
	frequency_t comp_freq = base.freq * 2;
	for (auto p : peaks) {
		if (p.freq == comp_freq)
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

