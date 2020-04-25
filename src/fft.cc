#include "datatype.h"
#include "fft.h"

#include <cassert>

/* placeholder floating point implementation */
#include <cmath>
#include <complex>

fixp twiddle_re(int phase, int lenght);
fixp twiddle_im(int phase, int lenght);
complex_t multiply(complex_t a, fixp b_re, fixp b_im);

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
	complex_t y[MAX_FFT_SIZE]; // complex_t is not "POD", so
	                           // clang won't allocate variable length stack
	fft0(n, 1, 0, x, y);
	for (int k = 0; k < n; k++) x[k] /= n;
}
/* End of code from okojisan */


/* Frogs are only interested in the magnitude, not phase of result data */
void fft_calc_abs(complex_t *data, frequency_buf_t &out)
{
	for (unsigned i = 0; i < out.size(); i++) {
		uint32_t o = data[i].real() * data[i].real() + data[i].imag() * data[i].imag();
		static_assert(sizeof(out[0]) == 2, "output element type has been changed - change this code too");
		// TODO: compile-time assert typeid(out[0]) == typeid(uint16_t), and use the final bit of accuracy :)
		if (o > 0x7FFFF) o = 0x7FFF;
		out[i] = o;
	}
}

/* Twiddle factor, i.e. sin/cos( p*theta) values
 * used in the FFT */
fixp twiddle_re(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return cos(p*theta);
	return sin_table[MAX_FFT_SIZE / 4 + p * stride];
}
fixp twiddle_im(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return -sin(p*theta);
	return -sin_table[p * stride];
}
complex_t multiply(complex_t a, fixp b_re, fixp b_im)
{
	complex_t rv;
	rv.real((a.real() * b_re - a.imag() * b_im) / FIXP_SCALE);
	rv.imag((a.real() * b_im + b_re * a.imag()) / FIXP_SCALE);
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

