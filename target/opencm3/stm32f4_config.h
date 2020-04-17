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
 *   PB15- data
 *   PB12- l/r clock
 *
 */

#define PORT_HEARTBEAT_LED GPIOD
#define PIN_HEARTBEAT_LED GPIO13


#define I2S_MICROPHONE I2S2_EXT_BASE
#define I2S_SPEAKER    I2S3_EXT_BASE
#define I2C_SPEAKER    I2C1


static inline void board_setup_clock(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

	rcc_osc_on(RCC_PLLI2S);
	rcc_periph_clock_enable(RCC_SPI2); //input i2s
	rcc_periph_clock_enable(RCC_SPI3); //output i2s

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
}

