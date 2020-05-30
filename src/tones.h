#pragma once
#include <cstdint>

enum tones
{
	/* Lowest note & octave that FFT has currently enough resolution for */
	A3         = 0, As3 = 1, Bf3 = 1, B3 = 2, C4 = 3, Cs4 = 4, Df4 = 4, D4 = 5,
	Ds4        = 6, E4  = 7, F4  = 8, Ff4 = 9, G4  = 10, Gf4 = 11,
	/* Next one */
	A4         = 12, As4 = 13, B4  = 14, C5  = 15, Cs5 = 16, D5  = 17, Ds5 = 18,
	E5         = 19, F5  = 20, Fs5 = 21, G5  = 22, Gs5 = 23,
	A5         = 24, As5 = 25, B5  = 26, C6  = 27, Cs6 = 28, D6  = 29, Ds6 = 30,
	E6         = 31, F6  = 32, Fs6 = 33, G6  = 34, Gs6 = 35,
	A6         = 36, As6 = 37, B6  = 38, C7  = 39, Cs7 = 40, D7  = 41, Ds7 = 42,
	E7         = 43, F7  = 44, Fs7 = 45, G7  = 46, Gs7 = 47,
	/* No point in going higher: A7 is 3500Hz */
	A7         = 48,
	LAST_TONE  = 48,
	NOT_A_TONE = 99,
};

constexpr double twelweth_root_of_2 = 1.05946309435930;
class tone_frequencies
{
public:
	float freq[LAST_TONE];
	constexpr tone_frequencies() : freq()
	{
		static_assert(A3 == 0, "starting calculation from wrong tone");
		double start_freq = 220;
		for (auto &f : freq) {
			f = start_freq;
			start_freq *= twelweth_root_of_2;
		}
	}
	float get_freq(enum tones idx) const
	{
		return freq[idx];
	}
};
static constexpr tone_frequencies tone_freqs;

enum tones find_tone(uint16_t frequency);

