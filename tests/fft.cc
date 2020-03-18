#include "gtest/gtest.h"
#include <algorithm>
#include <cmath>
#include <complex>

#include "fft.h"

/* Internal FFT implementation - not the
 * "frog api" used in libtreefrog
 * complex_t is an internal formet to fft.cc so needs
 * redefinition here
 */
typedef std::complex<int32_t> complex_t;
extern void fft_sa(int n, complex_t *x);
extern void fft_calc_abs(complex_t *data, croak_buf_t &out);

struct bin_value
{
	int bin;
	int16_t value;
};

class fftTestBase
{
public:
	fftTestBase()
	{
		phase = 0.2;
	}

	void add_sine(int peak, int f_input)
	{
		for (int i = 0; i < fft_size; i++) {
			in[i] += peak * sin((2 * M_PI * i * f_input / fs) + phase);
		}
	}

	// biggest value first
	void sort_by_value(std::vector<bin_value> &values)
	{
		std::sort(values.begin(), values.end(),
		    [] (struct bin_value &a, struct bin_value &b)
		    {
			    return a.value > b.value;
		    }
		    );
	}
	void sort_by_bin(std::vector<bin_value> &values)
	{
		std::sort(values.begin(), values.end(),
		    [] (struct bin_value &a, struct bin_value &b)
		    {
			    return a.bin < b.bin;
		    }
		    );
	}
	std::vector<bin_value> max_values(int16_t *data, int n_ret)
	{
		int                    max_i = -1;
		double                 max_a = -1;
		std::vector<bin_value> values;
		for (int i = 0; i < fft_size / 2; i++) {
			struct bin_value b = { /*.bin=*/
				i, /*.value =*/ data[i]
			};
			values.push_back(b);
		}

		sort_by_value(values);

		std::vector<bin_value> rv(values.begin(), values.begin() + n_ret);
		return rv;
	}

	/* Merge neighbouring bins into one.
	 * I.e. if the spectrum of a tone is not sharp (timing erros, low fs, short fft size..)
	 * it spreads over several bins.
	 * This joins such bins into one (the one with highest value of the group) and gives
	 * the new bin_value a value that is the sum of its neighbours */
	std::vector<bin_value> merge_neighbours(const std::vector<bin_value> &data)
	{
		std::vector<bin_value> input = data;
		std::vector<bin_value> output;
		sort_by_bin(input);

		bool      prev_consecutive = false;
		double    merged_value = 0;
		bin_value max_merged = { 0, 0 };
		for (int i = 0; i < input.size() + 1; i++) {
			int       thisbin = input[i].bin;
			int       nextbin = input[i + 1].bin;
			double    thisval = input[i].value;
			bool      next_consecutive = (nextbin - thisbin == 1);
			bin_value current_binval;

			/* This is not a continuation from the previous one */
			if (prev_consecutive == false) {
				merged_value = 0;
				max_merged = input[i];
			}

			/* Merge this bin with current range */
			merged_value += thisval;
			if (thisval > max_merged.value)
				max_merged = input[i];


			/* If this is the last of the range */
			if (next_consecutive == false) {
				current_binval.bin = max_merged.bin;
				current_binval.value = merged_value;
				output.push_back(current_binval);
			}

			prev_consecutive = next_consecutive;
		}
		return output;
	}

	int fs;
	int fft_size;
	float phase;
	std::array<complex_t, MAX_FFT_SIZE> in;
	croak_buf_t out;
	float bin_accuracy;
};

class fftTest :
	public fftTestBase,
	public testing::TestWithParam<int32_t>
{
public:
	void SetUp(void) override
	{
		fs = GetParam();
		fft_size = 256;
		if (fs > 16000)
			fft_size = 512;
		if (fs > 44100)
			fft_size = 1024;
		in.fill(0);
		out.fill(0);
		/* Width of a bin, in Hz */
		bin_accuracy = ((float)fs) / fft_size;
	}
	void TearDown(void) override
	{}
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

TEST_P(fftTest, oneSine)
{
	add_sine(
		8,    //amplitude
		500); //frequency

	fft_sa(fft_size, in.data());
	fft_calc_abs(in.data(), out);

	int max_i = index_of_peak(out.data(), fft_size / 2);
	ASSERT_NEAR(
		500,
		index_to_frequency(max_i, fs, fft_size),
		bin_accuracy);
}

TEST_P(fftTest, manySines)
{
	add_sine(
		12,   //amplitude
		200); //frequency
	add_sine(
		20,   //amplitude
		900); //frequency
	add_sine(
		30,    //amplitude
		1400); //frequency

	fft_sa(fft_size, in.data());
	fft_calc_abs(in.data(), out);

	std::vector<bin_value> raw_bins = max_values(out.data(), 6);
	std::vector<bin_value> max_bins = merge_neighbours(raw_bins);
	sort_by_value(max_bins);

	/* The three sines in input should not be bin-merged */
	ASSERT_GE(max_bins.size(), 3);

	EXPECT_NEAR(
		1400,
		index_to_frequency(max_bins[0].bin, fs, fft_size),
		bin_accuracy);
	EXPECT_NEAR(
		900,
		index_to_frequency(max_bins[1].bin, fs, fft_size),
		bin_accuracy);
	EXPECT_NEAR(
		200,
		index_to_frequency(max_bins[2].bin, fs, fft_size),
		bin_accuracy);
}
INSTANTIATE_TEST_SUITE_P(Random_fs_values, fftTest, testing::Range(6000, 48000, 6100));
INSTANTIATE_TEST_SUITE_P(Probable_fs_values, fftTest, testing::Values(8000, 16000, 24000, 44100, 48000));


class specificFFTTest :
	public fftTestBase,
	public ::testing::Test
{
	void SetUp(void) override
	{}
};

TEST_F(specificFFTTest, saturatingFixedPoint)
{
	fs = 8000;
	fft_size = 2048;
	bin_accuracy = ((float)fs) / fft_size;

	add_sine(
		256,   //amplitude
		1000); //frequency

	fft_sa(fft_size, in.data());
	fft_calc_abs(in.data(), out);

	int max_i = index_of_peak(out.data(), fft_size / 2);
	ASSERT_NEAR(
		1000,
		index_to_frequency(max_i, fs, fft_size),
		bin_accuracy);
}

