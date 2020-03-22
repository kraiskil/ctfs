#pragma once
#include <array>
#include <cstdint>

typedef int16_t fixp;
// TODO 0x8000 is nicer to divide with, but that would
// cause overflows. Perhaps 0x4000?
#define FIXP_SCALE 0x4000

constexpr unsigned listen_buffer_samples = 2048;
typedef std::array<int16_t, listen_buffer_samples> listen_buf_t;

// For FFT internal use only (and unit tests)
typedef int32_t fft_internal_datatype;

