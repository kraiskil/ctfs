#include "board_config.h"
#include "target_adaptation_api.h"
#include "opencm3.h"

extern "C" {
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/spi.h"
#include "libopencm3/stm32/usart.h"
}



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


void debug_led_on(enum led_ids i)
{
	if (leds[i].inverted)
		gpio_clear(leds[i].port, leds[i].pin);
	else
		gpio_set(leds[i].port, leds[i].pin);
}
void debug_led_off(enum led_ids i)
{
	if (leds[i].inverted)
		gpio_set(leds[i].port, leds[i].pin);
	else
		gpio_clear(leds[i].port, leds[i].pin);
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

