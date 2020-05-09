#include "tones.h"
constexpr tone_frequencies tone_freqs;

enum tones find_tone(uint16_t frequency)
{
	for (int i = A3; i < LAST_TONE; i++) {
		enum tones e = (enum tones)i;
		// TODO: allow error margin: current STM32 setup
		// has a 3% error in frequency (due to inaccurate clocks)
		// also consider the FFT bin width at low frequencies.
		if (tone_freqs.get_freq(e) == frequency)
			return e;
	}

	return NOT_A_TONE;
}

