#include "config.h"
#include "datatype.h"
#include "treefrog.h"
#include <cstring>


extern "C" {
int _write(int file, char *buf, int len);
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
}

#include "board_config.h"

static constexpr int croak_samples = config_fs_output * 1.5 /*seconds*/;


/* Callback from the libc when using printf */
extern "C" {
int _write(int file, char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
		usart_send_blocking(USART2, buf[i]);
	return i;
}
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

static void i2s_playback_setup(void)
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
	board_setup_gpio();
	board_setup_i2c();
	board_setup_usart();
	board_setup_i2s_in();

	i2s_playback_setup();

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

	printf("croak!\r\n");

	for (int i = 0; i < croak_samples; i++) {
		spi_send(SPI3, 0);                 // stereo: other channel is mute
		spi_send(SPI3, get_croak_data(i)); // stereo: other channel is mute
	}

	audioDAC_shutdown();

	gpio_clear(PORT_HEARTBEAT_LED, PIN_HEARTBEAT_LED);
}

