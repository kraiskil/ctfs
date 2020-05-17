#include "config.h"
#include "datatype.h"
#include "debug.h"
#include "fft.h"
#include "frog_tones.h"
#include "harmonics.h"
#include "treefrog.h"



void treefrog(void)
{
	listen_buf_t buffer;
	while (true) {
		wallclock_start();
		listen_for_croaks(buffer);

		bool do_the_croak = should_I_croak(buffer);
		#ifdef HAVE_DEBUG_MEASUREMENTS
		total_execution_time = wallclock_time_us();
		#endif

		if (do_the_croak)
			croak();
		else
			sleep_a_bit();
		print_statistics();
	}
}

bool should_I_croak(listen_buf_t &buffer)
{
	debug_led_on(LED_PROCESSING);
	frequency_buf_t            out;
	frog_tones                 ft(buffer, out);
	fft<fft_internal_datatype> the_fft;
	the_fft.fs = config_fs_input;
	the_fft.fft_size = buffer.size();

	the_fft.dc_blocker(buffer);
	the_fft.run(buffer, out);
	ft.find_peaks();

	uint16_t harmonics[3];
	find_harmonics(ft, harmonics);

	debug_led_off(LED_PROCESSING);
	if (harmonics[0] > 200)
		return true;
	else
		return false;

}

void croak(void)
{
	// probably could do some generic
	// croaking selection

	play_croak(/*TODO: select which croak */);
}

// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void)
{
	// TODO: determine how long to sleep

	sleep_for(/*TODO pass value */);
}

void print_statistics(void)
{
#ifdef HAVE_DEBUG_MEASUREMENTS
	/* The trick with %jd and intmax_t is because %d is 16 bit on arm-eabi, and 32 on x86 -> compiler warnings */
	printf("FFT execution time: %jd ms (%jd us)\n",
	    (intmax_t)fft_execution_time / 1000, (intmax_t)fft_execution_time);
	printf("peak detetct execution time: %jd ms (%jd us)\n",
	    (intmax_t)peak_detect_execution_time / 1000, (intmax_t)peak_detect_execution_time);
	printf("total execution time: %jd ms (%jd us)\n",
	    (intmax_t)total_execution_time / 1000, (intmax_t)total_execution_time);
#endif
}

