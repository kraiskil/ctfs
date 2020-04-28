#include "datatype.h"

struct bin_val
{
	uint16_t bin;
	uint16_t val;
};
typedef std::array<struct bin_val, frequency_buffer_samples> tone_array_t;


class frog_tones
{
public:
	frog_tones(listen_buf_t &input_buf, frequency_buf_t &freq_buf)
		: audio_buffer(input_buf), freq_buffer(freq_buf)
	{
		tones.fill({ 0, 0 });
	}
	void fft(void);
	void find_peaks(void);
	unsigned get_num_peaks(void);
	struct bin_val get_peak_by_val(uint16_t peak_num);
	struct bin_val get_peak_by_bin(uint16_t peak_num);

private:
	void sort_tones_by_bin(void);
	void sort_tones_by_value(void);
	listen_buf_t &audio_buffer;
	frequency_buf_t &freq_buffer;
	tone_array_t tones;
};

void find_tones(listen_buf_t &fft_output, uint16_t *tones);

