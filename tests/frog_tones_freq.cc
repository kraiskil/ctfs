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
		srand(42);
	}
	int index_to_frequency(int idx, int fs, int fft_size)
	{
		/* TODO: remove - use the fft one */
		return (float)idx * fs / fft_size;
	}
};

TEST_F(FrogTonesFreqTest, SingleSine)
{
	add_audio_sine(200, 1000);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, TwoSines)
{
	add_audio_sine(200, 1000);
	add_audio_sine(200, 1500);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 2);
}

TEST_F(FrogTonesFreqTest, SingleSineWithNoise)
{
	add_audio_sine(200, 1000);
	add_audio_noise(40);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, ManySineWithNoise)
{
	add_audio_sine(200, 1000);
	add_audio_sine(100, 1300);
	add_audio_sine(150, 2800);
	add_audio_noise(40);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, LoudSinesWithNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(40);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}
TEST_F(FrogTonesFreqTest, LoudSinesWithLoudNoise)
{
	add_audio_sine(1200, 1000);
	add_audio_sine(1100, 1300);
	add_audio_sine(1150, 2800);
	add_audio_noise(300);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 3);
}

TEST_F(FrogTonesFreqTest, OnlyLoudNoise)
{
	add_audio_noise(400);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	/* There might be peak detects at high frequencies, since
	 * the amplitude shifts are so abrupt. */
	/* TODO: Not sure if this test makes any sense, after input sampling
	 * frequency was dropped from 48k to 8k. Not sure how natural this
	 * kind of noise is...*/
	EXPECT_LT(ft->get_num_peaks(), 10);
	EXPECT_GT(
		index_to_frequency(ft->get_peak_by_bin(0).bin, fs, audio_buffer.size()),
		1333);
}

TEST_F(FrogTonesFreqTest, WhistleSine)
{
	// There HAS to be a C++ way of doing this, but even with this
	// comment, this is quicker than figuring that way out :)
	for (int i = 0; i < audio_buffer.size(); i++)
		audio_buffer[i] = whistle_sine[i];
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	ASSERT_EQ(ft->get_num_peaks(), 1);
	// TODO: this value is not verified by measuring the sound frequency
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 44);
}

TEST_F(FrogTonesFreqTest, ResetBetweenRuns)
{
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);

	audio_buffer.fill(0);
	srand(42);
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}

TEST_F(FrogTonesFreqTest, asHz)
{
	add_audio_sine(500, 1200);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_NEAR(ft->as_Hz(ft->get_peak_by_bin(0).bin), 1200, bin_accuracy);
}

TEST_F(FrogTonesFreqTest, DcBlocker)
{
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	for (auto &v: audio_buffer)
		v += 800;

	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_GT(freq_buffer[0], 790); // almost 800
	// peak find does not find the DC peak -> get the bin of the 500Hz signal
	int prev_peak = ft->get_peak_by_bin(0).bin;

	the_fft.dc_blocker(audio_buffer);
	the_fft.run(audio_buffer, freq_buffer);
	EXPECT_EQ(freq_buffer[0], 0);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, prev_peak);
}

TEST_F(FrogTonesFreqTest, NegativeDcBlocker)
{
	add_audio_noise(50);
	add_audio_sine(1200, 500);
	for (auto &v: audio_buffer)
		v += -800;

	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_GT(freq_buffer[0], 790); // almost 800
	// peak find does not find the DC peak -> get the bin of the 500Hz signal
	int prev_peak = ft->get_peak_by_bin(0).bin;

	the_fft.dc_blocker(audio_buffer);
	the_fft.run(audio_buffer, freq_buffer);
	EXPECT_EQ(freq_buffer[0], 0);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, prev_peak);
}

/* SPH0645 has a "huge" DC - larder than "normal" sound levels.
 * These values approximate observed recordings */
TEST_F(FrogTonesFreqTest, LargeNoisyDcIsBlocked)
{
	add_audio_noise(50);
	add_audio_sine(3000, 1200);
	for (auto &v: audio_buffer)
		v += 1800;

	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();
	EXPECT_GT(freq_buffer[0], 1790); // almost 1800
	// peak detection ignores DC bin internally - there is only one peak visible here
	EXPECT_NEAR(ft->as_Hz(ft->get_peak_by_bin(0).bin), 1200, bin_accuracy);

	the_fft.dc_blocker(audio_buffer);
	the_fft.run(audio_buffer, freq_buffer);
	EXPECT_EQ(freq_buffer[0], 0);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_NEAR(ft->as_Hz(ft->get_peak_by_val(0).bin), 1200, bin_accuracy);
}

