#include "gmock/gmock.h"
#include "frog_tones.h"

class MockFrogTones : public frog_tones
{
public:
	MockFrogTones()
		: frog_tones(abuf, fbuf)
	{}
	MOCK_METHOD0(get_num_peaks, unsigned(void));
	MOCK_METHOD1(get_peak_by_val, struct bin_val (uint16_t));
	    MOCK_METHOD1(get_peak_by_bin, struct bin_val (uint16_t));
	        MOCK_CONST_METHOD1(as_Hz, int(uint16_t));

private:
	listen_buf_t abuf;
	frequency_buf_t fbuf;
};

