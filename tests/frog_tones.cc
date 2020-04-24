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

