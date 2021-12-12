#include "treefrog.h"
#include "target_adaptation_api.h"
#include <chrono>
using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
	board_init();

	while (true) {
		play_croak(G4);
		sleep_for(6s);
	}
}

