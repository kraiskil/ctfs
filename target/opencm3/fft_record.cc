#include "board_config.h"
#include "datatype.h"
#include "peak_detect.h"
#include "target_adaptation_api.h"
#include <cstdio>

void listen_for_croaks(listen_buf_t &buffer);

int main(void)
{
	board_setup_clock();
	board_setup_gpio();
	board_setup_usart();
	board_setup_i2s_in();
	board_setup_wallclock();

	listen_buf_t    abuf;
	frequency_buf_t fbuf;
	peak_array_t    pbuf;
	peak_detect     ft(abuf, fbuf, pbuf);
	uint32_t        elapsed_time;
	fft<float>      the_fft;
	the_fft.fs = config_fs_input;
	the_fft.fft_size = abuf.size();

	while (1) {
		listen_for_croaks(abuf);
		the_fft.dc_blocker(abuf);
		wallclock_start();
		debug_led_on(LED_PROCESSING);
		the_fft.run(abuf, fbuf);
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

