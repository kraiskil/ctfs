#include "datatype.h"
#include <cstdio>
extern "C" {
#include <libopencm3/stm32/spi.h>
}

#define SKIP_CHANNEL 1

void listen_for_croaks(listen_buf_t &buffer)
{
	int16_t max = 0;
	uint8_t channel;
	i2s_enable(SPI2);

	unsigned i;
	/* Powerup is 50ms = 2400 dataframes */
	for (i = 0; i < 4800; i++) {
		uint32_t flushbuffer = i2s_read32(SPI2, &channel);
		(void)flushbuffer;
	}

	for (i = 0; i < buffer.size(); i++) {
		int32_t data = i2s_read32(SPI2, &channel);
		data >>= 16;
		buffer[i] = data;
		if (channel == SKIP_CHANNEL)
			i--;
		if (buffer[i] > max)
			max =  buffer[i];
	}
	i2s_disable(SPI2);
//printf("Max input sound: %x\r\n", max);
}

