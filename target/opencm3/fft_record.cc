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

int main(void)
{
	board_setup_clock();
	board_setup_gpio();
	board_setup_usart();
	board_setup_i2s_in();
	listen_buf_t    abuf;
	frequency_buf_t fbuf;
	frog_tones      ft(abuf, fbuf);

	while (1) {
		listen_for_croaks(abuf);
		ft.dc_blocker();
		ft.fft();

		for (unsigned i = 0; i < fbuf.size(); i++) {
			printf("%d ", fbuf[i]);
		}
		printf("\n\n");
	}

	return 0;
}

