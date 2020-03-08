#include <cstdint>
#include "sin_table.h"

/* Maximum FFT size code is prepared for. Probably also
 * the only one used. */
const int MAX_FFT_SIZE = SIN_TABLE_RESOLUTION;

/* A version of FFT that calculates a real to real FFT,
 * ignoring the phase of the result. Also, since frogs
 * are more interested in the speed of calculations,
 * the square rooting of the result is omitted */
void frog_fft(const int16_t*in, int16_t *out, int N);
int index_of_peak(int16_t *buf, int N);
int index_to_frequency(int idx, int fs, int fft_size);

