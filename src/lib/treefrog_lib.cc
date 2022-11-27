#include "config.h"
#include "datatype.h"
#include "debug.h"
#include "fft.h"
#include "peak_detect.h"
#include "harmonics.h"
#include "treefrog.h"


static enum tone what_to_croak(listen_buf_t &buffer);

void treefrog(void)
{
	listen_buf_t buffer;
	while (true) {
		wallclock_start();
		debug_led_on(LED_LISTENING);
		listen_for_croaks(buffer);
		debug_led_off(LED_LISTENING);


		debug_led_on(LED_PROCESSING);
		enum tone croak = what_to_croak(buffer);
		debug_led_off(LED_PROCESSING);
		#ifndef NDEBUG
		total_execution_time = wallclock_time_us();
		#endif

		if (croak != NOT_A_TONE) {
			debug_led_on(LED_CROAK);
			play_croak(croak);
			debug_led_off(LED_CROAK);
			// Ignore replies to our croak;
			sleep_for(2 * croak_duration);
		}
		else
			sleep_for(croak_duration / 2);

		print_statistics();
	}
}

static enum tone what_to_croak(listen_buf_t &buffer)
{
	frequency_buf_t            out;
	peak_array_t               peaks;
	peak_detect                pd(buffer, out, peaks);
	tones                      t(peaks);
	fft<fft_internal_datatype> the_fft;
	the_fft.fs = config_fs_input;
	the_fft.fft_size = buffer.size();
	pd.frequency_correction = get_input_frequency_correction();

	the_fft.dc_blocker(buffer);
	the_fft.run(buffer, out);
	pd.find_peaks();


	if (t.has_croak() == false)
		return NOT_A_TONE;
	else
		return t.what_to_croak();
}

// frog-level sleep, when there are no croaks to join
void sleep_a_bit(void)
{
	debug_led_on(LED_SLEEP);
	sleep_for(8s);
	debug_led_off(LED_SLEEP);
}

void print_statistics(void)
{
#ifndef NDEBUG
	/* The trick with %jd and intmax_t is because %d is 16 bit on arm-eabi, and 32 on x86 -> compiler warnings */
	printf("\tFFT execution time: %jd ms (%jd us)\n",
	    (intmax_t)fft_execution_time / 1000, (intmax_t)fft_execution_time);
	printf("\tpeak detetct execution time: %jd ms (%jd us)\n",
	    (intmax_t)peak_detect_execution_time / 1000, (intmax_t)peak_detect_execution_time);
	printf("\ttotal execution time: %jd ms (%jd us)\n",
	    (intmax_t)total_execution_time / 1000, (intmax_t)total_execution_time);
#endif
}

