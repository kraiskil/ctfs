#include "config.h"
#include "datatype.h"
#include "treefrog.h"
#include <cstring>

extern "C" {
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
}

#if defined STM32F4
#include "stm32f4_config.h"
#elif defined STM32F0
#include "stm32f0_config.h"
#else
#error Need a target board config file
#endif


static constexpr int croak_samples = config_fs * 1.5 /*seconds*/;

/* TODO: move these functions to dedicated board config files */
static void gpio_setup(void)
{
	/* PortA is used for:
	 *  USART
	 * PortB:
	 *  DAC I2C
	 * PortC is used for:
	 *  Digital audio out
	 * PortD is used for:
	 *  Heartbeat LED
	 */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	gpio_mode_setup(PORT_HEARTBEAT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_HEARTBEAT_LED);
	gpio_mode_setup(PORT_HEARTBEAT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

	/* I2C to Audio chip */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO6); //SCL
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO9); //SDA
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO6);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO9);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO4); //Audio chip !RESET


	// CCR must be configured when device is disabled
	rcc_periph_clock_enable(RCC_I2C1);
	i2c_peripheral_disable(I2C1);
	//i2c_set_clock_frequency(I2C1, 42);
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 42);
	i2c_set_own_7bit_slave_address(I2C1, 0); // For OAR1 bit 14
	int *addy = (int*)0x4000541c;
	*addy = 0x834;
	i2c_peripheral_enable(I2C1);
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

static void write_i2c_to_audiochip(uint8_t reg, uint8_t contents)
{
	uint8_t packet[2];
	packet[0] = reg;
	packet[1] = contents;
	/* STM32F411 gives device address with R/W bit,
	 * libopencm wants it without it */
	uint8_t address = (0x94) >> 1;

	i2c_transfer7(I2C1, address, packet, 2, NULL, 0);
}

static void i2s_setup(void)
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

static void audioDAC_shutdown(void)
{
	// TODO: does this clear all register settings?
	// there is an alternative to power down the chip internally too
	// this way audioDAC_setup() can be split into _init() and _start()
	gpio_clear(GPIOD, GPIO4);
}
static void audioDAC_setup(void)
{
	// Set !RESET
	gpio_set(GPIOD, GPIO4);

	write_i2c_to_audiochip(0x20, 0xc0); // Master A volume
	write_i2c_to_audiochip(0x21, 0xc0); // Master B volume

	write_i2c_to_audiochip(0x06, 0x04); // interface control 1: set I2S dataformat
	write_i2c_to_audiochip(0x02, 0x9e); // power control 1: Magic value to power up the chip
}


int main(void)
{
	board_setup_clock();

	gpio_setup();
	debuggig_usart_setup();

	i2s_setup();

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
	gpio_set(PORT_HEARTBEAT_LED, PIN_HEARTBEAT_LED);
	audioDAC_setup();

	usart_send_blocking(USART2, 'c');
	usart_send_blocking(USART2, 'r');
	usart_send_blocking(USART2, 'o');
	usart_send_blocking(USART2, 'a');
	usart_send_blocking(USART2, 'k');
	usart_send_blocking(USART2, '!');
	usart_send_blocking(USART2, '\r');
	usart_send_blocking(USART2, '\n');

	for (int i = 0; i < croak_samples; i++) {
		spi_send(SPI3, 0);                 // stereo: other channel is mute
		spi_send(SPI3, get_croak_data(i)); // stereo: other channel is mute
	}

	audioDAC_shutdown();

	gpio_clear(PORT_HEARTBEAT_LED, PIN_HEARTBEAT_LED);
}

