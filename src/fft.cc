#include <cstdint>
#include "fft.h"
void frog_fft(const int16_t *in, int16_t *out, int N)
{}

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

