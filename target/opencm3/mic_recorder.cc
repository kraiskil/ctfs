#include "board_config.h"
#include "datatype.h"
#include "frog_tones.h"
#include <cstdio>

void listen_for_croaks(listen_buf_t &buffer);

extern "C" {
int _write(int file, char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
		usart_send_blocking(USART2, buf[i]);
	return i;
}
}

void print_for_serialfft(listen_buf_t &buf)
{
	for (unsigned i = 0; i < buf.size(); i++) {
		printf("%d ", buf[i]);
	}
	printf("\n");
}

void print_as_c_header(listen_buf_t &buf)
{
	printf("/*=========*/\n");
	printf("/* Recorded from mic */\n");
	printf("int16_t mic_data[%d] = {\n", buf.size());
	for (unsigned i = 0; i < buf.size(); i++) {
		printf("%08x, ", buf[i]);
		if (i % 16 == 15)
			printf("\n");
	}
	printf("};\n");
	printf("\n");
}

int main(void)
{
	board_setup_clock();
	board_setup_gpio();
	board_setup_usart();
	board_setup_i2s_in();

	listen_buf_t    buf;
	frequency_buf_t fbuf;
	frog_tones      ft(buf, fbuf);
	fft<float>      the_fft;

	while (1) {
		listen_for_croaks(buf);
		the_fft.dc_blocker(buf);

		print_for_serialfft(buf);
	}

	return 0;
}

