#pragma once
#include <cstdint>
/* The number of samples per one sine period in the table */
const int SIN_TABLE_RESOLUTION = 2048;
/* 1 and a quarter wave - for easier lookup code */
const int      SIN_TABLE_N_ELEM = 5 * SIN_TABLE_RESOLUTION / 4;
extern int16_t sin_table[SIN_TABLE_N_ELEM];

