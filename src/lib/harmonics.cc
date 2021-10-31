#include "harmonics.h"

bool has_harmonics(peak_detect &ft, unsigned bin_no)
{
	uint16_t base_bin = ft.get_peak_by_bin(bin_no).bin;
	bin_no++;
	for (; bin_no < ft.get_num_peaks(); bin_no++) {
		uint16_t comp_bin = ft.get_peak_by_bin(bin_no).bin;

		int peak_diff = comp_bin - base_bin * 2;
		if (peak_diff <= 1 && peak_diff >= -1)
			return true;
	}
	return false;
}

void find_harmonics(peak_detect &ft, uint16_t (&harmonics)[3])
{
	uint16_t peak;
	int      n_harm = 0;
	unsigned np = ft.get_num_peaks();

	harmonics[0] = 0;
	harmonics[1] = 0;
	harmonics[2] = 0;

	for (unsigned i = 0; i < np; i++) {
		peak = ft.get_peak_by_bin(i).bin;

		// search for a harmonic
		bool found = has_harmonics(ft, i);

		// if found, record as kth harmonic
		if (found) {
			// TODO: should this be peak-frequency, not
			// bin center frequency?
			harmonics[n_harm] = ft.bin_frequency(peak);
			n_harm++;
			if (n_harm > 2)
				return;
		}
		// TODO: filter by amplitude?

	}
}

