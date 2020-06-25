/* board configuration for
 * a bluepill, bluepill2 or blackpill, with
 * STM32F407CC. Yeah, I'm not quite sure what I've
 * bought, but it was cheap, and the vendor swore the
 * board has a genuine ST chip on it. Sure looks like it does.
 * Some sources claim the board is open hardware, but documentation
 * is limited to a .png with pin explanations. The board has pin
 * names on the silk - and these can be matched with the MCU datasheet.

 * Connections:
 *  Sound out (SPI3):
 *   ? - master clock
 *   ?- bit clock
 *   ?- data
 *   ? - l/r clock
 *   ? - DAC reset (leftover from disco - my bluepill will be connected to a PCM5102)
 *  Sound in (SPI N):
 *   ? - master clock
 *   ?- bit clock
 *   ?- data
 *   ?- l/r clock
 * LEDS:
 *   ? - croak
 *   ? - sleep
 *   PC13- processing
 *
 */

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

#define I2S_MICROPHONE I2S1_EXT_BASE
#define I2S_OUT SPI2



/* Correction term for the audio clock running a bit slow */
/* TODO: Check - leftover from disco */
constexpr float frequency_correction = 1.024;
float get_input_frequency_correction(void)
{
	return frequency_correction;
}

struct led
{
	unsigned port;
	uint16_t pin;
};
struct led leds[LED_LAST] = {
	{
		GPIOC, GPIO13
	},                 //croak
	{ GPIOC, GPIO12 }, //sleep
	{ GPIOC, GPIO11 }, //processing
};
void debug_led_on(enum led_ids i)
{
	gpio_set(leds[i].port, leds[i].pin);
}
void debug_led_off(enum led_ids i)
{
	gpio_clear(leds[i].port, leds[i].pin);
}


void board_setup_clock(void)
{
#if 0
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI1); //input i2s
	rcc_periph_clock_enable(RCC_SPI2); //output i2s
#endif
}

void board_setup_gpio(void)
{
	/* PortA is used for:
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
#if 0
	/* Connects PA2 to USART2 TX */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO4); //Audio chip !RESET
#endif
}

void board_setup_i2c(void)
{
	// No I2C: the bluepill is used with PCM5102A.
}

void board_setup_usart(void)
{
#if 0
	rcc_periph_clock_enable(RCC_USART2);

	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_enable(USART2);
#endif
}

void board_setup_i2s_in(void)
{
// Start with I2S communications to audio DAC
#if 0
	/* I2S pins:
	 * Master clock: PA3
	 * Bit clock: PB13
	 * Data in: PC3
	 * L/R clock: PB12
	 */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO3);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO3);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO13);

	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO3);
	gpio_set_af(GPIOC, GPIO_AF5, GPIO3);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO12);

	/* I2S is implemented as a HW mode of the SPI peripheral.
	* Since this is a STM32F411, there is a separate I2S PLL
	* that needs to be enabled.
	*/
	i2s_disable(SPI2);
	i2s_set_standard(SPI2, i2s_standard_philips);
	i2s_set_dataformat(SPI2, i2s_dataframe_ch32_data24);
	i2s_set_mode(SPI2, i2s_mode_master_receive);
	/* RCC_PLLI2SCFGR configured values are:
	 * 0x24003010 i.e.
	 * PLLR = 2
	 * PLLI2SN = 192
	 * PLLI2SM = 16
	 * And since the input is PLL source (i.e. HSI = 16MHz)
	 * The I2S clock = 16 / 16 * 192 / 2 = 96MHz
	 * Calculate sampling frequency from equation given in
	 * STM32F411 reference manual:
	 * Fs = I2Sclk/ (32*2 * ((2*I2SDIV)+ODD))
	 * I2SDIV = I2Sclk/(128*Fs)
	 * Fs=48kHz => 15,624 so 15 + ODD bit set
	 */
	i2s_set_clockdiv(SPI2, 15, 1);
	i2s_enable(SPI2);
#endif
}

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
	gpio_set(GPIOA, GPIO9);
}
void audioDAC_setup(void)
{
	// No setup needed. Except perhaps release softmute, if we end up using it
	gpio_clear(GPIOA, GPIO9);
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
	i2s_set_dataformat(SPI2, i2s_dataframe_ch32_data16);
	i2s_set_mode(SPI2, i2s_mode_master_transmit);
	//i2s_masterclock_enable(SPI2); Don't set this - not routed out
	/* RCC_PLLI2SCFGR configured values are:
	 * 0x24003010 i.e. TODO: CHECK!
	 * PLLR = 2
	 * PLLI2SN = 192
	 * PLLI2SM = 16
	 * And since the input is PLL source (i.e. HSI = 16MHz) TODO: chekc!
	 * The I2S clock = 16 / 16 * 192 / 2 = 96MHz
	 *
	 * Calculate sampling frequency from equation given in
	 * STM32F4xx reference manual RM0090:
	 * (Channel must be 32bits and Fs 16kHz, minimum, for the PCM5102
	 * to be able to generate clocks without master/system clock input
	 * see datasheet table 11)
	 * Fs = I2Sclk/ (32*2 * ((2*I2SDIV)+ODD))
	 * I2SDIV = I2Sclk/(128*Fs)
	 * I2SDIV=24 => 46,875 so 46 + ODD bit clear
	 */
	static_assert(config_fs_output == 16000, "calculated I2S clock divisors with wrong values");
	i2s_set_clockdiv(SPI2, 46, 0);
	i2s_enable(SPI2);
}

