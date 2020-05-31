#pragma once
#include "config.h"
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

// A frequency - tones of the [sic - there are several] equal-tempered
// scale are not integers. Nor are FFT bin center frequencies
class frequency_t
{
public:
	float f;
	frequency_t() : f(0) {}
	frequency_t(float f) : f(f){}
	frequency_t(double f) : f(f){}
	frequency_t(uint16_t f) : f(f){}
	frequency_t(int f) : f(f){}
	frequency_t& operator=(uint16_t f)
	{
		this->f = f;
		return *this;
	}
	frequency_t& operator=(float f)
	{
		this->f = f;
		return *this;
	}

	operator float() const
	{
		return this->f;
	}
	operator uint16_t() const
	{
		return this->f;
	}

	bool operator==(const frequency_t &other) const
	{
		return fabs(this->f - other.f) < 0.01;
	}
	bool operator==(const int &other) const
	{
		return fabs(this->f - other) < 0.01;
	}
};

struct peak
{
	frequency_t freq;
	uint16_t ampl;
};
typedef std::array<struct peak, max_detected_peaks> peak_array_t;

