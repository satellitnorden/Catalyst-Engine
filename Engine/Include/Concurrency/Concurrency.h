#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency constants.
namespace ConcurrencyConstants
{
	constexpr uint32 MINIMUM_NUMBER_OF_HARDWARE_THREADS{ 4 };
}

//Enumeration covering all wait modes.
enum class WaitMode : uint8
{
	PAUSE,
	YIELD
};

/*
*	Concurrency namespace, containing common functions relating to concurrency.
*/
namespace Concurrency
{

	/*
	*	Returns the main thread's index.
	*/
	NO_DISCARD uint64 &MainThreadIndex() NOEXCEPT;

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
		*	Initializes the current thread's index.
		*/
		void InitializeIndex() NOEXCEPT;

		/*
		*	Returns the current thread's index.
		*/
		NO_DISCARD uint64 &Index() NOEXCEPT;

		/*
		*	Returns if the current thread is the main thread.
		*/
		NO_DISCARD bool IsMainThread() NOEXCEPT;

		/*
		*	Pauses the current thread.
		*/
		void Pause() NOEXCEPT;

		/*
		*	Yields the current thread.
		*/
		void Yield() NOEXCEPT;

		/*
		*	Puts the current thread to sleep for N amount of nanoseconds.
		*/
		void SleepFor(const uint64 number_of_nanoseconds) NOEXCEPT;

	}

}