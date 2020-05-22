#pragma once
#include "datatype.h"
#include "fft.h"

struct bin_val
{
	uint16_t bin;
	uint16_t val;
};
typedef std::array<struct bin_val, frequency_buffer_samples> tone_array_t;


class frog_tones
{
public:
	unsigned peak_stddev_limit;
	frog_tones(listen_buf_t &input_buf,
	    frequency_buf_t &freq_buf,
	    unsigned peak_stddev_limit = default_peak_stddev_limit
	    )
		: audio_buffer(input_buf),
		freq_buffer(freq_buf),
		peak_stddev_limit(peak_stddev_limit)
	{
		tones.fill({ 0, 0 });
	}
	void find_peaks(void);
	virtual unsigned get_num_peaks(void);
	virtual struct bin_val get_peak_by_val(uint16_t peak_num);
	virtual struct bin_val get_peak_by_bin(uint16_t peak_num);
	virtual int as_Hz(uint16_t frequency_bin) const;

private:
	void sort_tones_by_bin(void);
	void sort_tones_by_value(void);
	listen_buf_t &audio_buffer;
	frequency_buf_t &freq_buffer;
	tone_array_t tones;
};

void find_tones(listen_buf_t &fft_output, uint16_t *tones);

