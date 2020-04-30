#include "fft.h"
#include "frog_tones_test.h"
#include "whistle_sine.h"

#include <cmath>

class FrogTonesFreqTest : public FrogTonesTest
{
public:
	void SetUp(void) override
	{
		FrogTonesTest::SetUp();
		fs = 48000;
		srand(42);
		bin_accuracy = ((float)fs) / audio_buffer.size();
	}
	void add_audio_sine(int peak, int f_input, float phase = 0.2)
	{
		for (int i = 0; i < audio_buffer.size(); i++) {
			audio_buffer[i] += peak * sin((2 * M_PI * i * f_input / fs) + phase);
		}
	}
	void add_audio_noise(uint16_t max_ampl)
	{
		for (auto &s : audio_buffer) {
			int16_t noise = rand() % (2 * max_ampl) - max_ampl;
			;
			s += noise;
		}
	}

	float bin_accuracy;
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
	add_audio_noise(40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, ManySineWithNoise)
{
	add_audio_sine(200, 1000);
	add_audio_sine(100, 1300);
	add_audio_sine(150, 2800);
	add_audio_noise(40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, LoudSinesWithNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(40);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}
TEST_F(FrogTonesFreqTest, LoudSinesWithLoudNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(300);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, OnlyLoudNoise)
{
	add_audio_noise(400);
	ft->fft();
	ft->find_peaks();
	/* There might be peak detects at high frequencies, since
	 * the amplitude shifts are so abrupt. */
	EXPECT_LT(ft->get_num_peaks(), 10);
	EXPECT_GT(
		index_to_frequency(ft->get_peak_by_bin(0).bin, fs, audio_buffer.size()),
		8000);
}

TEST_F(FrogTonesFreqTest, WhistleSine)
{
	// There HAS to be a C++ way of doing this, but even with this
	// comment, this is quicker than figuring that way out :)
	for (int i = 0; i < audio_buffer.size(); i++)
		audio_buffer[i] = whistle_sine[i];
	ft->fft();
	ft->find_peaks();
	ASSERT_EQ(ft->get_num_peaks(), 2);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 0);
	// TODO: this value is not verified by measuring the sound frequency
	EXPECT_EQ(ft->get_peak_by_val(1).bin, 44);
}

TEST_F(FrogTonesFreqTest, ResetBetweenRuns)
{
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);

	audio_buffer.fill(0);
	srand(42);
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, asHz)
{
	add_audio_sine(500, 1200);
	ft->fft();
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_NEAR(ft->as_Hz(ft->get_peak_by_bin(0).bin), 1200, bin_accuracy);
}

