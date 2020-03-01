#include "gtest/gtest.h"
#include <cmath>

#include "fft.h"

class fft_test : public testing::Test
{
public:
	void SetUp(void) override
	{
		fs = 8000;
		fft_size = 256;
		in = (int16_t*)calloc(fft_size, sizeof(int16_t));
		out = (int16_t*)calloc(fft_size, sizeof(int16_t));

		/* How wide is half a bin, in Hz */
		bin_accuracy = ((float)fs / 2) / fft_size;
	}

	void add_sine(int peak, int f_input)
	{
		for (int i = 0; i < fft_size; i++) {
			in[i] += peak * sin(2 * M_PI * i * f_input / fs);
		}
	}

	int fs;
	int fft_size;
	int16_t *in;
	int16_t *out;
	float bin_accuracy;
};

TEST(fft, index_of_peak)
{
	int16_t buf[8];
	memset(buf, 0, sizeof(buf));

	buf[1] = 1;
	EXPECT_EQ(1, index_of_peak(buf, 8));

	buf[5] = 10;
	EXPECT_EQ(5, index_of_peak(buf, 8));
}

TEST(fft, indexToFrequency)
{
	int fs = 8000;
	int fft_size = 256;

	EXPECT_EQ(0, index_to_frequency(0, fs, fft_size) );
	EXPECT_EQ(4000, index_to_frequency(128, fs, fft_size) );
	EXPECT_EQ(500, index_to_frequency(16, fs, fft_size) );
	EXPECT_EQ(468, index_to_frequency(15, fs, fft_size) );
}

TEST_F(fft_test, one_sine)
{
	add_sine(
		256,  //amplitude
		500); //frequency

	frog_fft(in, out, fft_size);

	int max_i = index_of_peak(out, fft_size / 2);
	ASSERT_NEAR(
		500,
		index_to_frequency(max_i, fs, fft_size),
		bin_accuracy);
}

TEST_F(fft_test, many_sines)
{
	add_sine(
		100,  //amplitude
		500); //frequency
	add_sine(
		80,    //amplitude
		1000); //frequency
	add_sine(
		50,    //amplitude
		1200); //frequency

	frog_fft(in, out, fft_size);

	int max_i = index_of_peak(out, fft_size / 2);
	ASSERT_NEAR(
		500,
		index_to_frequency(max_i, fs, fft_size),
		bin_accuracy);
}

