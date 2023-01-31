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
	board_setup_audio_in();

	listen_buf_t    abuf;
	frequency_buf_t fbuf;
	peak_array_t    pbuf;
	peak_detect     ft(fbuf, pbuf);
	fft<float>      the_fft(abuf, fbuf);

	the_fft.fs = config_fs_input;
	the_fft.fft_size = abuf.size();
	ft.frequency_correction = get_input_frequency_correction();

	while (1) {
		listen_for_croaks(abuf);
		the_fft.dc_blocker();
		the_fft.run();
		the_fft.noise_filter(100); // 100Hz seems to be a good choice for Spanish towns.
		ft.find_peaks();

		printf("Frequency bins [0-500):\n        ");
		for (unsigned k = 0; k < 20; k++)
			printf("%5d ", k);
		printf("\n--------------------------------------------------------------------");
		printf("--------------------------------------------------------------------\n");
		printf("%5d | ", 0);
		for (unsigned j = 0; j < 500; j++) {
			printf("%5d ", fbuf[j]);
			if (j % 20 == 19)
				printf("\n%5d | ", 2 * (j / 20 + 1));
		}
		printf("\n");

		for (unsigned i = 0; i < ft.get_num_peaks(); i++) {
			if ((float)ft.bin_frequency(ft.get_peak_by_val(i).bin) > 5000)
				continue;
			printf("%f Hz (bin %d) - ampl: %d\n",
			    (float)ft.bin_frequency(ft.get_peak_by_val(i).bin),
			    ft.get_peak_by_val(i).bin,
			    ft.get_peak_by_val(i).val);
		}
		printf("\n================\n");
	}

	return 0;
}

