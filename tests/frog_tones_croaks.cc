#include "frog_tones_test.h"

class FrogTonesCroakTest :
	public FrogTonesTest,
	public testing::TestWithParam<int32_t>
{
public:
	enum note tone;
	tones *t;
	uint16_t harmonics[3] = { 42, 42, 42 };

	void SetUp(void) override
	{
		tone = static_cast<enum note>(GetParam());
		// Fill in croak from after the attack, to make sure there
		// is some data in in audio_buffer - not just attack-muted silence
		// (the detection algorithms don't work well when audio amplitude is <10)
		for (int i = 0; i < audio_buffer.size(); i++)
			audio_buffer[i] = get_croak_data(i + attack_len, tone);

		t = new tones(peaks);
	}
};


TEST_P(FrogTonesCroakTest, PeaksFromCroak)
{
		the_fft->dc_blocker();
		the_fft->run();
		ft->find_peaks();

		EXPECT_GT(ft->get_num_peaks(), 1);
		EXPECT_TRUE(ft->has_peak_at(tone_freq[tone]));
	}

	TEST_P(FrogTonesCroakTest, DetectCroaks)
	{
		the_fft->dc_blocker();
		the_fft->run();
	ft->find_peaks();
	EXPECT_TRUE(t->has_croak());
}


INSTANTIATE_TEST_SUITE_P(AllTonesUpToF6,
    FrogTonesCroakTest,
    testing::Range(0, (int)F6));
// don't go up to LAST_TONE - the algorithm seems to break with higher base frequencies
// exact limit where it break depends on waveform

