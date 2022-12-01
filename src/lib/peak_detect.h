#pragma once
#include "datatype.h"
#include "fft.h"

/* A raw fft bin and its value. */
struct bin_val
{
	uint16_t bin;
	uint16_t val;
};

class peak_detect
{
public:
	peak_detect(
		frequency_buf_t &freq_buf,
		peak_array_t &peaks,
		unsigned peak_stddev_limit = default_peak_stddev_limit
		) :
		freq_buffer(freq_buf),
		peaks(peaks),
		peak_stddev_limit(peak_stddev_limit),
		frequency_correction(1)
	{
		peak_bins.fill({ 0, 0 });
		peaks.fill({ 0, 0 }); //TODO. assume this is in .bss and zeroed?
	}
	void find_peaks(void);
	virtual unsigned get_num_peaks(void);
	virtual struct bin_val get_peak_by_val(uint16_t peak_num);
	virtual struct bin_val get_peak_by_bin(uint16_t peak_num);
	/* Get the center frequency of bin. NOT the center frequency
	 * of a peak */
	virtual frequency_t bin_frequency(uint16_t frequency_bin) const;
	virtual frequency_t bin_frequency(float frequency_bin) const;
	/* calculate the peak frequency of the peak with maximum at
	 * the given bin. This does inerpolation. */
	virtual struct peak peak_at(uint16_t frequency_bin) const;
	bool has_peak_at(frequency_t frequency);

private:
	void sort_tones_by_bin(void);
	void sort_tones_by_value(void);
	frequency_buf_t &freq_buffer;
	peak_array_t &peaks;

	// Similar to frequency buffer, but with bin number data added,
	// for easier sorting. Also, peak value is calculated from adjacent
	// bins to combat peak spread (think FFT-sinc). bin numbers are still
	// integers to show from which bin the amplitude value is calculated
	// Bins that don't have a candidate peak are set to zero amplitude
	typedef std::array<struct bin_val, frequency_buffer_samples> binval_array_t;
	binval_array_t peak_bins;
public:
	unsigned peak_stddev_limit;
	float frequency_correction;

private:
	friend class FrogTonesPeakTest_MeasuredData1_Test;
	friend class FrogTonesPeakTest_MeasuredData2_Test;
};

