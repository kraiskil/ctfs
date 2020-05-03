#include "gtest/gtest.h"
#include "frog_tones.h"

class FrogTonesTest :
	public testing::Test
{
public:
	frequency_buf_t freq_buffer;
	listen_buf_t audio_buffer;
	frog_tones *ft;

	void SetUp(void) override
	{
		freq_buffer.fill(0);
		audio_buffer.fill(0);
		ft = new frog_tones(audio_buffer, freq_buffer);
		// TODO: for testing, should be the other way round:
		// we set config_fs_input here, and frog_tones reads that.
		fs = 48000; // =config_fs_input;
		bin_accuracy = ((float)fs) / audio_buffer.size();
	}

	void add_audio_sine(int peak, int f_input, float phase = 0.2)
	{
		for (int i = 0; i < audio_buffer.size(); i++) {
			audio_buffer[i] += peak * sin((2 * M_PI * i * f_input / fs) + phase);
		}
	}
	void add_audio_noise(uint16_t max_ampl)
	{
		for (auto &s : audio_buffer) {
			int16_t noise = rand() % (2 * max_ampl) - max_ampl;
			;
			s += noise;
		}
	}
	int fs;
	float bin_accuracy;
};

