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
	}
};

