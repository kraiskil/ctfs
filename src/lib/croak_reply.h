#include "datatype.h"

class croak_reply
{
	croak_array_t &input;
public:
	croak_reply(croak_array_t &input)
		: input(input) {}

	enum note what_to_croak(void)
	{
		return C5;
	}
};

