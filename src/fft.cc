#include "fft.h"

/* placeholder floating point implementation */
#include <cmath>
extern "C" {
#include <fftw3.h>
}
void frog_fft(const int16_t *in, int16_t *out, int fft_size)
{

	fftw_complex *fc_out;
	fftw_complex *fc_in;
	fc_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_size);
	fc_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_size);

	for (int i = 0; i < fft_size; i++) {
		fc_in[i][0] = in[i];
		fc_in[i][1] = 0;
	}
	fftw_plan p = fftw_plan_dft_1d(fft_size, fc_in, fc_out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);

	for (int i = 0; i < fft_size; i++) {
		double o = sqrt(fc_out[i][0] * fc_out[i][0]
		        + fc_out[i][1] * fc_out[i][1]);
		out[i] = o;
	}
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

