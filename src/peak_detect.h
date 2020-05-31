#pragma once
#include "datatype.h"
#include "fft.h"

struct bin_val
{
	uint16_t bin;
	uint16_t val;
};


class peak_detect
{
public:
	peak_detect(listen_buf_t &input_buf,
	    frequency_buf_t &freq_buf,
	    peak_array_t &peaks,
	    unsigned peak_stddev_limit = default_peak_stddev_limit
	    )
		: audio_buffer(input_buf), // what is this for???
		freq_buffer(freq_buf),
		peaks(peaks),
		peak_stddev_limit(peak_stddev_limit),
		frequency_correction(1)
	{
		tones.fill({ 0, 0 });
		peaks.fill({ 0, 0 }); //TODO. assume this is in .bss and zeroed?
	}
	void find_peaks(void);
	virtual unsigned get_num_peaks(void);
	virtual struct bin_val get_peak_by_val(uint16_t peak_num);
	virtual struct bin_val get_peak_by_bin(uint16_t peak_num);
	/* Get the center frequency of bin. NOT the center frequency
	 * of a peak */
	virtual frequency_t bin_frequency(uint16_t frequency_bin) const;
	/* calculate the peak frequency of the peak with maximum at
	 * the given bin. This does inerpolation. */
	virtual struct peak peak_at(uint16_t frequency_bin) const;
	bool has_peak_at(frequency_t frequency);

private:
	typedef std::array<struct bin_val, frequency_buffer_samples> tone_array_t;
	void sort_tones_by_bin(void);
	void sort_tones_by_value(void);
	listen_buf_t &audio_buffer;
	frequency_buf_t &freq_buffer;
	peak_array_t &peaks;
	tone_array_t tones;
public:
	unsigned peak_stddev_limit;
	float frequency_correction;

};

