/* board configuration for
 * STM32F4 Discovery MB1115
 * (the one with a STM32F411VET6U)
 *
 * Connections:
 *  USART (USART1):
 *   PA2 - USART Rx (i.e. towards host)
 *  Sound out (SPI3):
 *   PC7 - master clock
 *   PC10- bit clock
 *   PC12- data
 *   PA4 - l/r clock
 *   PD4 - DAC reset
 *  I2C (I2C1):
 *   PB6 - SCL
 *   PB9 - SDA
 *  Sound in (SPI2):
 *   PA3 - master clock
 *   PB13- bit clock
 *   PC3- data
 *   PB12- l/r clock
 * LEDS:
 *   PD12- croak (green)
 *   PD13- orange
 *   PD14- sleep (red)
 *   PD15- processing (blue)
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

/* Correction term for the audio clock running a bit slow */
constexpr float frequency_correction = 1.024;
float get_input_frequency_correction(void)
{
	return frequency_correction;
}

struct led leds[LED_LAST] = {
	{
		GPIOD, GPIO12, not_inverted
	},                               //croak
	{ GPIOD, GPIO14, not_inverted }, //sleep
	{ GPIOD, GPIO15, not_inverted }, //processing
};


void board_setup_clock(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2); //input i2s
	rcc_periph_clock_enable(RCC_SPI3); //output i2s

}

void board_setup_gpio(void)
{
	/* PortA is used for:
	 *  USART
	 *  Digital audio in
	 * PortB:
	 *  DAC I2C
	 *  Digital audio in
	 * PortC is used for:
	 *  Digital audio out
	 * PortD is used for:
	 *  Heartbeat LED
	 */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	for (int id = 0; id < LED_LAST; id++) {
		gpio_mode_setup(leds[id].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, leds[id].pin);
		gpio_mode_setup(leds[id].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, leds[id].pin);
	}

	/* I2C to Audio chip */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO6); //SCL
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO9); //SDA
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO6);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO9);

	/* Connects PA2 to USART2 TX */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO4); //Audio chip !RESET
}

void board_setup_i2c(void)
{
	// CCR must be configured when device is disabled
	rcc_periph_clock_enable(RCC_I2C1);
	i2c_peripheral_disable(I2C1);
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 42);
	i2c_set_own_7bit_slave_address(I2C1, 0); // For OAR1 bit 14
	/* TODO: what is this?? */
	int *addy = (int*)0x4000541c;
	*addy = 0x834;
	i2c_peripheral_enable(I2C1);
}

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

void board_setup_i2s_in(void)
{
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

void audioDAC_shutdown(void)
{
	// TODO: does this clear all register settings?
	// there is an alternative to power down the chip internally too
	// this way audioDAC_setup() can be split into _init() and _start()
	gpio_clear(GPIOD, GPIO4);
}
void audioDAC_setup(void)
{
	// Set !RESET
	gpio_set(GPIOD, GPIO4);

	write_i2c_to_audiochip(0x20, dac_volume); // Master A volume
	write_i2c_to_audiochip(0x21, dac_volume); // Master B volume

	write_i2c_to_audiochip(0x06, 0x04); // interface control 1: set I2S dataformat
	write_i2c_to_audiochip(0x02, 0x9e); // power control 1: Magic value to power up the chip
}

void i2s_playback_setup(void)
{
	/* I2S pins:
	 * Master clock: PC7
	 * Bit clock: PC10
	 * Data: PC12
	 * L/R clock: PA4
	 */
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);
	gpio_set_af(GPIOC, GPIO_AF6, GPIO7);
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
	gpio_set_af(GPIOC, GPIO_AF6, GPIO10);
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOC, GPIO_AF6, GPIO12);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO4);
	gpio_set_af(GPIOA, GPIO_AF6, GPIO4);

	/* I2S is implemented as a HW mode of the SPI peripheral.
	* Since this is a STM32F411, there is a separate I2S PLL
	* that needs to be enabled.
	*/
	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI3);
	i2s_disable(SPI3);
	i2s_set_standard(SPI3, i2s_standard_philips);
	i2s_set_dataformat(SPI3, i2s_dataframe_ch16_data16);
	i2s_set_mode(SPI3, i2s_mode_master_transmit);
	i2s_masterclock_enable(SPI3);
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
	i2s_set_clockdiv(SPI3, 23, 1);
	i2s_enable(SPI3);
}

