#include "datatype.h"

// entrypoint for treefrog
void treefrog(void);

bool should_I_croak(listen_buf_t &buffer);
void croak(void);
// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void);

// Get the sample_num'th data in one emitted croak
int16_t get_croak_data(int sample_num);

// Size in samples that the platform records for detection
// algorithms.
// TODO rename to listen samples
static const int croak_buffer_size_samples = 1024;

/* Platform-level interfaces */
void play_croak(void);
void sleep_for(void);
void listen_for_croaks(listen_buf_t &buffer);

