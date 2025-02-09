#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Concurrency/Concurrency.h>

//Intrinsics.
#include <immintrin.h>

//STL.
#include <thread>

/*
*	Concurrency namespace, containing common functions relating to concurrency.
*/
namespace Concurrency
{

	/*
	*	Returns the number of hardware threads.
	*/
	NO_DISCARD uint32 NumberOfHardwareThreads() NOEXCEPT
	{
		//Ask the standard library. (:
		const uint32 number_of_hardware_threads{ std::thread::hardware_concurrency() };

		//In the rare case that the standard library reports zero, use the minimum number of hardware threads for the Windows platform.
		return number_of_hardware_threads != 0 ? number_of_hardware_threads : ConcurrencyConstants::MINIMUM_NUMBER_OF_HARDWARE_THREADS;
	}

	/*
	*	CurrentThread namespace, containing common functions relating to the current thread.
	*/
	namespace CurrentThread
	{

		/*
		*	Pauses the current thread.
		*/
		void Pause() NOEXCEPT
		{
			_mm_pause();
		}

		/*
		*	Yields the current thread.
		*/
		void Yield() NOEXCEPT
		{
			//Just use the standard library.
			std::this_thread::yield();
		}

		/*
		*	Puts the current thread to sleep for N amount of nanoseconds.
		*/
		void SleepFor(const uint64 number_of_nanoseconds) NOEXCEPT
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(number_of_nanoseconds));
		}

	}

}
#endif