#include "gtest/gtest.h"
#include "datatype.h"
#include <algorithm>
#include <cmath>
#include <complex>

#include "fft.h"

class TestFFT : public testing::TestWithParam<int32_t>
{
public:
	unsigned fs;
	unsigned fft_size;
	unsigned fft_scale;
	fft<int32_t> int32_fft;
	fft<float> float_fft;
	listen_buf_t input;
	frequency_buf_t output;

	void SetUp(void) override
	{
		input.fill(0);
		output.fill(0);
		fs = GetParam();
		fft_size = input.size();
		fft_scale = 1;

		int32_fft.fs = fs;
		int32_fft.fft_size = fft_size;
		float_fft.fs = fs;
		float_fft.fft_size = fft_size;
		int32_fft.scale = fft_scale;
		float_fft.scale = fft_scale;
	}
};

TEST_P(TestFFT, StepInputFloat)
{
	//Big enough, as fft output is cast to ints
	uint16_t step_value = 100;
	for (int i = fft_size / 2; i < fft_size; i++)
		input[i] = step_value;

	float_fft.run(input, output);

	/* Output shape should be a sinc */
	EXPECT_FLOAT_EQ(output[0] / fft_scale, step_value / 2);
	EXPECT_FLOAT_EQ(output[2], 0);
	EXPECT_FLOAT_EQ(output[4], 0);
	EXPECT_FLOAT_EQ(output[6], 0);

	EXPECT_GT(output[1], output[3]);
	EXPECT_GT(output[3], output[5]);
	EXPECT_GT(output[5], output[7]);
}
TEST_P(TestFFT, StepInputInt32)
{
	//Big enough, as fft output is cast to ints
	uint16_t step_value = 100;
	for (int i = fft_size / 2; i < fft_size; i++)
		input[i] = step_value;

	int32_fft.run(input, output);

	/* Output shape should be a sinc */
	/* NB: output[0] check is missing - it gets normalized, so absolute value is unknonw */
	EXPECT_EQ(output[2], 0);
	EXPECT_EQ(output[4], 0);
	EXPECT_EQ(output[6], 0);

	EXPECT_GT(output[1], output[3]);
	EXPECT_GT(output[3], output[5]);
	//The resolution doesn't seem to be enough
	//If the /N is removed from the implementation, then this should
	//fail, as [5] should be > [7]!
	EXPECT_EQ(output[5], output[7]);
}

TEST_P(TestFFT, Normalization)
{
	// Big enough for normalization to kick in.
	uint16_t step_value = 30000;
	for (int i = fft_size / 2; i < fft_size; i++)
		input[i] = step_value;

	int32_fft.run(input, output);

	/* Output shape should be a sinc */
	/* NB: output[0] check is missing - it gets normalized, so absolute value is unknonw */
	EXPECT_EQ(output[2], 0);
	EXPECT_EQ(output[4], 0);
	EXPECT_EQ(output[6], 0);

	EXPECT_GT(output[1], output[3]);
	EXPECT_GT(output[3], output[5]);
	//The resolution doesn't seem to be enough
	//If the /N is removed from the implementation, then this should
	//fail, as [5] should be > [7]!
	EXPECT_EQ(output[5], output[7]);
}

INSTANTIATE_TEST_SUITE_P(Probable_fs_values, TestFFT, testing::Values(8000, 16000, 48000));

