#include "frog_tones_test.h"

class FrogTonesPeakTest : public FrogTonesTest
{
public:
	void SetUp(void) override
	{
		FrogTonesTest::SetUp();
		srand(42);
		audio_buffer.fill(1);
	}
	void add_freq_noise(uint16_t min_ampl, uint16_t max_ampl)
	{
		int range = max_ampl - min_ampl + 1; // limits are inclusive
		for (auto &s : freq_buffer) {
			uint16_t noise = rand() % range + min_ampl;
			s += noise;
		}
	}
};

TEST_F(FrogTonesPeakTest, NoPeaksInZeroData)
{
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

TEST_F(FrogTonesPeakTest, NoPeaksInNoise)
{
	add_freq_noise(0, 10);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

TEST_F(FrogTonesPeakTest, NoPeaksInLoudNoise)
{
	add_freq_noise(100, 110);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

/* These fail probably because of adjacent peak removal is missing.
 * check with debugger */
TEST_F(FrogTonesPeakTest, SinglePeak)
{
	freq_buffer[50] = 100;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 50);
}
TEST_F(FrogTonesPeakTest, SinglePeakInNoise)
{
	add_freq_noise(30, 40);
	freq_buffer[50] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}
TEST_F(FrogTonesPeakTest, ManyPeaksInNoise)
{
	add_freq_noise(30, 40);
	freq_buffer[50] = 150;
	freq_buffer[100] = 130;
	freq_buffer[150] = 110;
	freq_buffer[200] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 50);
	EXPECT_EQ(ft->get_peak_by_val(1).bin, 100);
	EXPECT_EQ(ft->get_peak_by_val(2).bin, 150);
	EXPECT_EQ(ft->get_peak_by_val(3).bin, 200);
}
TEST_F(FrogTonesPeakTest, LowPeaksInNoise)
{
	add_freq_noise(30, 40);
	freq_buffer[50] = 50;
	freq_buffer[100] = 50;
	freq_buffer[150] = 50;
	freq_buffer[200] = 50;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}

TEST_F(FrogTonesPeakTest, WidePeaksWithoutNoise)
{
	freq_buffer[50] = 80;
	freq_buffer[100] = 80;
	freq_buffer[101] = 80;
	freq_buffer[151] = 80;
	freq_buffer[152] = 80;
	freq_buffer[153] = 80;
	freq_buffer[154] = 80;
	freq_buffer[200] = 80;
	freq_buffer[201] = 80;
	freq_buffer[202] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}

TEST_F(FrogTonesPeakTest, WidePeaksInNoise)
{
	add_freq_noise(10, 20);
	freq_buffer[50] = 80;
	freq_buffer[100] = 80;
	freq_buffer[101] = 80;
	freq_buffer[151] = 80;
	freq_buffer[152] = 80;
	freq_buffer[153] = 80;
	freq_buffer[154] = 80;
	freq_buffer[200] = 80;
	freq_buffer[201] = 80;
	freq_buffer[202] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}




extern unsigned s1(frequency_buf_t &freq_buffer, unsigned i, unsigned k);
TEST_F(FrogTonesPeakTest, S1)
{
	freq_buffer[0] = 10;
	freq_buffer[6] = 10;
	/* The value at the edges doesn't really make sense here,
	 * but then neither edge will contain anything interesting
	 * anyways, from a frog's perspective */
	EXPECT_EQ(s1(freq_buffer, 0, 3), 5);
	EXPECT_EQ(s1(freq_buffer, 3, 3), 0);
	EXPECT_EQ(s1(freq_buffer, 5, 2), 0);
	EXPECT_EQ(s1(freq_buffer, 6, 2), 10);

	int end = freq_buffer.size() - 1;
	freq_buffer[end] = 10;
	freq_buffer[end - 6] = 10;
	EXPECT_EQ(s1(freq_buffer, end, 3), 5);
	EXPECT_EQ(s1(freq_buffer, end - 3, 3), 0);
	EXPECT_EQ(s1(freq_buffer, end - 5, 2), 0);
	EXPECT_EQ(s1(freq_buffer, end - 6, 2), 10);
}

extern float calculate_stddev(frequency_buf_t&, uint32_t mean);
TEST_F(FrogTonesTest, CalculateStddev)
{
	EXPECT_FLOAT_EQ(calculate_stddev(freq_buffer, 0), 0);

	freq_buffer.fill(4);
	EXPECT_FLOAT_EQ(calculate_stddev(freq_buffer, 4), 0);

	/* results from octave - the mean is not accurate!*/
	freq_buffer[0] = 100;
	EXPECT_NEAR(calculate_stddev(freq_buffer, 4), 3, 0.1);

	freq_buffer[1] = 100;
	freq_buffer[2] = 100;
	freq_buffer[3] = 100;
	EXPECT_NEAR(calculate_stddev(freq_buffer, 4), 5.9912, 0.1);
}

