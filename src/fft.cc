#include "fft.h"
#define MAX_FFT_SIZE 2048

#include <cassert>

/* placeholder floating point implementation */
#include <cmath>
#include <complex>
#include <iostream>
typedef std::complex<double> complex_t;

double twiddle_re(int phase, int lenght);
double twiddle_im(int phase, int lenght);
complex_t multiply(complex_t a, double b_re, double b_im);

/* Stockham autosort FFT by okojisan from
 * http://wwwa.pikara.ne.jp/okojisan/otfft-en/stockham2.html
 * Original uses doubles and <cmath>. Fixpoint conversion by me.
 */
void fft0(int n, int s, bool eo, complex_t *x, complex_t *y)
// n  : sequence length
// s  : stride
// eo : x is output if eo == 0, y is output if eo == 1
// x  : input sequence(or output sequence if eo == 0)
// y  : work area(or output sequence if eo == 1)
{
	const int m = n / 2;

	if (n == 1) {
		if (eo) for (int q = 0; q < s; q++) y[q] = x[q];
	}
	else {
		for (int p = 0; p < m; p++) {
			for (int q = 0; q < s; q++) {
				const complex_t a = x[q + s * (p + 0)];
				const complex_t b = x[q + s * (p + m)];
				y[q + s * (2 * p + 0)] =  a + b;
				y[q + s * (2 * p + 1)] = multiply((a - b), twiddle_re(p, n), twiddle_im(p, n));
			}
		}
		fft0(n / 2, 2 * s, !eo, y, x);
	}
}
void fft_sa(int n, complex_t *x) // Fourier transform
// n : sequence length
// x : input/output sequence
{
	complex_t *y = new complex_t[n];
	fft0(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] /= n;
}



void frog_fft(const int16_t *in, int16_t *out, int fft_size)
{

	complex_t data[fft_size];

	for (int i = 0; i < fft_size; i++) {
		data[i].real(in[i]);
		data[i].imag(0);
	}

	fft_sa(fft_size, data);

	for (int i = 0; i < fft_size; i++) {
		double o = std::abs(data[i]);
		out[i] = o;
	}
}

/* Twiddle factor computation for FFT */
// TODO: make sin_table of size MAX_FFT_SIZE/4, and
// do the logic in twiddle_*() functions
// also replace init_sin_table() with values computed offline
double sin_table[5 * MAX_FFT_SIZE / 4];
__attribute__((__constructor__))
void init_sin_table(void)
{
	const double theta0 = 2 * M_PI / MAX_FFT_SIZE;
	for (int i = 0; i < 5 * MAX_FFT_SIZE / 4; i++)
		sin_table[i] = sin(i * theta0);
}
double twiddle_re(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return cos(p*theta0);
	return sin_table[MAX_FFT_SIZE / 4 + p * stride];
}
double twiddle_im(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return -sin(p*theta0);
	return -sin_table[p * stride];
}
complex_t multiply(complex_t a, double b_re, double b_im)
{
	complex_t rv;
	rv.real(a.real() * b_re - a.imag() * b_im);
	rv.imag(a.real() * b_im + b_re * a.imag());
	return rv;
}


int index_of_peak(int16_t *buf, int N)
{
	int16_t maxval = 0;
	int     maxidx = 0;
	for (int i = 0; i < N; i++) {
		if (buf[i] > maxval) {
			maxidx = i;
			maxval = buf[i];
		}
	}
	return maxidx;
}

int index_to_frequency(int idx, int fs, int fft_size)
{
	/* TODO: define fft size as its log2 value, and shift */
	return (float)idx * fs / fft_size;
}

