#include "gtest/gtest.h"
#include <cmath>

#include "fft.h"


void add_sine(int16_t *in, int fft_size, int peak, int fs, int f_input)
{
	for (int i = 0; i < fft_size; i++) {
		in[i] += peak * sin(2 * M_PI * i * f_input / fs);
	}
}
float index_to_frequency(int idx, int fs, int fft_size)
{
	return (float)idx * fs / fft_size;
}

TEST(fft, index_of_peak)
{
	int16_t buf[8];
	memset(buf, 0, sizeof(buf));

	buf[1] = 1;
	EXPECT_EQ(1, index_of_peak(buf, 8));

	buf[5] = 10;
	EXPECT_EQ(5, index_of_peak(buf, 8));
}

TEST(fft, one_sine)
{
	int     fs = 8000;
	int     fft_size = 2048;
	int     peak_amplitude = 256;
	int     peak_fequency = 500;
	int16_t in[fft_size];
	int16_t out[fft_size];

	add_sine(
		in,
		fft_size,
		peak_amplitude,
		fs,
		peak_fequency);

	frog_fft(in, out, fft_size);

	int max_i = index_of_peak(out, fft_size);
	std::cout << "max_i: " << max_i << std::endl;
	ASSERT_NEAR(
		500,
		index_to_frequency(max_i, fs, fft_size),
		((float)fs / 2) / fft_size); // Frequency bin accuracy
}

