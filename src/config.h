#pragma once
/* Sampling frequency of input audio.
 * If you change this, change the test data too */
constexpr int config_fs_input = 8000;
constexpr int config_fs_output = 8000;
/* The total length, in samples, of a emitted croak */
constexpr int croak_num_samples = 2 * config_fs_output;

/* Peak detection algorithm: a peak must be
 * this many standard deviations above the average frequency
 * bin values to be recognized as a peak.
 * The content of this variable must be changed when fft
 * scaling changes.
 */
constexpr unsigned default_peak_stddev_limit = 7; // float-fft

/* FFT scale factor:
 * when casting float-results of the fft into uin16_t
 * some precision gets lost. Scale the fft result by this
 * value to avoid having all fft bins as zero.
 */
constexpr float default_fft_scale = 1;


/* Output ADSR filter coefficients.
 * Lenghths in samples. */
constexpr float attack_level = 3.5;
constexpr float sustain_level = 1.0;
constexpr int   attack_len = 800.0 /*ms*/ * config_fs_output / 1000;
constexpr int   decay_len = 400.0 /*ms*/ * config_fs_output / 1000;
constexpr int   sustain_len = 200.0 /*ms*/ * config_fs_output / 1000;
constexpr int   release_len = 300.0 /*ms*/ * config_fs_output / 1000;
constexpr int   croak_len = attack_len + decay_len + sustain_len + release_len;

