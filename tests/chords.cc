/*
 * Tests for the chord algorithms
 * i.e. given a soundscape, what
 * should the next tone be

 * This test stinks.
 * What chord/tone in chord to play next is videly complex,
 * depends on previous tones too.
 * croak_reply is the place to put the AI into, and these tests
 * are therefore impossible / should be done on the triad/chord classes
 * instead of the croak_reply (which is more chord progression stuff)
 */
#include "gtest/gtest.h"
#include "croak_reply.h"
#include "datatype.h"


class ChordsTest : public testing::Test
{
public:
	croak_array_t croaks; // = {NOT_A_TONE}; - this would work if croak_array_t was an array, not an std::array...
	croak_reply *replies;
	void SetUp(void) override
	{
		croaks.fill(NOT_A_TONE);
		replies = new croak_reply(croaks);
	}
};

TEST_F(ChordsTest, Silence)
{
	// TODO: Disable spontaneous (seed) croaks
	enum note reply = replies->what_to_croak();
	EXPECT_EQ(reply, NOT_A_TONE);
}

TEST_F(ChordsTest, CMajorTriad3rd)
{
	// TODO: fix replies to the Major scale
	croaks[0] = C4;
	enum note reply = replies->what_to_croak();
	EXPECT_EQ(reply, E4);
}

TEST_F(ChordsTest, CMajorTriad5th)
{
	// TODO: fix replies to the Major scale
	croaks[0] = C4;
	croaks[1] = E4;
	enum note reply = replies->what_to_croak();
	EXPECT_EQ(reply, G4);
}

TEST_F(ChordsTest, CMajorTriad5thUnsorted)
{
	// TODO: fix replies to the Major scale
	croaks[3] = C4;
	croaks[2] = E4;
	enum note reply = replies->what_to_croak();
	EXPECT_EQ(reply, G4);
}

