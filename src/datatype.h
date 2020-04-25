#pragma once
#include <array>
#include <complex>
#include <cstdint>

typedef int16_t fixp;
// TODO 0x8000 is nicer to divide with, but that would
// cause overflows. Perhaps 0x4000?
#define FIXP_SCALE 0x4000

constexpr unsigned listen_buffer_samples = 2048;
constexpr unsigned frequency_buffer_samples = listen_buffer_samples / 2;
typedef std::array<int16_t, listen_buffer_samples> listen_buf_t;
typedef std::array<uint16_t, frequency_buffer_samples> frequency_buf_t;

// For FFT internal use only (and unit tests)
typedef int32_t fft_internal_datatype;
typedef std::complex<fft_internal_datatype> complex_t;

