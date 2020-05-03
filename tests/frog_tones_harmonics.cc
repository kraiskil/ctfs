#include "frog_tones_test.h"
#include "harmonics.h"

using namespace testing;

class FrogTonesHarmonicsTest : public FrogTonesTest
{
public:
	void add_frog_sound(uint16_t ampl, uint16_t base_freq)
	{
		add_audio_sine(ampl, base_freq);
		add_audio_sine(ampl, 2 * base_freq);
	}

	uint16_t harmonics[3] = { 42, 42, 42 };
};


TEST_F(FrogTonesHarmonicsTest, SingleCroak)
{
	// Add the sound of a single frog in the forest.
	// (Does this exist, if nobody is there to hear it?)
	add_frog_sound(200, 1000);

	ft->fft();
	ft->find_peaks();

	find_harmonics(*ft, harmonics);
	EXPECT_EQ(ft->get_num_peaks(), 2);
	EXPECT_NEAR(ft->get_peak_by_bin(0).bin * 2, ft->get_peak_by_bin(1).bin, 1);

	EXPECT_NEAR(harmonics[0], 1000, bin_accuracy);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(FrogTonesHarmonicsTest, SingleCroakAndNoise)
{
	add_frog_sound(200, 1000);
	add_audio_noise(200);

	ft->fft();
	ft->find_peaks();
	find_harmonics(*ft, harmonics);

	EXPECT_NEAR(harmonics[0], 1000, bin_accuracy);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(FrogTonesHarmonicsTest, NotACroak)
{
	// Not a frog
	add_audio_sine(200, 1000);
	add_audio_sine(200, 2300);

	ft->fft();
	ft->find_peaks();
	find_harmonics(*ft, harmonics);

	EXPECT_EQ(harmonics[0], 0);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(FrogTonesHarmonicsTest, FrogInA4)
{
	add_frog_sound(200, 440);

	ft->fft();
	ft->find_peaks();
	find_harmonics(*ft, harmonics);

	EXPECT_NEAR(harmonics[0], 440, bin_accuracy);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(FrogTonesHarmonicsTest, FourArtificailTreeFrogs)
{
	// 4 frogs with enough spatial resolution
	add_frog_sound(200, 400);
	add_frog_sound(200, 700);
	add_frog_sound(200, 1200);
	add_frog_sound(200, 1500);

	ft->fft();
	ft->find_peaks();
	find_harmonics(*ft, harmonics);

	EXPECT_NEAR(harmonics[0], 400, bin_accuracy);
	EXPECT_NEAR(harmonics[1], 700, bin_accuracy);
	EXPECT_NEAR(harmonics[2], 1200, bin_accuracy);
}

