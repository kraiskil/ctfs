#pragma once
#include <array>
#include <complex>
#include <cstdint>

typedef int16_t fixp;
// TODO 0x8000 is nicer to divide with, but that would
// cause overflows. Perhaps 0x4000?
#define FIXP_SCALE 0x4000

typedef int16_t audio_sample_t;
constexpr unsigned listen_buffer_samples_log2 = 11;
constexpr unsigned listen_buffer_samples = 1 << listen_buffer_samples_log2;
constexpr unsigned frequency_buffer_samples = listen_buffer_samples / 2;
typedef std::array<audio_sample_t, listen_buffer_samples> listen_buf_t;

typedef uint16_t fft_bin_t;
//TODO: the selection of how to calculate fft should be a target choice.
//      When you move this decision to the target, make sure the tests
//      execute both integer and float tests.
//typedef int32_t fft_internal_datatype;
typedef float fft_internal_datatype;
typedef std::array<fft_bin_t, frequency_buffer_samples> frequency_buf_t;

