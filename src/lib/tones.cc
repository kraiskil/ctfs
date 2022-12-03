#include "tones.h"
#include "debug.h"

enum tone tones::find_tone(const frequency_t frequency)
{
	/* TODO: how about a smarter search algorithm? */
	for (int i = A3; i < LAST_TONE; i++) {
		enum tone e = (enum tone)i;
		if (frequency.close_enough(tone_freq[e]))
			return e;
	}

	return NOT_A_TONE;
}

/* check if peak at base is a croak:
 * The fine-tuning parameters here depend on the croak
 * wavelet used */
bool tones::has_harmonics(struct peak &base)
{
	frequency_t comp_freq = (float)base.freq * 2; // - 2.45;
	for (auto p : peaks) {
		if (p.freq == 0)
			break;
		float diff = (float)p.freq - (float)comp_freq;
		if (fabs(diff) < 3)
			return true;
	}
	return false;
}



bool tones::has_croak(void)
{
	if (detected_tones[0] == NOT_A_TONE)
		detect_tones();
	return detected_tones[0] != NOT_A_TONE;
}

/* Search peaks-array for croaks (=peaks with correct harmonics
 * and base frequency on the equal tempered scale notes).
 * Store results in detect_tones-array */
void tones::detect_tones(void)
{
	unsigned tone_idx = 0;
	DB_PRINT("peaks:\n");
	for (auto p: peaks) {
		if (p.freq == 0)
			continue;
		DB_PRINT("\tpeak: %f, amplitude %d\n", float(p.freq), p.ampl);
		if (has_harmonics(p)) {
			enum tone t = find_tone(p.freq);
			if (t != NOT_A_TONE)
				detected_tones[tone_idx++] = t;
		}
	}
	#ifndef NDEBUG
#error todo print tone frequencies to match
	DB_PRINT("detected_tones:\n");
	for (auto t: detected_tones) {
		if (t == NOT_A_TONE)
			continue;
		DB_PRINT("\ttone: %d\n", t);
	}
	#endif
}

enum tone tones::first_harmonic(void)
{
	// Assume we want I of detected_tones[0]-major chord
	return static_cast<enum tone>(detected_tones[0] + 4);
}
enum tone tones::second_harmonic(void)
{
	// Assume we want I of detected_tones[0]-major chord
	if (detected_tones[1] == detected_tones[0] + 4)
		return static_cast<enum tone>(detected_tones[0] + 7);
	else
		return NOT_A_TONE;
}

enum tone tones::what_to_croak(void)
{
	// TODO: random start-croak
	if (detected_tones[0] == NOT_A_TONE)
		return E4;

	if (detected_tones[1] == NOT_A_TONE)
		return first_harmonic();

	if (detected_tones[2] == NOT_A_TONE)
		return second_harmonic();

	return NOT_A_TONE;
}

