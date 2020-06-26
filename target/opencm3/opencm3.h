#pragma once

// Target configs that don't fit into the <board>.cc
// Consider creating and moving these to a <board>.h

#if CMAKE_BOARD == stm32f4_bluepill
#define I2S_OUT SPI2
#elif CMAKE_BOARD == stm32f4_discovery
#define I2S_OUT SPI3
#else
#error unknown CMAKE_BOARD
#endif


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

