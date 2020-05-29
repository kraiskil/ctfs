#include "config.h"
#include "croak_sound.h"
#include "sin_table_float.h"
#include "tones.h"
#include <cassert>
#include <cmath>
#include <cstdio>

static inline float calc_sine(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	int idx = phase * SIN_TABLE_RESOLUTION;

	return sin_table_float[idx];
}
static inline float calc_croak(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	int idx = phase * SIN_TABLE_RESOLUTION;

	return croak_sound[idx];
}


static inline float calc_triangle(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	if (phase < 0.5)
		return -1 + 4 * phase;
	else
		return 1 - 4 * (phase - 0.5);
}

static inline float calc_saw(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	return 2 * phase - 1;
}

int16_t get_croak_data(int i)
{
	enum tones tone = C4; // TODO: this is a parameter
	int16_t    d = 0;
	int16_t    uv = 2000; //unit volume
	static_assert(SIN_TABLE_RESOLUTION <= SIN_TABLE_N_ELEM, "Need better algorithm now");

	float base = tone_freqs.get_freq(tone);


	float vibrato = calc_sine(i, 10) * 0.0001 + 1;
	d = calc_croak(i, base * vibrato) * uv;
	//d += calc_sine(i, base * base ) * uv;
	// 2nd voice
	//d += calc_croak(i, (base +0.1)*vibrato) * uv;
	// echo
	//d += calc_croak(i-300, base) * uv;

	float tremolo = calc_sine(i, 22) * 0.01 + 1;
	d *= tremolo;

	/* Run the ADSR filter */
	float amp;
	if (i < attack_len) {
		amp = attack_level * i / attack_len;
	}
	else if (i < (attack_len + decay_len) ) {
		int dec_i_left = decay_len - (i - attack_len);
		amp = sustain_level + (attack_level - sustain_level) * (dec_i_left) / decay_len;
	}
	else if (i < (attack_len + decay_len + sustain_len) ) {
		amp = sustain_level;
	}
	else {
		int rel_i_left = release_len - (i - (attack_len + decay_len + sustain_len));
		amp = sustain_level * rel_i_left / release_len;
	}
	d *= amp;

	return d;
}

