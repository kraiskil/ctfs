#include "harmonics.h"

bool has_harmonics(frog_tones &ft, int i)
{
	uint16_t base_bin = ft.get_peak_by_bin(i).bin;
	i++;
	for (; i < ft.get_num_peaks(); i++) {
		uint16_t comp_bin = ft.get_peak_by_bin(i).bin;

		int peak_diff = comp_bin - base_bin * 2;
		if (peak_diff <= 1 && peak_diff >= -1)
			return true;
	}
	return false;
}

void find_harmonics(frog_tones &ft, uint16_t (&harmonics)[3])
{
	uint16_t peak;
	int      n_harm = 0;
	unsigned np = ft.get_num_peaks();

	harmonics[0] = 0;
	harmonics[1] = 0;
	harmonics[2] = 0;

	for (int i = 0; i < np; i++) {
		peak = ft.get_peak_by_bin(i).bin;

		// search for a harmonic
		bool found = has_harmonics(ft, i);

		// if found, record as kth harmonic
		if (found) {
			harmonics[n_harm] = ft.as_Hz(peak);
			n_harm++;
			if (n_harm > 2)
				return;
		}
		// TODO: filter by amplitude?

	}
}

