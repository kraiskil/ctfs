/* board configuration for
 * a bluepill, bluepill2 or blackpill, with
 * STM32F401xB/C. Yeah, I'm not quite sure what I've
 * bought, but it was cheap, and the vendor swore the
 * board has a genuine ST chip on it. Sure looks like it does.
 * Some sources claim the board is open hardware, but documentation
 * is limited to a .png with pin explanations. The board has pin
 * names on the silk - and these can be matched with the MCU datasheet.

 * Connections:
 *  Sound out (SPI2):
 *   NA - master clock
 *   PB13- bit clock
 *   PB15- data
 *   PB12 - l/r clock
 *   FMT - PA8 (TODO: hard-connect low when there is a custom PCB)
 *   XSMT- PA9 - soft mute (active low)
 *  Sound in (SPI3):
 *   PB3- bit clock
 *   PB5- data
 *   PA4- l/r clock
 * USART
 *   PB6 - TX
 * LEDS:
 *   PC13 - croak
 *   ? - sleep
 *   ? - processing
 *
 */

#ifndef STM32F4
#define STM32F4
#endif

extern "C" {
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
}
#include "treefrog.h"
#include "opencm3.h"
#include "input_i2s.h"


/* Correction term for the audio clock running a bit fast */
constexpr float frequency_correction = 0.999;
float get_input_frequency_correction(void)
{
	return frequency_correction;
}

//NB: the onboard LED is PC13
struct led leds[LED_LAST] = {
	{
		GPIOC, GPIO13, not_inverted
	},                               //croak
	{ GPIOC, GPIO12, not_inverted }, //sleep
	{ GPIOC, GPIO11, inverted },     //processing
};

void board_setup_clock(void)
{
	// 25MHz HSE crystal
	rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI3); //input i2s
	rcc_periph_clock_enable(RCC_SPI2); //output i2s
}

void board_setup_gpio(void)
{
	/* PortA is used for:
	 * I2S input
	 * PortB:
	 * PortC is used for:
	 *  LED
	 * PortD is used for:
	 */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	for (int id = 0; id < LED_LAST; id++) {
		gpio_mode_setup(leds[id].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, leds[id].pin);
		gpio_mode_setup(leds[id].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, leds[id].pin);
	}

	debug_led_off(LED_CROAK);
}

void board_setup_i2c(void)
{
	// No I2C: the bluepill is used with PCM5102A.
}

void board_setup_usart(void)
{
	/* USART1 output on PB6 */
	static_assert(DEBUG_USART == USART1, "wrong rcc initialization");
	rcc_periph_clock_enable(RCC_USART1);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO6);
	gpio_set_af(GPIOB, GPIO_AF7, GPIO6);

	usart_set_baudrate(DEBUG_USART, 115200);
	usart_set_databits(DEBUG_USART, 8);
	usart_set_stopbits(DEBUG_USART, USART_STOPBITS_1);
	usart_set_mode(DEBUG_USART, USART_MODE_TX);
	usart_set_parity(DEBUG_USART, USART_PARITY_NONE);
	usart_set_flow_control(DEBUG_USART, USART_FLOWCONTROL_NONE);
	usart_enable(DEBUG_USART);
}

void board_setup_audio_in(void)
{}

void board_setup_wallclock(void)
{
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_reset_pulse(RST_TIM2); //not "enable" or "disable"?
	// 1us timer - measured to be accurate within 0.3% (w.r.t. my saleae)
	// If APB1 prescaler is set to anything but 1 in RCC, TIM2 will get
	// 2xAPB1 frequency input, hence the *2.
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 1000000));

	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, 0xffffffff);
	timer_enable_counter(TIM2);
}

void wallclock_start(void)
{
	// generate "update event" - i.e. reset the timer to start from zero (todo: check!)
	timer_generate_event(TIM2, TIM_EGR_UG);
}
uint32_t wallclock_time_us(void)
{
	return timer_get_counter(TIM2);
}

void audioDAC_shutdown(void)
{
	gpio_clear(GPIOA, GPIO9);
}
void audioDAC_setup(void)
{
	// No setup needed. Except perhaps release softmute, if we end up using it
	gpio_set(GPIOA, GPIO9);
}

void i2s_playback_setup(void)
{
	/* I2S pins - using SPI2:
	 * Master clock: PC6 - but this is not routed out on the bluepill board.
	 * Bit clock: PB13
	 * Data: PB15
	 * L/R clock: PB12
	 * FMT - PA8 (drive low for I2S)
	 * XSMT- PA9 - soft mute (active low)
	 */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO13);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO15);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO12);

	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
	gpio_clear(GPIOA, GPIO8);
	gpio_clear(GPIOA, GPIO9);

	/* I2S is implemented as a HW mode of the SPI peripheral.
	* There is a separate I2S PLL that needs to be enabled - but
	* contents are left at reset values.
	*/
	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2);
	i2s_disable(SPI2);
	i2s_set_standard(SPI2, i2s_standard_philips);
	i2s_set_dataformat(SPI2, i2s_dataframe_ch16_data16);
	i2s_set_mode(SPI2, i2s_mode_master_transmit);
	/*
	 * RM0368, table 91 gives the I2SDIV as 187, ODD bit =1,
	 * when "PLLM VCO =1 MHz or 2 MHz." TODO: check where this is is, but
	 * it seems to be working just fine:
	 * Measuring on Saleae, one single unit, 8kHz on the dot :)
	 */
	i2s_set_clockdiv(SPI2, 187, 1);
	static_assert(config_fs_output == 8000, "calculated I2S clock divisors with wrong values");
	i2s_enable(SPI2);
}

