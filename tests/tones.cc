#include "gtest/gtest.h"
#include "tones.h"

using namespace testing;

class TonesTest : public testing::Test
{
public:
	tones *t;
	peak_array_t p;


	void SetUp(void) override
	{
		t = new tones(p);
		p.fill({ 0, 0 });
		pai = 0;
	}

	//TODO: croaks may change - this is what a croak
	// looks like at the time of writing this.
	void add_croak_peak(enum tone base)
	{
		frequency_t f = tone_freq[base];
		p[pai] = { f, 100 };
		p[pai + 1] = { (float)f * 2, 100 };
		pai += 2;
		t->detect_tones();
	}
	void add_not_croak_peaks(enum tone base)
	{
		frequency_t f = tone_freq[base];
		p[pai] = { f, 100 };
		p[pai] = { (float)f * 1.4, 100 };
		pai += 2;
		t->detect_tones();
	}
private:
	unsigned pai; // peak-array-index

};


TEST_F(TonesTest, BasicToneFrequencies)
{
	EXPECT_EQ(t->find_tone(220), A3);
	EXPECT_EQ(t->find_tone(440), A4);
	EXPECT_EQ(t->find_tone(880), A5);
}

TEST_F(TonesTest, OutOfPitchTones)
{
	EXPECT_EQ(t->find_tone(222), NOT_A_TONE);
}

TEST_F(TonesTest, IsCroakEmptyPeaks)
{
	EXPECT_FALSE(t->has_croak());
}

TEST_F(TonesTest, IsCroakOnePeak)
{
	p[0] = { 220, 50 };
	EXPECT_FALSE(t->has_croak());
}

TEST_F(TonesTest, FindsACroak)
{
	add_croak_peak(A3);
	EXPECT_TRUE(t->has_croak());
}

TEST_F(TonesTest, FindsTwoCroaks)
{
	add_croak_peak(A3);
	add_croak_peak(C4);
	EXPECT_TRUE(t->has_croak());
}

TEST_F(TonesTest, TwoPeaksNotACroak)
{
	add_not_croak_peaks(A3);
	EXPECT_FALSE(t->has_croak());
}


/* TODO:
 * These tests assume the frog replies with a tone
 * in the 'base' major chord:
 * I.e. if always base-major_3rd-perfect_5th */
TEST_F(TonesTest, FirstHarmonicC4)
{
	add_croak_peak(C4);
	ASSERT_TRUE(t->has_croak());
	EXPECT_EQ(t->what_to_croak(), E4);
}
TEST_F(TonesTest, FirstHarmonicE4)
{
	add_croak_peak(E4);
	ASSERT_TRUE(t->has_croak());
	EXPECT_EQ(t->what_to_croak(), Gs4);
}
TEST_F(TonesTest, SecondHarmonicC4)
{
	add_croak_peak(C4);
	add_croak_peak(E4);
	ASSERT_TRUE(t->has_croak());
	EXPECT_EQ(t->what_to_croak(), G4);
}
TEST_F(TonesTest, SecondHarmonicE4)
{
	add_croak_peak(E4);
	add_croak_peak(Gs4);
	ASSERT_TRUE(t->has_croak());
	EXPECT_EQ(t->what_to_croak(), B4);
}
TEST_F(TonesTest, NoHarmonicC4)
{
	add_croak_peak(C4);
	add_croak_peak(D4);
	EXPECT_EQ(t->what_to_croak(), NOT_A_TONE);
}

