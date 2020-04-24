#include "gtest/gtest.h"
#include "frog_tones.h"

class FrogTonesTest :
	public testing::Test
{
public:
	frequency_buf_t buffer;
	frog_tones *ft;

	void SetUp(void) override
	{
		buffer.fill(0);
		ft = new frog_tones(buffer);
		srand(42);
	}

	void add_noise(uint16_t min_ampl, uint16_t max_ampl)
	{
		int range = max_ampl - min_ampl + 1; // limits are inclusive
		for (auto &s : buffer) {
			uint16_t noise = rand() % range + min_ampl;
			s += noise;
		}
	}
};

TEST_F(FrogTonesTest, NoPeaksInZeroData)
{
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

TEST_F(FrogTonesTest, NoPeaksInNoise)
{
	add_noise(0, 10);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

TEST_F(FrogTonesTest, NoPeaksInLoudNoise)
{
	add_noise(100, 110);
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 0);
}

/* These fail probably because of adjacent peak removal is missing.
 * check with debugger */
TEST_F(FrogTonesTest, SinglePeak)
{
	buffer[50] = 100;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 50);
}
TEST_F(FrogTonesTest, SinglePeakInNoise)
{
	add_noise(30, 40);
	buffer[50] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 1);
}
TEST_F(FrogTonesTest, ManyPeaksInNoise)
{
	add_noise(30, 40);
	buffer[50] = 150;
	buffer[100] = 130;
	buffer[150] = 110;
	buffer[200] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
	EXPECT_EQ(ft->get_peak_by_val(0).bin, 50);
	EXPECT_EQ(ft->get_peak_by_val(1).bin, 100);
	EXPECT_EQ(ft->get_peak_by_val(2).bin, 150);
	EXPECT_EQ(ft->get_peak_by_val(3).bin, 200);
}
TEST_F(FrogTonesTest, LowPeaksInNoise)
{
	add_noise(30, 40);
	buffer[50] = 50;
	buffer[100] = 50;
	buffer[150] = 50;
	buffer[200] = 50;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}

TEST_F(FrogTonesTest, WidePeaksWithoutNoise)
{
	buffer[50] = 80;
	buffer[100] = 80;
	buffer[101] = 80;
	buffer[151] = 80;
	buffer[152] = 80;
	buffer[153] = 80;
	buffer[154] = 80;
	buffer[200] = 80;
	buffer[201] = 80;
	buffer[202] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}

TEST_F(FrogTonesTest, WidePeaksInNoise)
{
	add_noise(10, 20);
	buffer[50] = 80;
	buffer[100] = 80;
	buffer[101] = 80;
	buffer[151] = 80;
	buffer[152] = 80;
	buffer[153] = 80;
	buffer[154] = 80;
	buffer[200] = 80;
	buffer[201] = 80;
	buffer[202] = 80;
	ft->find_peaks();
	EXPECT_EQ(ft->get_num_peaks(), 4);
}




extern unsigned s1(frequency_buf_t &buffer, unsigned i, unsigned k);
TEST_F(FrogTonesTest, S1)
{
	buffer[0] = 10;
	buffer[6] = 10;
	/* The value at the edges doesn't really make sense here,
	 * but then neither edge will contain anything interesting
	 * anyways, from a frog's perspective */
	EXPECT_EQ(s1(buffer, 0, 3), 5);
	EXPECT_EQ(s1(buffer, 3, 3), 0);
	EXPECT_EQ(s1(buffer, 5, 2), 0);
	EXPECT_EQ(s1(buffer, 6, 2), 10);

	int end = buffer.size() - 1;
	buffer[end] = 10;
	buffer[end - 6] = 10;
	EXPECT_EQ(s1(buffer, end, 3), 5);
	EXPECT_EQ(s1(buffer, end - 3, 3), 0);
	EXPECT_EQ(s1(buffer, end - 5, 2), 0);
	EXPECT_EQ(s1(buffer, end - 6, 2), 10);
}

extern float calculate_stddev(frequency_buf_t&, uint32_t mean);
TEST_F(FrogTonesTest, CalculateStddev)
{
	EXPECT_FLOAT_EQ(calculate_stddev(buffer, 0), 0);

	buffer.fill(4);
	EXPECT_FLOAT_EQ(calculate_stddev(buffer, 4), 0);

	/* results from octave - the mean is not accurate!*/
	buffer[0] = 100;
	EXPECT_NEAR(calculate_stddev(buffer, 4), 3, 0.1);

	buffer[1] = 100;
	buffer[2] = 100;
	buffer[3] = 100;
	EXPECT_NEAR(calculate_stddev(buffer, 4), 5.9912, 0.1);
}

