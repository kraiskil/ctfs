/* Top-level board configuration.
 */
#pragma once
#include <cstdint>

// Define the chip in use for OpenCM3
#ifndef STM32F4
#define STM32F4
#endif

// Audio sample frequenies
constexpr int config_fs_output = 8000;
constexpr int config_fs_input = 8000;
// Output "unit" volume. Sine with this amplitude is still pleasant.
constexpr uint16_t config_output_volume_value = 600;
#define I2S_OUT SPI3
#define I2S_IN SPI2

// value for DAC volume registers
// TODO: there is no dac_volume setting. This is not the place for this setting! Remove
constexpr uint8_t dac_volume = 0xf0;

//#define HAVE_DEBUG_SWO
#define HAVE_DEBUG_USART
#define DEBUG_USART USART2
#define DEBUG_USART_RCC RCC_USART2

