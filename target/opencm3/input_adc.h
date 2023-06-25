/*
 * Input handling adaptation.
 * Intended to be #include'd from the
 * BOARD_SUPPORT_FILE
 */
#include "board_config.h"
#include "config.h"
#include "datatype.h"
#include <cstdio>
extern "C" {
#include <libopencm3/stm32/adc.h>
}

// Read one sample from the ADC.
// This expects there is a timer that starts
// the ADC conversion, and here we wait for
// ADC ready and read the result
static int16_t read_8k_sample(void)
{
	// TODO: configure interrupts, wait for one rather than polling...
	while (!adc_eoc(ADC1)) {
		;
	}
	int16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}

void listen_for_croaks(listen_buf_t &buffer)
{

	unsigned i;
	for (i = 0; i < buffer.size(); i++) {
		buffer[i] = read_8k_sample();
	}
}

void adc_input_init(void)
{}

