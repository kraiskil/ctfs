#include <cstdint>
// entrypoint for treefrog
void treefrog(void);

bool should_I_croak(int16_t *buffer);
void croak(void);
// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void);

// Size in samples that the platform records for detection
// algorithms.
static const int croak_buffer_size_samples = 1024;

/* Platform-level interfaces */
void play_croak(void);
void sleep_for(void);
void listen_for_croaks(int16_t *buffer);

