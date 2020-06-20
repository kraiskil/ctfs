#include "treefrog.h"


/* Generic API */

/* Correction term for the input audio clock.
 * Multiply calcualted frequencies with this value to get the
 * real frequency. */
float get_input_frequency_correction(void);
void debug_led_on(enum led_ids i);
void debug_led_off(enum led_ids i);
void wallclock_start(void);
uint32_t wallclock_time_us(void);
void board_init(void);


/* Specific API - this might not be the right place */
void board_setup_clock(void);
void board_setup_gpio(void);
void board_setup_i2c(void);
void board_setup_usart(void);
void board_setup_i2s_in(void);
void board_setup_wallclock(void);

void write_i2c_to_audiochip(uint8_t reg, uint8_t contents);
void audioDAC_shutdown(void);
void audioDAC_setup(void);
void i2s_playback_setup(void);

