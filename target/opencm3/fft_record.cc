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
	board_setup_wallclock();

	listen_buf_t    abuf;
	frequency_buf_t fbuf;
	frog_tones      ft(abuf, fbuf);
	uint32_t        elapsed_time;

	while (1) {
		listen_for_croaks(abuf);
		ft.dc_blocker();
		wallclock_start();
		debug_led_on(LED_PROCESSING);
		ft.fft();
		debug_led_off(LED_PROCESSING);
		elapsed_time = wallclock_time_us();
		printf("Calculated fft in %ldus\n", elapsed_time);

		for (unsigned i = 0; i < fbuf.size(); i++) {
			printf("%d ", fbuf[i]);
		}
		printf("\n\n");
	}

	return 0;
}

