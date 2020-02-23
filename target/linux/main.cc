#include "treefrog.h"
#include <chrono>
#include <thread>

int main(void)
{
	treefrog();
}


void sleep_for(void)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}
