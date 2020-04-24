#include "datatype.h"

class frog_tones
{
public:
	frog_tones(frequency_buf_t &buf)
		: buffer(buf)
	{}
	void find_peaks(void);
	unsigned get_num_peaks(void);
private:
	frequency_buf_t &buffer;
};

void find_tones(listen_buf_t &fft_output, uint16_t *tones);

