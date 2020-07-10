#include "treefrog.h"
#include "target_adaptation_api.h"

int main(int argc, char *argv[])
{
	board_init();

	while (true) {
		play_croak(C5);
		sleep_for();
		sleep_for();
		sleep_for();
	}
}

