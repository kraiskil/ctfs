/* Top-level board configuration.
 */
#pragma once
#include <cstdint>
constexpr int config_fs_output = 8000;
constexpr int config_fs_input = 8000;
#define I2S_OUT SPI3

// value for DAC volume registers
constexpr uint8_t dac_volume = 0xf0;

