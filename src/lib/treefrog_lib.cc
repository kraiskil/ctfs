#include "config.h"
#include "datatype.h"
#include "debug.h"
#include "fft.h"
#include "peak_detect.h"
#include "croak_reply.h"
#include "harmonics.h"
#include "treefrog.h"


frequency_buf_t spectrum;
peak_array_t    peaks;
croak_array_t   croaks;
peak_detect     pd(spectrum, peaks);
tones           t(peaks, croaks);
croak_reply     cr(croaks);



static enum note what_to_croak(listen_buf_t &buffer);
static enum note listen_and_process(void);

// helper function to tune delays to sync the sounds
static void sleep_before_listening(void);
static void sleep_before_croaking(void);
static void sleep_after_croaking(void);
static void sleep_when_silent(void);


// main entry point for the full frog
void treefrog(void)
{
	enum note croak;

	while (true) {
		wallclock_start();

		sleep_before_listening();
		croak = listen_and_process();

		if (croak != NOT_A_TONE) {
			sleep_before_croaking();
			debug_led_on(LED_CROAK);
			play_croak(croak);
			debug_led_off(LED_CROAK);
			sleep_after_croaking();
		}
		else
			sleep_when_silent();

	}
}

static enum note listen_and_process(void)
{
	listen_buf_t buffer;
	debug_led_on(LED_LISTENING);
	listen_for_croaks(buffer);
	debug_led_off(LED_LISTENING);

	debug_led_on(LED_PROCESSING);
	peaks.fill({ 0, 0 });    // Don't save history of the soundscape
	croaks.fill(NOT_A_TONE); // Don't save history of the soundscape
	enum note croak = what_to_croak(buffer);
	debug_led_off(LED_PROCESSING);
	#ifndef NDEBUG
	total_execution_time = wallclock_time_us();
	//print_statistics();
	#endif
	return croak;
}

static enum note what_to_croak(listen_buf_t &audio_input)
{
	// TODO: make up my mind. Should audio_input be a global or not?
	fft<fft_internal_datatype> the_fft(
		audio_input,
		spectrum,
		listen_buffer_samples, // fft_size
		config_fs_input);      // fs, sampling freq
	the_fft.dc_blocker();
	the_fft.run();
	pd.frequency_correction = get_input_frequency_correction();
	pd.find_peaks();
	t.detect_tones();
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

static void sleep_before_listening(void)
{
	milliseconds_t sleep(1000 + rand() % 2000);
	sleep_for(sleep);
}
static void sleep_before_croaking(void)
{
	milliseconds_t sleep(croak_duration / 4);
	sleep_for(sleep);
}
static void sleep_after_croaking(void)
{
	milliseconds_t sleep(rand() % 2000);
	sleep_for(sleep);
	sleep_for(2 * croak_duration);
}
static void sleep_when_silent(void)
{
	milliseconds_t sleep(croak_duration / 8);
	sleep_for(sleep);
}

