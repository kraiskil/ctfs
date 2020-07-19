#pragma once
#include <cstdint>
// Test sounds are all 8kHz.
constexpr int config_fs_output = 8000;
constexpr int config_fs_input = 8000;
// Output "unit" volume. Sine with this amplitude is still pleasant.
constexpr uint16_t config_output_volume_value = 2000;

