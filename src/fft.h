#include <cstdint>
/* A version of FFT that calculates a real to real FFT,
 * ignoring the phase of the result. Also, since frogs
 * are more interested in the speed of calculations,
 * the square rooting of the result is omitted */
void frog_fft(const int16_t*in, int16_t *out, int N);
int index_of_peak(int16_t *buf, int N);

