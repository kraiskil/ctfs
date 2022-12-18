/*
 * (currently) simplistic algorithm to
 * figure out what tone to play next.
 */
#include "chords.h"
#include "datatype.h"
#include "debug.h"

#include <algorithm>
#include <cstdlib>

class croak_reply
{
	croak_array_t &input;
	int steps_of_silence;
	// TODO:
	major M;
public:
	croak_reply(croak_array_t &input)
		: input(input)
	{
		steps_of_silence = 3;
	}

	bool input_is_silence(void)
	{
		return input[0] == NOT_A_TONE;
	}

	enum note generate_random_seednote(void)
	{
		// Generate note between [A3,G4]
		unsigned offset = rand() % 10;
		return static_cast<enum note>(A3 + offset);
	}

	enum note maybe_break_silence(void)
	{
		// TODO: randomize the delay
		steps_of_silence--;
		if (steps_of_silence < 0) {
			DB_PRINT("Breaking silence!\n");
			steps_of_silence = 10;
			return generate_random_seednote();
		}
		else
			return NOT_A_TONE;

	}

	enum note what_to_croak(void)
	{
		std::sort(input.begin(), input.end());

		if (input_is_silence() )
			return maybe_break_silence();

		if (M.is_matching(input) )
			return M.get_tone(input);


		return NOT_A_TONE;
	}
};

