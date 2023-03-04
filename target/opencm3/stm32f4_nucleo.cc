/* board configuration for
 * STM32F411-NUCLEO
 *
 * Connections:
 *  USART (USART2):
 *   (internal to the ST-LINK-v2-1)
 *   PA2 - TX
 *   PA3 - RX
 *  Sound out (SPI/I2S 2):
 *   PB13- bit clock
 *   PB15- data
 *   PB12 - l/r clock
 *  Sound in (ADC? 1):
 *   select one of these
 *   PA0 - ADC1, ch0
 *   PA1 - ADC1, ch1
 * LEDS:
 *   old values! there is just the one led on this board
 *   PA5 - (builtin red) listening
 *   PA6 - (unknown. Hopefully disconnected) all the other ones
 * Timers:
 *   TIM2 - wallclock. 1ms tick
 *   TIM5 - Audio input ADC trigger
 */


// Define the chip in use for OpenCM3
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
#include "input_adc.h"

/* Correction term for the audio clock running a bit slow */
constexpr float frequency_correction = 1.024;
float get_input_frequency_correction(void)
{
	return frequency_correction;
}

// GPIO A5 - onboard red LED
//      A6 - unconnected pin. Used as dummy

struct led leds[LED_LAST] = {
	{
		GPIOA, GPIO6, not_inverted
	},                              //croak
	{ GPIOA, GPIO6, not_inverted }, //sleep
	{ GPIOA, GPIO6, not_inverted }, //processing
	{ GPIOA, GPIO6, not_inverted }, //listening
};


void board_setup_clock(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2); //output i2s
}

void board_setup_gpio(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);

	for (int id = 0; id < LED_LAST; id++) {
		gpio_mode_setup(leds[id].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, leds[id].pin);
	}

	/* Connects PA2 to USART2 TX */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

}

void board_setup_i2c(void)
{}

void board_setup_usart(void)
{
	rcc_periph_clock_enable(DEBUG_USART_RCC);

	usart_set_baudrate(DEBUG_USART, 115200);
	usart_set_databits(DEBUG_USART, 8);
	usart_set_stopbits(DEBUG_USART, USART_STOPBITS_1);
	usart_set_mode(DEBUG_USART, USART_MODE_TX);
	usart_set_parity(DEBUG_USART, USART_PARITY_NONE);
	usart_set_flow_control(DEBUG_USART, USART_FLOWCONTROL_NONE);
	usart_enable(DEBUG_USART);
}

void tim5_isr(void)
{
	// clear something, or this ISR gets hit without pause
	TIM_SR(TIM5) &= ~TIM_SR_UIF;
	gpio_toggle(GPIOA, GPIO5); // red led
	// TODO: toggle the ADC conversion
	adc_start_conversion_regular(ADC1);
}

void board_setup_audio_in(void)
{
	rcc_periph_clock_enable(RCC_ADC1);
	// PA0 -> ADC1, ch0
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	//gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

	// Timer5 to trigger the ADC conversion at audio input frequency
	// 16 bit prescaler, 32bit counter.
	rcc_periph_clock_enable(RCC_TIM5);
	rcc_periph_reset_pulse(RST_TIM5);
	timer_set_prescaler(TIM5, 84); // 84MHz -> 1us count
	constexpr int counts = 1e6 / config_fs_input;
	// 1s blinky test
	//constexpr int counts = 1e6;
	timer_set_period(TIM5, counts);
	nvic_enable_irq(NVIC_TIM5_IRQ);
	timer_enable_update_event(TIM5);
	timer_enable_irq(TIM5, TIM_DIER_UIE);
	timer_enable_counter(TIM5);


	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_power_on(ADC1);

	// Select channel 0as the ADC input - PA0
	uint8_t channel_array[16];
	channel_array[0] = 0; // Check which one
	adc_set_regular_sequence(ADC1, 1, channel_array);
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

#if 0
void write_i2c_to_audiochip(uint8_t reg, uint8_t contents)
{
	uint8_t packet[2];
	packet[0] = reg;
	packet[1] = contents;
	/* STM32F411 gives device address with R/W bit,
	 * libopencm wants it without it */
	uint8_t address = (0x94) >> 1;

	i2c_transfer7(I2C1, address, packet, 2, NULL, 0);
}
#endif

void audioDAC_shutdown(void)
{
	// TODO: this is not nucleo specific, rather DAC specific.. Move to separate file
}
void audioDAC_setup(void)
{}

void i2s_playback_setup(void)
{
	/* I2S pins:
	 * Master clock: -
	 * Bit clock: PB13
	 * Data: PB15
	 * L/R clock: PB12
	 */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO13);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO15);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO12);

	/* I2S is implemented as a HW mode of the SPI peripheral.
	* Since this is a STM32F411, there is a separate I2S PLL
	* that needs to be enabled.
	*/
	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2);
	i2s_disable(SPI2);
	i2s_set_standard(SPI2, i2s_standard_philips);
	i2s_set_dataformat(SPI2, i2s_dataframe_ch16_data16);
	i2s_set_mode(SPI2, i2s_mode_master_transmit);
	i2s_masterclock_enable(SPI2);
	/* RCC_PLLI2SCFGR configured values are:
	 * 0x24003010 i.e.
	 * PLLR = 2
	 * PLLI2SN = 192
	 * PLLI2SM = 16
	 * And since the input is PLL source (i.e. HSI = 16MHz)
	 * The I2S clock = 16 / 16 * 192 / 2 = 96MHz
	 * Calculate sampling frequency from equation given in
	 * STM32F411 reference manual:
	 * Fs = I2Sclk/ (32*2 * ((2*I2SDIV)+ODD)*4)
	 * I2SDIV = I2Sclk/(512*Fs)
	 * I2SDIV=24 => 23,4 so 23 + ODD bit set
	 */
	i2s_set_clockdiv(SPI2, 23, 1);
	i2s_enable(SPI2);
}

