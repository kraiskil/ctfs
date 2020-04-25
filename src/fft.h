#include <cstdint>
#include "datatype.h"
#include "sin_table.h"


/* Maximum FFT size code is prepared for. Probably also
 * the only one used. */
constexpr int MAX_FFT_SIZE = listen_buffer_samples;
static_assert(MAX_FFT_SIZE <= SIN_TABLE_RESOLUTION,
    "sin table is not accurate enough");


// Stockham Autosort Fast Fourier Transform
void fft_sa(int n, complex_t *x);
int index_of_peak(int16_t *buf, int N);
int index_to_frequency(int idx, int fs, int fft_size);
void fft_calc_abs(complex_t *data, frequency_buf_t &out);

