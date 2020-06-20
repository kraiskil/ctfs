#include "treefrog.h"
#include "board_config.h"

int main(int argc, char *argv[])
{
	board_init();

	while (true) {
		play_croak(C4);
		sleep_for();
		sleep_for();
		sleep_for();
	}
}

