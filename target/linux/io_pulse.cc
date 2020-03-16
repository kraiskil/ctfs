#include "datatype.h"
#include <iostream>

extern "C" {
#include <pulse/simple.h>
}


static pa_simple *s = NULL;

void io_init(void)
{
	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2
	};
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
{
	int     rv;
	int     err;
	ssize_t bytes = buffer.size() * sizeof(int16_t);

	rv = pa_simple_read(s, buffer.data(), bytes, &err);

	if (rv) {
		std::cerr << "pa_simple_read() failed with " << err << std::endl;
		exit(1);
	}

	/* TODO: make this Q&D "volume control" ... more robust.
	 * the FFT doesn't like values much over ~200, or the
	 * calculations saturate. This scales down the input.
	 */
	for (int i = 0; i < buffer.size(); i++) {
		buffer[i] >>= 6;
	}
}

