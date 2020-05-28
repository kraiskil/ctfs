#include "config.h"
#include "sin_table_float.h"
#include "tones.h"
#include <cassert>
#include <cmath>

int16_t get_croak_data(int i)
{
	enum tones tone = C5; // TODO: this is a parameter
	int16_t    d = 0;
	int16_t    uv = 50000; //unit volume
	static_assert(SIN_TABLE_RESOLUTION <= SIN_TABLE_N_ELEM, "Need better algorithm now");

	//float step = tone_freqs.get_freq(tone) * SIN_TABLE_N_ELEM / config_fs_output;
	//float step = 440 * SIN_TABLE_RESOLUTION / config_fs_output;
	// not a sine float step = SIN_TABLE_RESOLUTION/(config_fs_output / tone_freqs.get_freq(tone));
	// sine: float step = SIN_TABLE_RESOLUTION/(config_fs_output / 1000);
	//float step =  (float)SIN_TABLE_RESOLUTION/(config_fs_output / tone_freqs.get_freq(tone));
	float is_per_cycle = config_fs_output / tone_freqs.get_freq(tone);
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	int idx = phase * SIN_TABLE_RESOLUTION;

	d = sin_table_float[idx] * uv / 2;
	//d += sin_table[(int)(i * 2*step) % (SIN_TABLE_RESOLUTION)] * uv / 2;
	//d += sin_table[(int)(i * 3*step) % (SIN_TABLE_RESOLUTION)] * uv / 2;
	//d += sin_table[(int)(i * 4*step) % (SIN_TABLE_RESOLUTION)] * uv / 2;

	float amp = 0;
	if (i < croak_num_samples / 2)
		amp = 2.0 * i / croak_num_samples;
	else
		amp = 2.0 * (croak_num_samples - i) / croak_num_samples;

	//d *= amp;

	return d;
}

