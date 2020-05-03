#include "treefrog.h"
#include <chrono>
#include <thread>

void io_init(void);

int main(int argc, char *argv[])
{
	io_init();
	while (true) {
		play_croak();
		sleep_for();
	}
}


void sleep_for(void)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

