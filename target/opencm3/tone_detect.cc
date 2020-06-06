#include "board_config.h"
#include "datatype.h"
#include "peak_detect.h"
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
	peak_array_t    pbuf;
	peak_detect     ft(abuf, fbuf, pbuf);
	fft<float>      the_fft;

	the_fft.fs = config_fs_input;
	the_fft.fft_size = abuf.size();

	while (1) {
		listen_for_croaks(abuf);
		the_fft.dc_blocker(abuf);
		the_fft.run(abuf, fbuf);
		the_fft.noise_filter(fbuf, 100); // 100Hz seems to be a good choice for Spanish towns.
		ft.find_peaks();

		printf("Peaks:\n");
		for (unsigned i = 0; i < ft.get_num_peaks(); i++) {
			if ((uint16_t)ft.bin_frequency(ft.get_peak_by_val(i).bin) > 5000)
				continue;
			printf("%f Hz (bin %d) - ampl: %d\n",
			    (float)ft.bin_frequency(ft.get_peak_by_val(i).bin),
			    ft.get_peak_by_val(i).bin,
			    ft.get_peak_by_val(i).val);
		}
		printf("================\n\n");

		tones t(pbuf);
		if (t.has_croak() ) {
			printf("--- CROAK found! should reply with (enum tones)%d\n\n", t.what_to_croak());
		}
	}

	return 0;
}

