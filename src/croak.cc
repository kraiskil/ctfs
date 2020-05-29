#include "config.h"
#include "sin_table_float.h"
#include "tones.h"
#include <cassert>
#include <cmath>
#include <cstdio>

static inline int calc_index(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	int idx = phase * SIN_TABLE_RESOLUTION;

	return idx;
}

static float rnd(void)
{
	static float r = 0;
	r += 0.001;
	if (r > 0.1) r = 0;
	return r;
}

int16_t get_croak_data(int i)
{
	enum tones tone = C5; // TODO: this is a parameter
	int16_t    d = 0;
	int16_t    uv = 2000; //unit volume
	static_assert(SIN_TABLE_RESOLUTION <= SIN_TABLE_N_ELEM, "Need better algorithm now");

	float base = tone_freqs.get_freq(tone);

	int idx = calc_index(i, base);
	d = (rnd() + sin_table_float[idx]) * uv / 2;

	idx = calc_index(i, 2 * base);
	d += (rnd() + sin_table_float[idx]) * uv;

	idx = calc_index(i, 3 * base);
	d += (rnd() + sin_table_float[idx]) * uv / 4;

#if 0
	idx = calc_index(i, 2);
	d *= (sin_table_float[idx] * 0.5 + 0.4);

	idx = calc_index(i, 2.3 * base);
	d += sin_table_float[idx] * uv / 8;

//	idx = calc_index(i, 4.3*base);
//	d += sin_table_float[idx] * uv / 16;

	idx = calc_index(i, 3.3 * base);
	d += sin_table_float[idx] * uv / 8;
#endif



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

