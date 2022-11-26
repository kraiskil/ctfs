#include "config.h"
#include "datatype.h"
#include "sin_table.h"
#include "treefrog.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>

extern "C" {
#include <pulse/simple.h>
}

static pa_simple *rec = NULL;


void io_init(void)
{
	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = config_fs_input,
		.channels = 1
	};
	static const pa_buffer_attr attr = {
		.maxlength = listen_buffer_bytes,
		.fragsize = listen_buffer_bytes,
	};
	int                         error;
	rec = pa_simple_new(
		NULL, //?
		"treefrog",
		PA_STREAM_RECORD,
		"treefrogs.monitor", // special Pulse Audio device. Must be set up with accompanying script
		"listening",         // stream name
		&ss,
		NULL, // channel map - default
		&attr,
		&error);
	if (rec == NULL) {
		std::cerr << "error opening capture device" << std::endl;
		exit(1);
	}
}

void listen_for_croaks(listen_buf_t &buffer)
{
	int     rv;
	int     err;
	ssize_t bytes = buffer.size() * sizeof(int16_t);

	/* This doesn't work. It should drop all data in the buffer.
	*/
	rv = pa_simple_flush(rec, &err);
	if (rv) {
		std::cerr << "pa_simple_flush() failed with " << err << std::endl;
		exit(1);
	}
	/* so as a work-around, read twice. Its not like we are in a hurry here... */
	rv = pa_simple_read(rec, buffer.data(), bytes, &err);
	rv = pa_simple_read(rec, buffer.data(), bytes, &err);
	if (rv) {
		std::cerr << "pa_simple_read() failed with " << err << std::endl;
		exit(1);
	}

	/* TODO: make this Q&D "volume control" ... more robust.
	 * the FFT doesn't like values much over ~200, or the
	 * calculations saturate. This scales down the input.
	 */
	float max = 0;
	max = *std::max_element(buffer.begin(), buffer.end());
	std::cerr << "Max value from listening: " << max << std::endl;
	for (int i = 0; i < buffer.size(); i++) {
		//buffer[i] >>= 1;
		//	std::cout << std::setw(6) << buffer[i] << " ";
		//	if( i%32 == 31)
		//		std::cout << std::endl;
		buffer[i] *= 200.0 / max;
	}
}

void play_croak(enum tone tone_to_croak)
{
	int     croak_samples = croak_len_samples;
	int16_t croak_sound[croak_samples];

	std::cout << "croaking tone: " << tone_to_croak << std::endl;

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
		"treefrogs", // special Pulse Audio device. Must be set up with accompanying script
		"croaking",  // stream name
		&ss,
		NULL, // channel map - default
		NULL, // buffering opts - default
		&error);


	for (int i = 0; i < croak_samples; i++) {
		croak_sound[i] = get_croak_data(i, tone_to_croak);
	}

	pa_simple_write(pas, croak_sound, sizeof(croak_sound), NULL);

	// wait for the croak to finish playing.
	// (I don't trust the pa_simple functions to provide this anymore :))
	//pa_simple_drain(pas, &error);
	std::this_thread::sleep_for(croak_duration);
}

