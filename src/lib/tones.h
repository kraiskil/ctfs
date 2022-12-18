#pragma once
#include "datatype.h"
#include <cstdint>

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


// This class is badly named.
// A more descriptive name would be 'soundscape'
// When constructing an object of this class
// is fed with collection of peaks found from a FFT
// (in order to separate responsibilities, the FFT, peak detection
//  and tone detection (here) are kept separate)
// TODO: this must be split up:
//   - detection of tones
//   - decision on what croaks to generate
class tones
{
public:
	tones(peak_array_t &peaks, croak_array_t &tones)
		: peaks(peaks), detected_tones(tones)
	{
		detected_tones.fill(NOT_A_TONE);
	}

	// Does this soundscape contain the sound of a croak (i.e. tone)
	bool has_croak(void);
	// Given the existing croaks in this soundscape, what should the next
	// croak be
//	enum note what_to_croak(void);
	// process input peaks data, fill detected_tones with
	// result
	void detect_tones(void);

// TODO: this function should be private for "production" code,
//       but unit tests need it
	enum note find_tone(const frequency_t frequency);
private:
	enum note first_harmonic(void);
	enum note second_harmonic(void);

	bool has_harmonics(struct peak &base);
	// Array of frequencies and their amplitudes
	peak_array_t &peaks;
	// Array of detected tones.
	croak_array_t &detected_tones;
};

