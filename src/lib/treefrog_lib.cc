#include "config.h"
#include "datatype.h"
#include "debug.h"
#include "fft.h"
#include "peak_detect.h"
#include "croak_reply.h"
#include "harmonics.h"
#include "treefrog.h"


static enum note what_to_croak(listen_buf_t &buffer);

// main entry point for the full frog
void treefrog(void)
{
	listen_buf_t buffer;
	while (true) {
		wallclock_start();
		debug_led_on(LED_LISTENING);
		listen_for_croaks(buffer);
		debug_led_off(LED_LISTENING);


		debug_led_on(LED_PROCESSING);
		enum note croak = what_to_croak(buffer);
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

static enum note what_to_croak(listen_buf_t &audio_input)
{
	frequency_buf_t            spectrum;
	peak_array_t               peaks;
	croak_array_t              croaks;
	fft<fft_internal_datatype> the_fft(
	                                audio_input,
	                                spectrum,
	                                listen_buffer_samples, // fft_size
	                                config_fs_input);      // fs, sampling freq
	peak_detect                pd(spectrum, peaks);
	tones t(peaks);
	croak_reply cr(croaks);

	the_fft.dc_blocker();
	the_fft.run();
	pd.frequency_correction = get_input_frequency_correction();
	pd.find_peaks();

	if (t.has_croak() == false)
		return NOT_A_TONE;
	else
		return cr.what_to_croak();
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

