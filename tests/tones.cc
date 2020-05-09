#include "gtest/gtest.h"
#include "tones.h"

using namespace testing;

class TonesTest : public testing::Test
{
public:
	void SetUp(void) override
	{}

};


TEST_F(TonesTest, BasicToneFrequencies)
{
	EXPECT_EQ(find_tone(220), A3);
	EXPECT_EQ(find_tone(440), A4);
	EXPECT_EQ(find_tone(880), A5);
}

TEST_F(TonesTest, OutOfPitchTones)
{
	EXPECT_EQ(find_tone(222), NOT_A_TONE);
}

