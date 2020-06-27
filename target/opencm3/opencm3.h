#pragma once
#include <cstdint>
#include "treefrog.h"

typedef bool led_polarity;
constexpr led_polarity inverted = true;
constexpr led_polarity not_inverted = false;
struct led
{
	unsigned port;
	uint16_t pin;
	led_polarity inverted; // if true, setting pin turns led off
};
// Array of lenght LED_LAST describing the available leds. Implement this in
// <board>.cc
extern struct led leds[LED_LAST];

