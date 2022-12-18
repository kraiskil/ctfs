#pragma once
#include "config.h"
#include <array>
#include <complex>
#include <cstdint>

/******
 * Audio time domain input
 */
typedef int16_t audio_sample_t;
constexpr unsigned listen_buffer_samples_log2 = 11;
constexpr unsigned listen_buffer_samples = 1 << listen_buffer_samples_log2;
constexpr unsigned listen_buffer_bytes = listen_buffer_samples * sizeof(audio_sample_t);
constexpr unsigned frequency_buffer_samples = listen_buffer_samples / 2;
typedef std::array<audio_sample_t, listen_buffer_samples> listen_buf_t;


/******
 * FFT
 */
typedef int16_t fixp;
// TODO 0x8000 is nicer to divide with, but that would
// cause overflows. Perhaps 0x4000?
#define FIXP_SCALE 0x4000

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
	frequency_t& operator*(int i)
	{
		this->f *= i;
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

	bool close_enough(const frequency_t &other) const
	{
		float ratio = this->f / other.f;
		if (ratio > 0.995 && ratio < 1.005)
			return true;
		else
			return false;
	}

};


/******
 * Peaks, tones and croaks
 */
enum note
{
	/* Lowest note & octave that FFT has currently enough resolution for */
	A3         = 0, As3 = 1, Bf3 = 1, B3 = 2, C4 = 3, Cs4 = 4, Df4 = 4, D4 = 5,
	Ds4        = 6, Ef4 = 6, E4  = 7, F4  = 8, Fs4 = 9, G4  = 10, Gs4 = 11,
	/* Next one */
	A4         = 12, As4 = 13, B4  = 14, C5  = 15, Cs5 = 16, D5  = 17, Ds5 = 18,
	E5         = 19, F5  = 20, Fs5 = 21, G5  = 22, Gs5 = 23,
	A5         = 24, As5 = 25, B5  = 26, C6  = 27, Cs6 = 28, D6  = 29, Ds6 = 30,
	E6         = 31, F6  = 32, Fs6 = 33, G6  = 34, Gs6 = 35,
	A6         = 36, As6 = 37, B6  = 38, C7  = 39, Cs7 = 40, D7  = 41, Ds7 = 42,
	E7         = 43, F7  = 44, Fs7 = 45, G7  = 46, Gs7 = 47,
	/* No point in going higher: A7 is 3500Hz */
	A7         = 48,
	LAST_TONE  = 48,
	NOT_A_TONE = 49,
};


struct peak
{
	frequency_t freq;
	uint16_t ampl;
};
typedef std::array<struct peak, max_detected_peaks> peak_array_t;

typedef std::array<enum note, max_detected_tones> croak_array_t;

