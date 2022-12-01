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
const float tone_freq[LAST_TONE + 2] = {
	220.0f,  233.08f, 246.94f, 261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.0f,  415.3f,  440.0f,  466.16f,
	493.88f, 523.25f, 554.37f, 587.33f, 622.25f, 659.26f, 698.46f, 739.99f, 783.99f, 830.61f, 880.0f,  932.33f, 987.77f, 1046.5f,
	1108.7f, 1174.7f, 1244.5f, 1318.5f, 1396.9f, 1480.0f, 1568.0f, 1661.2f, 1760.0f, 1864.7f, 1975.5f, 2093.0f, 2217.5f, 2349.3f,
	2489.0f, 2637.0f, 2793.8f, 2960.0f, 3136.0f, 3322.4f, 3520.0f,
	0, // sentinel - NOT_A_TONE
};


class tones
{
public:
	tones(peak_array_t &peaks)
		: peaks(peaks)
	{
		detected_tones.fill(NOT_A_TONE);
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

