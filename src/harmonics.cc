#include <algorithm>

#include "frog_tones.h"

struct binval
{
	uint16_t bin;
	uint16_t val;
};
typedef std::array<struct binval, 3> fone_array_t;

/* a is semi-sorted already, with the
 * newest value at the bottom. so use
 * a kind of one-pass bubblesort
 * to sort a into descending order.
 * This implementation (at this time)
 * is 652 bytes smaller than std::sort() :D
 */
static inline void mysort(tone_array_t &a)
{
	for (unsigned i = a.size() - 1; i > 0; i--) {
		if (a[i - 1].val < a[i].val)
			std::swap(a[i - 1], a[i]);
	}
}

void find_tones(listen_buf_t &in, uint16_t *tones)
{
	tone_array_t res;
	res.fill({ 0, 0 });
	/* "lowpass filter" the input to get rid of DC and noise.
	 * Even with the exteme values of 8kHz sample, 256 lenght
	 * FFT, bin 8 is still 250 Hz - a harmonic that is not used
	 * by frogs.
	 * Since the input to this function is a FFT spectrum of a real
	 * signal, only the first half contains information. */
	for (unsigned i = 8; i < in.size() / 2; i++) {
		if (in[i] > res[2].val) {
			res[2].bin = i;
			res[2].val = in[i];
			mysort(res);
		}
	}
	tones[0] = res[0].bin;
	tones[1] = res[1].bin;
	tones[2] = res[2].bin;
}

