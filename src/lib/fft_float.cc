#include "datatype.h"
#include "fft.h"
#include "sin_table_float.h"

#include <cassert>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <limits>
using std::numeric_limits;


static float twiddle_re(int phase, int lenght);
static float twiddle_im(int phase, int lenght);

/* Stockham autosort FFT by okojisan from
 * http://wwwa.pikara.ne.jp/okojisan/otfft-en/stockham2.html
 * Original uses doubles and <cmath>. Fixpoint conversion by me.
 */
void fft0(int n, int s, bool eo, std::complex<float> *x, std::complex<float> *y)
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
				const std::complex<float> a = x[q + s * (p + 0)];
				const std::complex<float> b = x[q + s * (p + m)];
				y[q + s * (2 * p + 0)] =  a + b;
				std::complex<float> twiddle(twiddle_re(p, n), twiddle_im(p, n));
				//y[q + s * (2 * p + 1)] = multiply((a - b), twiddle_re(p, n), twiddle_im(p, n));
				y[q + s * (2 * p + 1)] = (a - b) * twiddle;
			}
		}
		fft0(n / 2, 2 * s, !eo, y, x);
	}
}


/* End of code from okojisan */



/* Frogs are only interested in the magnitude, not phase of result data */
void fft_calc_abs(std::complex<float> *data, frequency_buf_t &out)
{
	for (unsigned i = 0; i < out.size(); i++) {
		// sqrt() here increases computation time by 10ms (currently 10%)
		uint32_t o = sqrt(data[i].real() * data[i].real() + data[i].imag() * data[i].imag());
		static_assert(sizeof(out[0]) == 2, "output element type has been changed - change this code too");
		// TODO: compile-time assert typeid(out[0]) == typeid(uint16_t), and use the final bit of accuracy :)
		if (o > 0x7FFFF) o = 0x7FFF;
		out[i] = o;
	}
}

/* Twiddle factor, i.e. sin/cos( p*theta) values
 * used in the FFT */
static float twiddle_re(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return cos(p*theta);
	return sin_table_float[MAX_FFT_SIZE / 4 + p * stride];
}
static float twiddle_im(int p, int n)
{
	int stride = MAX_FFT_SIZE / n;
	// sanity check - N must be 2^x, x integer
	assert(__builtin_popcount(n) == 1);
	//return -sin(p*theta);
	return -sin_table_float[p * stride];
}

