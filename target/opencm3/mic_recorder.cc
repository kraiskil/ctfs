#include "board_config.h"
#include "datatype.h"
#include "peak_detect.h"
#include "target_adaptation_api.h"
#include <cstdio>

void listen_for_croaks(listen_buf_t &buffer);


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
		printf("% 8d, ", buf[i]);
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
	peak_array_t    pbuf;
	peak_detect     ft(fbuf, pbuf);
	fft<float>      the_fft;

	while (1) {
		listen_for_croaks(buf);
		the_fft.dc_blocker(buf);

		print_for_serialfft(buf);
		//print_as_c_header(buf);
	}

	return 0;
}

