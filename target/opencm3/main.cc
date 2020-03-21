#include "treefrog.h"

#ifdef STM32F4
#include "stm32f4_config.h"
#else
#error Need a target board config file
#endif

extern "C" {
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
}

/* TODO: move these functions to dedicated board config files */
static void gpio_setup(void)
{
	/* PortA is used for:
	 *  USART
	 * PortD is used for:
	 *  Heartbeat LED
	 */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOD);

	gpio_mode_setup(PORT_HEARTBEAT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_HEARTBEAT_LED);
}

static void debuggig_usart_setup(void)
{
	rcc_periph_clock_enable(RCC_USART2);

	/* Connects PA2 to USART2 TX */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_enable(USART2);
}


int main(void)
{
	gpio_setup();
	debuggig_usart_setup();
	treefrog();
}

void sleep_for(void)
{
	volatile int i = 1000000; //1M ~ 1Hz
	while (--i > 0) {
		;
	}
}

void play_croak(void)
{
	gpio_toggle(PORT_HEARTBEAT_LED, PIN_HEARTBEAT_LED);

	usart_send_blocking(USART2, 'c');
	usart_send_blocking(USART2, 'r');
	usart_send_blocking(USART2, 'o');
	usart_send_blocking(USART2, 'a');
	usart_send_blocking(USART2, 'k');
	usart_send_blocking(USART2, '!');
	usart_send_blocking(USART2, '\r');
	usart_send_blocking(USART2, '\n');
}

