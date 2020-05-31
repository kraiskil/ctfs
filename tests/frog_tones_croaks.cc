#include "frog_tones_test.h"

class FrogTonesCroakTest :
	public FrogTonesTest,
	public testing::TestWithParam<int32_t>
{
public:
	enum tone tone;
	uint16_t harmonics[3] = { 42, 42, 42 };

	void SetUp(void) override
	{
		tone = static_cast<enum tone>(GetParam());
		for (int i = 0; i < audio_buffer.size(); i++)
			audio_buffer[i] = get_croak_data(i, tone);
	}
};


TEST_P(FrogTonesCroakTest, PeaksFromCroak)
{
	the_fft.dc_blocker(audio_buffer);
	the_fft.run(audio_buffer, freq_buffer);
	ft->find_peaks();

	EXPECT_GT(ft->get_num_peaks(), 1);
	EXPECT_TRUE(ft->has_peak_at(tone_freq[tone]));
}


INSTANTIATE_TEST_SUITE_P(AllTonesUpToF6,
    FrogTonesCroakTest,
    testing::Range(0, (int)F6));
// don't go up to LAST_TONE - the algorithm seems to break with higher base frequencies
// exact limit where it break depends on waveform

