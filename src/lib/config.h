#pragma once
#include "board_config.h"
#include <chrono>

/* Peak detection algorithm: a peak must be
 * this many standard deviations above the average frequency
 * bin values to be recognized as a peak.
 * The content of this variable must be changed when fft
 * scaling changes.
 */
constexpr unsigned default_peak_stddev_limit = 3; // float-fft
/* Peak detection reports the this many strongest peaks,
 * ignores the rest */
constexpr unsigned max_detected_peaks = 20;
/* Currently a tone is defined only by two peaks. */
constexpr unsigned max_detected_tones = max_detected_peaks / 2;

/* FFT scale factor:
 * when casting float-results of the fft into uin16_t
 * some precision gets lost. Scale the fft result by this
 * value to avoid having all fft bins as zero.
 */
constexpr float default_fft_scale = 1;


/* Output ADSR filter coefficients.
 * Lenghths in samples.
 * Level absolute values are scaled for the PCM5102 DAC - which lacks
 * volume control.
 */
constexpr float attack_level = 1.5;
constexpr float sustain_level = 1.0;
constexpr int   attack_len = 1600.0 /*ms*/ * config_fs_output / 1000;
constexpr int   decay_len = 1000.0 /*ms*/ * config_fs_output / 1000;
constexpr int   sustain_len = 1500.0 /*ms*/ * config_fs_output / 1000;
constexpr int   release_len = 300.0 /*ms*/ * config_fs_output / 1000;

/* The length of a croak, in audio samples.
 * This is the number of samples in a single mono channel. */
constexpr int croak_len_samples = attack_len + decay_len + sustain_len + release_len;

/* The length of a croak, in milliseconds. */
typedef std::chrono::duration<uint32_t, std::milli> milliseconds_t;
constexpr milliseconds_t croak_duration(croak_len_samples * 1000 / config_fs_output);

