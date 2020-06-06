#pragma once
#include "datatype.h"
#include <cstdint>

enum tone
{
	/* Lowest note & octave that FFT has currently enough resolution for */
	A3         = 0, As3 = 1, Bf3 = 1, B3 = 2, C4 = 3, Cs4 = 4, Df4 = 4, D4 = 5,
	Ds4        = 6, E4  = 7, F4  = 8, Fs4 = 9, G4  = 10, Gs4 = 11,
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
	NOT_A_TONE = 49,
};

/* TODO: make this array a constexpr.
 * Probaby needs consterprifying frequency_t (as if the compiler
 * can't realize itself it), or perhaps yet another C++ spec? :)
 * This version does increase execution ROM size a bit...
 */
const frequency_t tone_freq[LAST_TONE + 2] = {
	220,    233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392,    415.3,  440,    466.16,
	493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880,    932.33, 987.77, 1046.5,
	1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480,   1568,   1661.2, 1760,   1864.7, 1975.5, 2093,   2217.5, 2349.3,
	2489,   2637,   2793.8, 2960,   3136,   3322.4, 3520,
	0, // sentinel - NOT_A_TONE
};


class tones
{
public:
	tones(peak_array_t &peaks)
		: peaks(peaks)
	{
		detected_tones.fill(NOT_A_TONE);
		detect_tones();
	}

	bool has_croak(void);
	enum tone what_to_croak(void);
	enum tone find_tone(const frequency_t frequency);
	void detect_tones(void);

private:
	enum tone first_harmonic(void);
	enum tone second_harmonic(void);

	bool has_harmonics(struct peak &base);
	// Array of frequencies and their amplitudes
	peak_array_t &peaks;
	// Array of detected tones.
	std::array<enum tone, max_detected_tones> detected_tones;
};

