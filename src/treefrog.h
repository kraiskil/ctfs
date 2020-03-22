#include "datatype.h"

// entrypoint for treefrog
void treefrog(void);

bool should_I_croak(listen_buf_t &buffer);
void croak(void);
// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void);

// Get the sample_num'th data in one emitted croak
int16_t get_croak_data(int sample_num);

/* Platform-level interfaces */
void play_croak(void);
void sleep_for(void);
void listen_for_croaks(listen_buf_t &buffer);

