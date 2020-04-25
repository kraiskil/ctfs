#include "frog_tones.h"
#include "fft.h"
#include <algorithm>
#include <cmath>

using std::max;
using std::min;

/* Peak detection algorithms from
 * Simple Algorithms for Peak Detection in Time-Series
 * Girish Keshav Palshikar.
 * Read this paper to understand the implementations! */


/* Assume unsigned input data!*/
unsigned s1(frequency_buf_t &buffer, unsigned i, unsigned k)
{
	int leftmax = 0;
	for (unsigned j = max(0U, i - k); j < i; j++) {
		int val = buffer[i] - buffer[j];
		leftmax = max(val, leftmax);
	}

	int rightmax = 0;
	for (unsigned j = i + 1; j < min(static_cast<unsigned>(buffer.size()), i + k + 1); j++) {
		int val = buffer[i] - buffer[j];
		rightmax = max(val, rightmax);
	}

	/* The last + buffer[i] is not part of the algorithm in the paper,
	 * but its lack seems rather a bug, IMHO */
	return (leftmax + rightmax) / 2;
}

float calculate_stddev(frequency_buf_t &a, uint32_t mean)
{
	float var = 0;
	for (auto v: a) {
		var += (v - mean) * (v - mean);
	}
	var /= a.size();
	return sqrt(var);
}

void frog_tones::find_peaks(void)
{
	frequency_buf_t a;
	int             windowsize = 3;
	for (unsigned i = 0; i < a.size(); i++) {
		a[i] = s1(freq_buffer, i, windowsize);
	}

	// C++ can't check the typeof(buffer[0]) == typeof(uint16_t]) ??
	static_assert(sizeof(freq_buffer[0]) == sizeof(uint16_t), "Check that the following does not overflow");
	uint32_t mean = 0;
	for (auto v : a)
		mean += v;
	mean /= a.size();

	float stddev = calculate_stddev(a, mean);

	// tone_array is same as O in the algorithm description
	// Just set those bins without statistically significant value to zero
	for (unsigned i = 0; i < a.size(); i++) {
		// avoid unsigned underflow
		if (a[i] < mean)
			continue;

		if ( (a[i] - mean) > (3 * stddev) ) {
			tones[i].bin = i;
			tones[i].val = a[i];
		}
	}

	// remove adjacent peak values
	// where peaks are less than windowsize apart
	// This implementation smells. There has to be
	// a better one available - investigate *before*
	// saturday evening coctails...
	for (unsigned i = windowsize; i < tones.size(); i++) {
		if (tones[i].val == 0)
			continue;

		for (unsigned j = i - windowsize; j < i; j++) {
			if (tones[j].val > tones[i].val)
				tones[i].val = 0;
			else
				tones[j].val = 0;
		}
	}
}

void frog_tones::sort_tones_by_value(void)
{
	std::sort(tones.begin(), tones.end(),
	    [] (struct bin_val &a, struct bin_val &b)
	    {
		    return a.val > b.val;
	    }
	    );
}
void frog_tones::sort_tones_by_bin(void)
{
	std::sort(tones.begin(), tones.end(),
	    [] (struct bin_val &a, struct bin_val &b)
	    {
		    return a.bin < b.bin;
	    }
	    );
}

unsigned frog_tones::get_num_peaks(void)
{
	sort_tones_by_value();
	int numtones = 0;
	for (auto v : tones) {
		if (v.val == 0)
			break;
		numtones++;
	}
	return numtones;
}

struct bin_val frog_tones::get_peak_by_val(uint16_t peak_num)
{
	sort_tones_by_value();
	return tones[peak_num];
}


void frog_tones::fft(void)
{
	int       fft_size = audio_buffer.size();
	complex_t data[MAX_FFT_SIZE];

	for (int i = 0; i < fft_size; i++) {
		data[i].real(audio_buffer[i]);
		data[i].imag(0);
	}

	fft_sa(fft_size, data);
	fft_calc_abs(data, freq_buffer);
}

