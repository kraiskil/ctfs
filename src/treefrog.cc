#include "treefrog.h"

void treefrog(void)
{
	while(true)
	{
		if( should_I_croak() )
			croak();
		else
			sleep_a_bit();
	}
}

bool should_I_croak(void)
{
	return true;
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

}

