#include "gtest/gtest.h"
#include "frog_tones.h"

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
	in[50] = 100;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 50);
	EXPECT_EQ(tones[1], 0);
	EXPECT_EQ(tones[2], 0);
}

TEST_F(HarmonicsTest, TwoNonZero)
{
	in[50] = 100;
	in[80] = 80;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 50);
	EXPECT_EQ(tones[1], 80);
	EXPECT_EQ(tones[2], 0);
}

TEST_F(HarmonicsTest, ThreeNonZero)
{
	in[50] = 100;
	in[80] = 80;
	in[120] = 40;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 50);
	EXPECT_EQ(tones[1], 80);
	EXPECT_EQ(tones[2], 120);
}

TEST_F(HarmonicsTest, ManyNonZero)
{
	in[50] = 100;
	in[80] = 80;
	in[120] = 40;
	in[150] = 60;
	in[200] = 110;
	in[24] = 40;
	find_tones(in, tones.data());
	EXPECT_EQ(tones[0], 200);
	EXPECT_EQ(tones[1], 50);
	EXPECT_EQ(tones[2], 80);
}

