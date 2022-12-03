#include "config.h"
#include "croak_sound.h"
#include "sin_table_float.h"
#include "tones.h"
#include <cassert>
#include <cmath>
#include <cstdio>

constexpr int N = 8;
constexpr int nfb = 5;
float         b[nfb][N] = {
/* 500 */ { 0.023002, 0.092000, 0.232851, 0.304294, 0.232851, 0.092000, 0.023002 },
/* 500 */ { 0.023002, 0.092000, 0.232851, 0.304294, 0.232851, 0.092000, 0.023002 },
// /* 1000 */{0.019768, 0.087736, 0.235581, 0.313830, 0.235581, 0.087736, 0.019768},
///* 1250 */ {0.017451, 0.084478, 0.237536, 0.321071, 0.237536, 0.084478, 0.017451},
/* 1500 */ { 0.014756, 0.080439, 0.239806, 0.329999, 0.239806, 0.080439, 0.014756 },
// /*1750 */{0.011767, 0.075600, 0.242310, 0.340647, 0.242310, 0.075600, 0.011767},
// /*2000*/ {0.0085872, 0.0699454, 0.2449476, 0.3530395, 0.2449476, 0.0699454, 0.0085872},
/* 3000 */ { -0.0035679, 0.0394313, 0.2540534, 0.4201664, 0.2540534, 0.0394313  - 0.0035679 },
/* 3000 */ { -0.0035679, 0.0394313, 0.2540534, 0.4201664, 0.2540534, 0.0394313  - 0.0035679 },
};
// phase-angle of the output wave
static inline float calc_phase(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);

	return phase;
}

static inline float calc_square(int i, float freq)
{
	float phase = calc_phase(i, freq);
	if (phase < 0.5)
		return 1;
	else
		return 0;
}

static inline float calc_sine(int i, float freq)
{
	float is_per_cycle = config_fs_output / freq;
	float phase = i / is_per_cycle - floor(i / is_per_cycle);
	assert(phase >= 0 && phase <= 1);
	int idx = phase * SIN_TABLE_RESOLUTION;

	return sin_table_float[idx];
}

static inline float calc_rectsine(int i, float freq)
{
	float phase = calc_phase(i, freq);
	int   idx = phase * SIN_TABLE_RESOLUTION;

	return fabs(sin_table_float[idx]);
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

float filter(float in, float b[N])
{
	static uint_fast8_t idx = 0;
	static float        x[N] = { 0 };
	// cutoff at 1500/8000

	x[idx] = in;
	float accu = 0;
	for (uint_fast8_t k = 0; k < N; k++) {
		// create the n-k idx
		// n==idx
		uint_fast8_t xidx = (idx - k + N) % N; // TODO: check the assembly
		assert(xidx >= 0);
		assert(xidx < N);
		accu += b[k] * x[xidx];
	}

	idx++;
	idx = idx % N; // TODO; check the compiler does a mask here.
	return accu;
}

int16_t get_croak_data(int i, enum note tone)
{
	float   d = 0;
	int16_t uv = config_output_volume_value;
	static_assert(SIN_TABLE_RESOLUTION <= SIN_TABLE_N_ELEM, "Need better algorithm now");

	frequency_t base = tone_freq[tone];


	float vibrato = calc_sine(i, 10) * 0.00001 + 1;
	d = calc_croak(i, (float)base * vibrato) * uv;
	//d += calc_sine(i, base * base+0.5 ) * uv/2;
	// 2nd voice
	//d += calc_croak(i, (base +0.1)*vibrato) * uv;
	// echo
	//d += calc_triangle(i-30, base) * uv/2;

	// This gives a "froggish" sound
	float tremolo = calc_sine(i, 35) * 0.35;
	d -= d * tremolo;

	// Individual croaks. Sawtooth might sound better?
	// Try adding this only in attack & decay phases - sounds kinda nice
	//float tremolo2 = calc_sine(i, 2);
	//d -= d*tremolo2;

	float filter_lfo = calc_sine(i, 2) / 2 + 1 * nfb - 1;
	int   filter_idx = filter_lfo;
	assert(filter_idx > 0 && filter_idx < nfb);
	d = filter(d, b[filter_idx]);

	/* Run the ADSR filter */
	float amp;
	if (i < attack_len) {
		amp = attack_level * i / attack_len;
		//amp *= amp;
	}
	else if (i < (attack_len + decay_len) ) {
		int dec_i_left = decay_len - (i - attack_len);
		amp = sustain_level + (attack_level - sustain_level) * (dec_i_left) / decay_len;
		//amp *= amp;
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

