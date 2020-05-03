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
		return data >> 16;
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
	i2s_enable(SPI2);

	unsigned i;
	/* Powerup is 50ms = 2400 dataframes */
	for (i = 0; i < 4800; i++) {
		uint8_t  channel;
		uint32_t flushbuffer = i2s_read32(SPI2, &channel);
		(void)flushbuffer;
	}

	for (i = 0; i < buffer.size(); i++) {
		buffer[i] = read_8k_sample();
	}
	i2s_disable(SPI2);
}

