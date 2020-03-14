#pragma once
#include <array>
#include <cstdint>

typedef int16_t fixp;
// TODO 0x8000 is nicer to divide with, but that would
// cause overflows. Perhaps 0x4000?
#define FIXP_SCALE 0x4000

constexpr unsigned croak_buffer_size = 2048;
typedef std::array<int16_t, croak_buffer_size> croak_buf_t;

