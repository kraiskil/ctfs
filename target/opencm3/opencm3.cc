#include "board_config.h"

extern "C" {
#include "libopencm3/stm32/spi.h"
#include "libopencm3/stm32/usart.h"
}


/* Callback from printf() in libc */
extern "C" {
int _write(int file, char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
		usart_send_blocking(USART2, buf[i]);
	return i;
}
}

void sleep_for(void)
{
	debug_led_on(LED_SLEEP);
	volatile int i = 1000000; //1M ~ 1Hz
	while (--i > 0) {
		;
	}
	debug_led_off(LED_SLEEP);
}


void play_croak(enum tone t)
{
	debug_led_on(LED_CROAK);
	audioDAC_setup();

	printf("croak!\r\n");

	for (int i = 0; i < croak_len; i++) {
		spi_send(SPI3, 0);                    // stereo: other channel is mute
		spi_send(SPI3, get_croak_data(i, t)); // stereo: other channel is mute
	}

	audioDAC_shutdown();

	debug_led_off(LED_CROAK);
}

