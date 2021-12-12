#include "board_config.h"
#include "target_adaptation_api.h"
#include "opencm3.h"

extern "C" {
#include "libopencm3/cm3/itm.h"
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
#if defined HAVE_DEBUG_USART
		usart_send_blocking(DEBUG_USART, buf[i]);
#elif defined HAVE_DEBUG_SWO
		while (ITM_STIM32(0) == 0) {
			;
		}
		ITM_STIM8(0) = buf[i];
#endif
	}
	return i;
}
}

void sleep_for(const std::chrono::duration<int> &time)
{
	std::chrono::microseconds us = std::chrono::microseconds(time);
	wallclock_start();
	debug_led_on(LED_SLEEP);

	while (std::chrono::microseconds(wallclock_time_us()) < us) {
		;
	}
	debug_led_off(LED_SLEEP);
}


void play_croak(enum tone t)
{
	debug_led_on(LED_CROAK);
	audioDAC_setup();

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

