/*
 * Interface for IO abstraction
 * on the Linux/Posix target.
 */

#include "config.h"
#include "datatype.h"
#include "tones.h"

void io_init(void);
void listen_for_croaks(listen_buf_t &buf);
void play_croak(enum note t);

