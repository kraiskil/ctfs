#include "board_config.h"

extern "C" {
#include "libopencm3/stm32/spi.h"
#include "libopencm3/stm32/usart.h"
}

// Target configs that don't fit into the <board>.cc
// Consider creating and moving these to a <board>.h

#if CMAKE_BOARD == stm32f4_bluepill
#define I2S_OUT SPI2
#elif CMAKE_BOARD == stm32f4_discovery
#define I2S_OUT SPI3
#else
#error unknown CMAKE_BOARD
#endif


/* Callback from printf() in libc */
extern "C" {
int _write(int file, char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
#if HAVE_DEBUG_USART
		usart_send_blocking(USART2, buf[i]);
#endif
	}
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
		spi_send(I2S_OUT, 0);                    // stereo: other channel is mute
		spi_send(I2S_OUT, get_croak_data(i, t)); // stereo: other channel is mute
	}

	audioDAC_shutdown();

	debug_led_off(LED_CROAK);
}


void board_init(void)
{
	board_setup_clock();
	board_setup_gpio();
	board_setup_i2c();
	board_setup_usart();
	board_setup_i2s_in();
	board_setup_wallclock();
	i2s_playback_setup();
}

