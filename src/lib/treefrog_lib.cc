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
		listen_for_croaks(buffer);

		debug_led_on(LED_PROCESSING);
		enum tone croak = what_to_croak(buffer);
		debug_led_off(LED_PROCESSING);
		#ifdef HAVE_DEBUG_MEASUREMENTS
		total_execution_time = wallclock_time_us();
		#endif

		if (croak != NOT_A_TONE)
			play_croak(croak);
		else
			sleep_a_bit();
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
	sleep_for(3s);
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

