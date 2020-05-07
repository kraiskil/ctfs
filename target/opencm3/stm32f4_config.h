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
 *   PB12- croak (green)
 *   PB13- orange
 *   PB14- sleep (red)
 *   PB15- processing (blue)
 *
 */

#pragma once
extern "C" {
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
}
#include "treefrog.h"

struct led
{
	unsigned port;
	uint16_t pin;
};
struct led leds[LED_LAST] = {
	{
		GPIOD, GPIO12
	},                 //croak
	{ GPIOD, GPIO14 }, //sleep
	{ GPIOD, GPIO15 }, //processing
};
void debug_led_on(enum led_ids i)
{
	gpio_set(leds[i].port, leds[i].pin);
}
void debug_led_off(enum led_ids i)
{
	gpio_clear(leds[i].port, leds[i].pin);
}


#define I2S_MICROPHONE I2S2_EXT_BASE
#define I2S_SPEAKER    I2S3_EXT_BASE
#define I2C_SPEAKER    I2C1


static inline void board_setup_clock(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2); //input i2s
	rcc_periph_clock_enable(RCC_SPI3); //output i2s

}

static inline void board_setup_gpio(void)
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

static inline void board_setup_i2c(void)
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

static inline void board_setup_usart(void)
{
	rcc_periph_clock_enable(RCC_USART2);

	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_enable(USART2);
}

static void board_setup_i2s_in(void)
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

