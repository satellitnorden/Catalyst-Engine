#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class AtomicFlag final
{

public:

	/*
	*	Tests and sets this atomic flag.
	*/
	bool TestAndSet() NOEXCEPT { return _Flag.test_and_set(); }

	/*
	*	Clears this atomic flag.
	*/
	void Clear() NOEXCEPT { _Flag.clear(); }

private:

	//The underlying atomic flag.
	std::atomic_flag _Flag{ 0 };

};