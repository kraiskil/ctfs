#include "frog_tones_test.h"
#include "harmonics.h"
#include "mock_frog_tones.h"

using namespace testing;

class HarmonicsTest : public testing::Test
{
public:
	void SetUp(void) override
	{}

	void add_num_results(int nr)
	{
		EXPECT_CALL(mock_pd, get_peak_by_bin(Ge(nr)))
		        .Times(0);
		ON_CALL(mock_pd, get_num_peaks())
		        .WillByDefault(Return(nr));

		for (int i = 0; i < nr; i++) {
			EXPECT_CALL(mock_pd, get_peak_by_bin(i))
			        .WillRepeatedly(Return(retvals[i]));
			EXPECT_CALL(mock_pd, as_Hz(retvals[i].bin)).
			        WillRepeatedly(Return(retvals[i].bin * bin_to_hz));
		}
	}

	const int bin_to_hz = 10; // converto bin to Hz by multiplying with this
	struct bin_val retvals[10] =
	{ {
		  100, 10
	  },           //1st frog base
	  { 200, 10 }, //1st frog harmonic
	  { 150, 10 }, //2nd frog base
	  { 300, 10 }, //2nd frog harmonic
	  { 180, 10 }, //3rd frog base
	  { 360, 10 }, //3rd frog harmonic
	  { 130, 10 }, //4th frog base
	  { 260, 10 }, //4th frog harmonic
	};
	uint16_t harmonics[3] = { 42, 42, 42 };

	NiceMock<MockPeakDetect> mock_pd;
};

TEST_F(HarmonicsTest, NoSounds)
{
	add_num_results(0);

	find_harmonics(mock_pd, harmonics);

	EXPECT_EQ(harmonics[0], 0);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}


TEST_F(HarmonicsTest, DetectFrogSound)
{
	add_num_results(2);

	find_harmonics(mock_pd, harmonics);

	EXPECT_EQ(harmonics[0], 1000);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(HarmonicsTest, NoDetectFrogSound)
{
	// bins 10 & 25 - this is not how frogs sound!
	retvals[1].bin = 25;
	add_num_results(2);

	find_harmonics(mock_pd, harmonics);

	EXPECT_EQ(harmonics[0], 0);
	EXPECT_EQ(harmonics[1], 0);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(HarmonicsTest, DetectTwoFrogSound)
{
	add_num_results(4);

	find_harmonics(mock_pd, harmonics);

	EXPECT_EQ(harmonics[0], 1000);
	EXPECT_EQ(harmonics[1], 1500);
	EXPECT_EQ(harmonics[2], 0);
}

TEST_F(HarmonicsTest, DetectFromFourFrogSounds)
{
	add_num_results(8);

	find_harmonics(mock_pd, harmonics);

	EXPECT_EQ(harmonics[0], 1000);
	EXPECT_EQ(harmonics[1], 1500);
	EXPECT_EQ(harmonics[2], 1800);
}

