/* Perform IO (i.e. sound input/output)
 * via audio files and sndlib
 */

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "config.h"
#include "datatype.h"

extern "C" {
#include <sndfile.h>
}

int16_t snd_pure_1kHz[listen_buffer_samples];
int16_t snd_pure_1kHz_fadein[listen_buffer_samples];
int16_t snd_zero[listen_buffer_samples];
int16_t snd_noise[listen_buffer_samples];
int16_t snd_noise_loud[listen_buffer_samples];

void read_data_file(const std::string &file, int16_t *buffer)
{
	SF_INFO sf_info;
	SNDFILE *sf = sf_open(file.c_str(), SFM_READ, &sf_info);

	if (sf == NULL) {
		std::cerr << "Error reading sound file " << file << std::endl;
		exit(1);
	}
	if (sf_info.samplerate != config_fs) {
		std::cerr << "Input data is at " << sf_info.samplerate << "Hz, expect " << config_fs << std::endl;
		exit(1);
	}
	if (sf_info.channels != 1) {
		std::cerr << "Input data has more than 1 channels" << std::endl;
		exit(1);
	}

	sf_read_short(sf, buffer, listen_buffer_samples);
}

void io_init(void)
{
	const char *datadir = getenv("CTFS_TEST_DATA");
	if (datadir == NULL) {
		std::cerr << "Environment variable CTFS_TEST_DATA not set" << std::endl;
		exit(1);
	}
	std::string dd(datadir);
	dd += "/";

	read_data_file(dd + "1kHzsine_2s.wav", snd_pure_1kHz);
	read_data_file(dd + "1kHzsine_2s_fades.wav", snd_pure_1kHz_fadein);

	memset(snd_zero, 0, sizeof(snd_zero));
	for (int i = 0; i < listen_buffer_samples; i++) {
		int r = rand() & 0xff;
		snd_noise_loud[i] = r;
		snd_noise[i] = r >> 2;
	}
}

void listen_for_croaks(listen_buf_t &buf)
{
	int16_t *buffer = buf.data();
	std::cout << "what am I hearing?" << std::endl;
	std::cout << "1) 1kHz sine" << std::endl;
	std::cout << "2) 1kHz sine fade in" << std::endl;
	std::cout << "3) noise" << std::endl;
	std::cout << "4) loud noise" << std::endl;
	std::cout << "5) just zeros" << std::endl;
	int option;
	std::cin >> option;

	int16_t *rdptr;

	switch (option)
	{
	case 1: rdptr = snd_pure_1kHz;
		break;
	case 2: rdptr = snd_pure_1kHz_fadein;
		break;
	case 3: rdptr = snd_noise;
		break;
	case 4: rdptr = snd_noise_loud;
		break;
	default:
		std::cout << "didn't understand, hearing zeros" << std::endl;
	// fall-through
	case 5: rdptr = snd_zero;
		break;
	}

	memcpy(buffer, rdptr, sizeof(int16_t) * listen_buffer_samples);
}

void play_croak(void)
{
	std::cout << "croak!" << std::endl;
}

