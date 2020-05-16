#pragma once
#include <cstdint>
#include "config.h"
#include "datatype.h"
#include "sin_table.h"

/* Maximum FFT size code is prepared for. Probably also
 * the only one used. */
constexpr int MAX_FFT_SIZE = listen_buffer_samples;
static_assert(MAX_FFT_SIZE <= SIN_TABLE_RESOLUTION,
    "sin table is not accurate enough");

// Stockham Autosort Fast Fourier Transform
void fft_sa(int n, std::complex<int32_t> *x);
void fft_sa(int n, std::complex<float> *x);
void fft_calc_abs(std::complex<int32_t> *data, frequency_buf_t &out);
void fft_calc_abs(std::complex<float> *data, frequency_buf_t &out);


template <class internal_data_representation>
class fft
{
	typedef std::complex<internal_data_representation> complex_t;
public:
	unsigned fft_size;
	unsigned fs;

	fft(unsigned fft_size = MAX_FFT_SIZE, unsigned fs = config_fs_input)
		: fft_size(fft_size), fs(fs)
	{}

	void run(listen_buf_t &input, frequency_buf_t &output)
	{
		complex_t complex_input[MAX_FFT_SIZE];
		for (unsigned i = 0; i < fft_size; i++) {
			complex_input[i].real(input[i]);
			complex_input[i].imag(0);
		}
		fft_sa(fft_size, complex_input);
		fft_calc_abs(complex_input, output);
	}
	int index_to_frequency(int idx)
	{
		/* TODO: define fft size as its log2 value, and shift */
		return (float)idx * fs / fft_size;
	}
};

