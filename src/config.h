#pragma once
/* Sampling frequency of input audio.
 * If you change this, change the test data too */
constexpr int config_fs_input = 8000;
constexpr int config_fs_output = 8000;
/* The total length, in samples, of a emitted croak */
constexpr int croak_num_samples = 2 * config_fs_output;

