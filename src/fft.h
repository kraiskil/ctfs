#include <cstdint>
#include "datatype.h"
#include "sin_table.h"

/* Maximum FFT size code is prepared for. Probably also
 * the only one used. */
constexpr int MAX_FFT_SIZE = listen_buffer_samples;
static_assert(MAX_FFT_SIZE <= SIN_TABLE_RESOLUTION,
    "sin table is not accurate enough");

/* A version of FFT that calculates a real to real FFT,
 * ignoring the phase of the result. Also, since frogs
 * are more interested in the speed of calculations,
 * the square rooting of the result is omitted */
void frog_fft(listen_buf_t & in, frequency_buf_t & out);
int index_of_peak(int16_t *buf, int N);
int index_to_frequency(int idx, int fs, int fft_size);

