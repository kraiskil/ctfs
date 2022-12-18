#include "config.h"
#include "datatype.h"
#include "debug.h"
#include "peak_detect.h"
#include "fft.h"
#include "treefrog.h"
#include <algorithm>
#include <cmath>

using std::max;
using std::min;

/* Peak detection algorithms from
 * Simple Algorithms for Peak Detection in Time-Series
 * Girish Keshav Palshikar.
 * Read this paper to understand the implementations! */
/* Assume unsigned input data!*/
unsigned s1(const frequency_buf_t &buffer, const unsigned i, const unsigned k)
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

void peak_detect::find_peaks(void)
{
	uint32_t        start_time = wallclock_time_us();
	frequency_buf_t a;
	// The number of bins to look to the sides for mergeable peak-candidate bins
	// Must be this low, because with low frequencies, the specral resolution
	// is ~4Hz, and notes at A2-level are spaced ~7Hz.
	// TODO: this should be a configuration parameter based on how low we must be
	// able to go, and fft resolution.
	constexpr int windowsize = 2;
	peak_bins.fill({ 0, 0 });

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
	peak_bins[0] = { 0, 0 };
	peak_bins[peak_bins.size() - 1].bin = peak_bins.size() - 1;
	peak_bins[peak_bins.size() - 1].val = 0;
	for (unsigned i = 1; i < a.size() - 1; i++) {
		peak_bins[i].bin = i;
		// avoid unsigned underflow
		if (a[i] < mean)
			continue;

		if ( (a[i] - mean) > (peak_stddev_limit * stddev) ) {
			// Original algorithm says to do
			// tones[i].val = freq_buffer[i];
			// here. But since input is a FFT output, lets
			// do a real Q&D peak value approximation to combat
			// the energy being spread across two adjacent bins.
			// This is far from accurate (no sincs involved! :))
			// Weigh the i'th bin a bit to avoid draws that shift
			// the interpolated frequency later
			peak_bins[i].val = max(((freq_buffer[i - 1] * 3) >> 2) + freq_buffer[i],
			        freq_buffer[i] + ((freq_buffer[i + 1] * 3) >> 2));
		}
	}

	// remove adjacent peak values
	// where peaks are less than windowsize apart
	// This implementation smells. There has to be
	// a better one available - investigate *before*
	// saturday evening coctails...
	//
	// One clear bug here is that this implementation
	// is with wide peaks, spanning more than 2*windowsize bins.
	// Smooth rises (when walking the tones in order of increasing
	// frequency) of peaks to be detected as single
	// peak, but a wide peak will be split into two on the
	// descending slope.
	static_assert(windowsize > 0, "bad indexing ensues");
	for (unsigned i = windowsize; i < peak_bins.size() - 1; i++) {
		if (peak_bins[i].val == 0)
			continue;

		for (unsigned j = i - windowsize; j < i; j++) {
			if (peak_bins[j].val > peak_bins[i].val)
				peak_bins[i].val = 0;
			else
				peak_bins[j].val = 0;
		}
	}

	sort_tones_by_value();
	for (unsigned i = 0; i < peaks.size(); i++) {
		if (peak_bins[i].val == 0)
			break;
		peaks[i] = peak_at(peak_bins[i].bin);
	}

	#ifdef HAVE_DEBUG_MEASUREMENTS
	peak_detect_execution_time = wallclock_time_us() - start_time;
	#else
	(void)start_time;
	#endif

}

void peak_detect::sort_tones_by_value(void)
{
	std::sort(peak_bins.begin(), peak_bins.end(),
	    [] (struct bin_val &a, struct bin_val &b)
	    {
		    return a.val > b.val;
	    }
	    );
}
void peak_detect::sort_tones_by_bin(void)
{
	std::sort(peak_bins.begin(), peak_bins.end(),
	    [] (struct bin_val &a, struct bin_val &b)
	    {
		    return a.bin < b.bin;
	    }
	    );
}

unsigned peak_detect::get_num_peaks(void)
{
	sort_tones_by_value();
	int numtones = 0;
	for (auto v : peak_bins) {
		if (v.val == 0)
			break;
		numtones++;
	}
	return numtones;
}

struct bin_val peak_detect::get_peak_by_val(uint16_t peak_num)
{
	sort_tones_by_value();
	return peak_bins[peak_num];
}
struct bin_val peak_detect::get_peak_by_bin(uint16_t peak_num)
{
	sort_tones_by_bin();
	uint16_t cpn = 0;
	for (unsigned i = 0; i < peak_bins.size(); i++) {
		if (peak_bins[i].val == 0)
			continue;
		if (cpn == peak_num)
			return peak_bins[i];
		cpn++;
	}
	// This should not happen - logically
	return { 0, 0 };
}


frequency_t peak_detect::bin_frequency(uint16_t frequency_bin) const
{
	float rv = (float)frequency_bin * config_fs_input / listen_buffer_samples;
	/* Correct systematic bad clock */
	rv *= frequency_correction;
	return rv;
}
/* same thing, but allow fractional bins - results of peak interpolation */
frequency_t peak_detect::bin_frequency(float frequency_bin) const
{
	float rv = frequency_bin * config_fs_input / listen_buffer_samples;
	/* Correct systematic bad clock */
	rv *= frequency_correction;
	return rv;
}

struct peak peak_detect::peak_at(uint16_t bin) const
{
	// return peak that has its maximum bin at frequency_bin
	// Use first algorithm that ecosia throws: the 'barycentric method':
	// https://dspguru.com/dsp/howtos/how-to-interpolate-fft-peak/
	uint16_t l, c, r;
	if (bin > 0)
		l = freq_buffer[bin - 1];
	else
		l = 0;  //?
	if (bin < freq_buffer.size() - 1)
		r = freq_buffer[bin + 1];
	else
		r = 0;
	c = freq_buffer[bin];

	float d = (float)(r - l) / (l + c + r);

	frequency_t f = bin_frequency(d + bin);
	uint16_t    peak_val = freq_buffer[bin]; // TODO: interpolate the amplitude too?

	return { f, peak_val };
}

bool peak_detect::has_peak_at(frequency_t freq)
{
	sort_tones_by_bin();
	for (auto &v: peak_bins) {
		if (v.val == 0)
			continue;
		if (freq.close_enough(peak_at(v.bin).freq))
			return true;
	}
	return false;
}

