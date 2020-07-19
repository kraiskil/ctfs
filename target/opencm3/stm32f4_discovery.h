/* Top-level board configuration.
 */
#pragma once
#include <cstdint>
// Audio sample frequenies
constexpr int config_fs_output = 8000;
constexpr int config_fs_input = 8000;
// Output "unit" volume. Sine with this amplitude is still pleasant.
constexpr uint16_t config_output_volume_value = 600;
#define I2S_OUT SPI3
#define I2S_IN SPI2

// value for DAC volume registers
constexpr uint8_t dac_volume = 0xf0;

#define HAVE_DEBUG_SWO

