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
	frog_tones(frequency_buf_t &buf)
		: buffer(buf)
	{
		tones.fill({ 0, 0 });
	}
	void find_peaks(void);
	unsigned get_num_peaks(void);
	struct bin_val get_peak_by_val(uint16_t peak_num);
private:
	void sort_tones_by_bin(void);
	void sort_tones_by_value(void);
	frequency_buf_t &buffer;
	tone_array_t tones;
};

void find_tones(listen_buf_t &fft_output, uint16_t *tones);

