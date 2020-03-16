#include "datatype.h"
#include <iostream>

extern "C" {
#include <pulse/simple.h>
}

void io_init(void)
{
	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2
	};
	pa_simple                   *s = NULL;
	int                         error;
	s = pa_simple_new(
		NULL, //?
		"treefrog",
		PA_STREAM_RECORD,
		NULL,        // select default device
		"listening", // stream name
		&ss,
		NULL, // channel map - default
		NULL, // buffering opts - default
		&error);
	if (s == NULL) {
		std::cerr << "error opening capture device" << std::endl;
		exit(1);
	}
}

void listen_for_croaks(croak_buf_t &buffer)
{}

