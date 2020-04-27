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
	*	Returns the number of hardware threads.
	*/
	FORCE_INLINE NO_DISCARD uint32 NumberOfHardwareThreads() NOEXCEPT;

	/*
	*	CurrentThread namespace, containing common functions relating to the current thread.
	*/
	namespace CurrentThread
	{

		/*
		*	Pauses the current thread.
		*/
		FORCE_INLINE void Pause() NOEXCEPT;

		/*
		*	Yields the current thread.
		*/
		FORCE_INLINE void Yield() NOEXCEPT;

		/*
		*	Returns the current thread's index.
		*/
		FORCE_INLINE uint64 &Index() NOEXCEPT
		{
			static thread_local uint64 INDEX{ 0 };

			return INDEX;
		}

		/*
		*	Initializes the current thread's index.
		*/
		FORCE_INLINE void InitializeIndex() NOEXCEPT
		{
			static std::atomic<uint64> INDEX_COUNTER{ 0 };

			Index() = INDEX_COUNTER.fetch_add(1);
		}

	}

}

//Implementation.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsConcurrencyCore.inl>
#endif