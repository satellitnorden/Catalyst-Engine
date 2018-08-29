#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class AtomicFlag final
{

public:

	/*
	*	Tests and sets this atomic flag.
	*/
	bool TestAndSet() NOEXCEPT { return flag.test_and_set(); }

	/*
	*	Clears this atomic flag.
	*/
	void Clear() NOEXCEPT { flag.clear(); }

private:

	//The underlying atomic flag.
	std::atomic_flag flag{ 0 };

};