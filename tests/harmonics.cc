#include "gtest/gtest.h"
#include "harmonics.h"

class HarmonicsTest :
	public testing::Test
{
public:
	listen_buf_t in;
	std::array<uint16_t, 3> tones;

	void SetUp(void) override
	{
		in.fill(0);
		tones.fill(0);
	}
};

TEST_F(HarmonicsTest, allZeroes)
{
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 0);
	EXPECT_EQ(tones[1], 0);
	EXPECT_EQ(tones[2], 0);
}

TEST_F(HarmonicsTest, OneNonZero)
{
	in[5] = 100;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 5);
	EXPECT_EQ(tones[1], 0);
	EXPECT_EQ(tones[2], 0);
}

TEST_F(HarmonicsTest, TwoNonZero)
{
	in[5] = 100;
	in[8] = 80;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 5);
	EXPECT_EQ(tones[1], 8);
	EXPECT_EQ(tones[2], 0);
}

TEST_F(HarmonicsTest, ThreeNonZero)
{
	in[5] = 100;
	in[8] = 80;
	in[12] = 40;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 5);
	EXPECT_EQ(tones[1], 8);
	EXPECT_EQ(tones[2], 12);
}

TEST_F(HarmonicsTest, ManyNonZero)
{
	in[5] = 100;
	in[8] = 80;
	in[12] = 40;
	in[15] = 60;
	in[20] = 110;
	in[24] = 40;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 20);
	EXPECT_EQ(tones[1], 5);
	EXPECT_EQ(tones[2], 8);
}

