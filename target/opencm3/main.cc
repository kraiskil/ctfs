#include "treefrog.h"

extern "C" {
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
}
#define PORT_LED GPIOC
#define PIN_LED GPIO8
static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	/* Manually: */
	//RCC_AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOC);


	/* Set GPIO8 (in GPIO port C) to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
}


int main(void)
{
	gpio_setup();
	treefrog();
}

void sleep_for(void)
{
	volatile int i=1000000;//1M ~ 1Hz
	while(--i > 0)
		;
}

void play_croak(void)
{
	gpio_toggle(PORT_LED, PIN_LED);	/* LED on/off */
}

