#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Concurrency namespace, containing common functions relating to concurrency.
*/
namespace Concurrency
{

	/*
	*	Returns the number of hardware threads.
	*/
	NO_DISCARD uint32 NumberOfHardwareThreads() NOEXCEPT;

	/*
	*	CurrentThread namespace, containing common functions relating to the current thread.
	*/
	namespace CurrentThread
	{

		/*
		*	Pauses the current thread.
		*/
		void Pause() NOEXCEPT;

		/*
		*	Yields the current thread.
		*/
		void Yield() NOEXCEPT;

	}

}