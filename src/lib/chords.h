#include "datatype.h"

class triad
{
public:
	// Can this triad be played out of the tones in the given array?
	bool is_matching(const croak_array_t &t)
	{
		if (t[0] == NOT_A_TONE) // No sounds to match. Won't do anything.
			return false;
		else if (t[1] == NOT_A_TONE) // Just one tone. Match by default
			return true;
		else if (t[2] != NOT_A_TONE) // Already three tones. Nothing to do anymore.
			return false;
		else
			return is_3rd(t[0], t[1]);
	}

	enum note get_tone(const croak_array_t &t)
	{
		if (t[1] == NOT_A_TONE) // Just one tone. Match by default
			return get_3rd(t[0]);
		else if (t[2] == NOT_A_TONE) // Already three tones. Nothing to do anymore.
			return get_5th(t[0]);
		else
			return NOT_A_TONE;
	}

	bool is_3rd(enum note root, enum note other)
	{
		return other == get_3rd(root);
	}
	virtual enum note get_3rd(enum note root) const = 0;
	virtual enum note get_5th(enum note root) const = 0;
};

class major : public triad
{
public:
	virtual enum note get_3rd(enum note root) const
	{
		return static_cast<enum note>(root + 4);
	}
	virtual enum note get_5th(enum note root) const
	{
		return static_cast<enum note>(root + 7);
	}
};

