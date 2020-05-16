/* board configuration for
 * STM32F4 Discovery MB1115
 * (the one with a STM32F411VET6U)
 */

#define PORT_HEARTBEAT_LED GPIOC
#define PIN_HEARTBEAT_LED GPIO8


#define I2S_MICROPHONE I2S2_EXT_BASE
#define I2S_SPEAKER    I2S3_EXT_BASE
#define I2C_SPEAKER    I2C1


static inline void board_setup_clock(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
}

