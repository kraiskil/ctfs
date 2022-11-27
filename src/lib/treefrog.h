#pragma once
#include "datatype.h"
#include "tones.h"
#include <chrono>
using namespace std::chrono_literals;

// entrypoint for treefrog
void treefrog(void);

// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void);

// Get the sample_num'th data in one emitted croak
int16_t get_croak_data(int sample_num, enum tone);

/* Platform-level interfaces */
void play_croak(enum tone);
void sleep_for(const milliseconds_t&);
void listen_for_croaks(listen_buf_t &buffer);

enum led_ids
{
	LED_CROAK      = 0,
	LED_SLEEP      = 1,
	LED_PROCESSING = 2,
	LED_LISTENING  = 3,
	LED_LAST       = 4
};
void debug_led_on(enum led_ids i);
void debug_led_off(enum led_ids i);
void wallclock_start(void);
uint32_t wallclock_time_us(void);

void print_statistics(void);

float get_input_frequency_correction(void);

