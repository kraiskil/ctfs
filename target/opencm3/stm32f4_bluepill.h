/* Top-level board configuration.
 */
#pragma once
#include <cstdint>

constexpr int config_fs_output = 16000;
constexpr int config_fs_input = 8000;
// Output "unit" volume. Sine with this amplitude is still pleasant.
// This value for PCM5102+PAM8403+my 4ohm-3W speaker
constexpr uint16_t config_output_volume_value = 400;

#define I2S_OUT SPI2
#define I2S_IN SPI3

#define HAVE_DEBUG_USART
#define DEBUG_USART USART1

