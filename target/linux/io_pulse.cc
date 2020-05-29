#include "config.h"
#include "datatype.h"
#include "sin_table.h"
#include "treefrog.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

extern "C" {
#include <pulse/simple.h>
}



void io_init(void)
{}

void listen_for_croaks(listen_buf_t &buffer)
{
	int       rv;
	int       err;
	ssize_t   bytes = buffer.size() * sizeof(int16_t);
	pa_simple *s = NULL;

	/* This doesn't work, so just open the device every time - or the
	 * data in the buffer would be old
	rv = pa_simple_flush(s, &err);
	if (rv) {
	        std::cerr << "pa_simple_flush() failed with " << err << std::endl;
	        exit(1);
	}
	*/

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = config_fs_input,
		.channels = 1
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

	rv = pa_simple_read(s, buffer.data(), bytes, &err);
	if (rv) {
		std::cerr << "pa_simple_read() failed with " << err << std::endl;
		exit(1);
	}

	/* TODO: make this Q&D "volume control" ... more robust.
	 * the FFT doesn't like values much over ~200, or the
	 * calculations saturate. This scales down the input.
	 */
	float max = 0;
	for (int i = 0; i < buffer.size(); i++) {
		//buffer[i] >>= 1;
		//	std::cout << std::setw(6) << buffer[i] << " ";
		//	if( i%32 == 31)
		//		std::cout << std::endl;
		if (buffer[i] > max)
			max = buffer[i];
	}
	max = *std::max_element(buffer.begin(), buffer.end());

	if (max > 256) {
		std::cout << "Input amplitude at " << max << ", please lower it" << std::endl;
	}

	pa_simple_free(s);
}

void play_croak(void)
{
	int     croak_samples = croak_len;
	int16_t croak_sound[croak_samples];

	std::cout << "croaking" << std::endl;

	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = config_fs_output,
		.channels = 1
	};
	int                         error;
	static pa_simple            *pas = pa_simple_new(
		NULL, //?
		"treefrog",
		PA_STREAM_PLAYBACK,
		NULL,       // select default device
		"croaking", // stream name
		&ss,
		NULL, // channel map - default
		NULL, // buffering opts - default
		&error);


	for (int i = 0; i < croak_samples; i++) {
		croak_sound[i] = get_croak_data(i);
	}

	pa_simple_write(pas, croak_sound, sizeof(croak_sound), NULL);
	////pa_simple_free(s);
}

