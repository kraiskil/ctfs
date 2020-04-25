#include "board_config.h"
#include "datatype.h"
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

	while (1) {
		listen_buf_t buf;
		listen_for_croaks(buf);

		for (unsigned i = 0; i < buf.size(); i++) {
			printf("%d ", buf[i]);
		}
		printf("\n");
	}

	return 0;
}

