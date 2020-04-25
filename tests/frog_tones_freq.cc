#include "frog_tones_test.h"

#include <cmath>

class FrogTonesFreqTest : public FrogTonesTest
{
public:
	void SetUp(void) override
	{
		FrogTonesTest::SetUp();
		fs = 48000;
		srand(42);
	}
	void add_audio_sine(int peak, int f_input, float phase = 0.2)
	{
		for (int i = 0; i < audio_buffer.size(); i++) {
			audio_buffer[i] += peak * sin((2 * M_PI * i * f_input / fs) + phase);
		}
	}
	void add_audio_noise(uint16_t min_ampl, uint16_t max_ampl)
	{
		int range = max_ampl - min_ampl + 1; // limits are inclusive
		for (auto &s : audio_buffer) {
			uint16_t noise = rand() % range + min_ampl;
			s += noise;
		}
	}

	int fs;

};

TEST_F(FrogTonesFreqTest, SingleSine)
{
	add_audio_sine(200, 1000);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, TwoSines)
{
	add_audio_sine(200, 1000);
	add_audio_sine(200, 1500);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 2);
}

TEST_F(FrogTonesFreqTest, SingleSineWithNoise)
{
	add_audio_sine(200, 1000);
	add_audio_noise(30, 40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, ManySineWithNoise)
{
	add_audio_sine(200, 1000);
	add_audio_sine(100, 1300);
	add_audio_sine(150, 2800);
	add_audio_noise(30, 40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, LoudSinesWithNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(30, 40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}
TEST_F(FrogTonesFreqTest, LoudSinesWithLoudNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(100, 300);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, OnlyLoudNoise)
{
	add_audio_noise(0, 400);
	ft->fft();
	ft->find_peaks();
	/* There *might* be a peak at DC, which is OK - we'd ignore that
	 * anyways */
	ASSERT_LT(ft->get_num_peaks(), 2);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 0);
}

