#include "frog_tones_test.h"

#include <cmath>

class FrogTonesFreqTest : public FrogTonesTest
{
public:
	void SetUp(void) override
	{
		FrogTonesTest::SetUp();
		fs = 48000;
	}
	void add_audio_sine(int peak, int f_input, float phase = 0.2)
	{
		for (int i = 0; i < audio_buffer.size(); i++) {
			audio_buffer[i] += peak * sin((2 * M_PI * i * f_input / fs) + phase);
		}
	}

	int fs;

};

TEST_F(FrogTonesFreqTest, fft)
{
	add_audio_sine(200, 1000);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

