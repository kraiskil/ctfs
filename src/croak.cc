#include "config.h"
#include "sin_table.h"

int16_t get_croak_data(int i)
{
	int16_t d = 0;
	static_assert(SIN_TABLE_RESOLUTION <= SIN_TABLE_N_ELEM, "Need better algorithm now");
	d = sin_table[(i * 32) % (SIN_TABLE_RESOLUTION)] / 2;
	d += sin_table[(i * 64) % (SIN_TABLE_RESOLUTION)] / 2;
	d += sin_table[(i * 128) % (SIN_TABLE_RESOLUTION)] / 2;

	float amp = 0;
	if (i < croak_num_samples / 2)
		amp = 2.0 * i / croak_num_samples;
	else
		amp = 2.0 * (croak_num_samples - i) / croak_num_samples;

	d *= amp;

	return d;
}

