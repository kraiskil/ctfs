#include "board_config.h"
#include "datatype.h"
#include "peak_detect.h"
#include "croak_reply.h"
#include "target_adaptation_api.h"
#include <cstdio>

void listen_for_croaks(listen_buf_t &buffer);


int main(void)
{
	board_setup_clock();
	board_setup_gpio();
	board_setup_usart();
	board_setup_i2s_in();
	listen_buf_t    abuf;
	frequency_buf_t fbuf;
	peak_array_t    pbuf;
	croak_array_t   cbuf;
	peak_detect     pd(fbuf, pbuf);
	fft<float>      the_fft(abuf, fbuf);
	croak_reply     cr(cbuf);

	the_fft.fs = config_fs_input;
	the_fft.fft_size = abuf.size();
	pd.frequency_correction = get_input_frequency_correction();

	while (1) {
		pbuf.fill({ 0, 0 });
		cbuf.fill(NOT_A_TONE);
		listen_for_croaks(abuf);
		the_fft.dc_blocker();
		the_fft.run();
		the_fft.noise_filter(100); // 100Hz seems to be a good choice for Spanish towns.
		pd.find_peaks();


		printf("Peaks:\n");
		for (unsigned i = 0; i < pd.get_num_peaks(); i++) {
			if ((uint16_t)pd.bin_frequency(pd.get_peak_by_val(i).bin) > 5000)
				continue;
			printf("%f Hz (bin %d) - ampl: %d\n",
			    (float)pd.bin_frequency(pd.get_peak_by_val(i).bin),
			    pd.get_peak_by_val(i).bin,
			    pd.get_peak_by_val(i).val);
		}
		printf("================\n\n");

		tones t(pbuf, cbuf);
		if (t.has_croak() ) {
			printf("--- CROAK found! should reply with (enum tones)%d\n\n", cr.what_to_croak());
		}
	}

	return 0;
}

