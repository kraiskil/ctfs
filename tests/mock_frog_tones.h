#include "gmock/gmock.h"
#include "peak_detect.h"

class MockPeakDetect : public peak_detect
{
public:
	MockPeakDetect()
		: peak_detect(fbuf, pbuf)
	{}
	MOCK_METHOD0(get_num_peaks, unsigned(void));
	MOCK_METHOD1(get_peak_by_val, struct bin_val (uint16_t));
	    MOCK_METHOD1(get_peak_by_bin, struct bin_val (uint16_t));
	        MOCK_CONST_METHOD1(bin_frequency, frequency_t(uint16_t));

private:
	listen_buf_t abuf;
	frequency_buf_t fbuf;
	peak_array_t pbuf;
};

