#include "config.h"
#include "datatype.h"
#include <cstdio>
extern "C" {
#include <libopencm3/stm32/spi.h>
}

/* TODO: this implementation of listen_for_croaks()
 * is for the SPH0645, which has a sampling frequency
 * fixed at 48000.
 * So build in a downsample-by-6
 */
#define SKIP_CHANNEL 1

static int16_t read_sample(void)
{
	uint8_t channel;
	int32_t data = i2s_read32(SPI2, &channel);
	if (channel == SKIP_CHANNEL)
		return read_sample();
	else
		// flush out the top '0's (since data is 24 bit)
		// and expect that >> is an arithmetic shift :)
		return (data << 8) >> 16;
}

static int16_t read_8k_sample(void)
{
	int32_t sum = 0;
	static_assert(config_fs_input == 8000, "wrong assumption of downsampling N here");
	for (int i = 0; i < 6; i++)
		sum += read_sample();
	return sum / 6;
}

void listen_for_croaks(listen_buf_t &buffer)
{

	unsigned i;
	/* Powerup is 50ms = 2400 dataframes */
	/* This is what the datasheet claims.
	 * Empirical studies show we need to wait somewhere between
	 * 100k-200k samples before the device's output
	 * bias has flattened enough so it can be removed.
	 * (and we do a post-processing bias removal by subtracting
	 *  sample averages - so that part should be near ideal)
	 * So just keep the I2S enabled - it will keep clock output
	 * on, and let the data overflow.
	 */

	for (i = 0; i < buffer.size(); i++) {
		buffer[i] = read_8k_sample();
	}
}

