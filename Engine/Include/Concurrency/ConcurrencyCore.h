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
	FORCE_INLINE static uint64 &MainThreadIndex() NOEXCEPT
	{
		static uint64 INDEX{ 0 };

		return INDEX;
	}

	/*
	*	Returns the number of hardware threads.
	*/
	FORCE_INLINE static NO_DISCARD uint32 NumberOfHardwareThreads() NOEXCEPT
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
		FORCE_INLINE static void Pause() NOEXCEPT;

		/*
		*	Yields the current thread.
		*/
		FORCE_INLINE static void Yield() NOEXCEPT
		{
			//Just use the standard library.
			std::this_thread::yield();
		}

		/*
		*	Puts the current thread to sleep for N amount of nanoseconds.
		*/
		FORCE_INLINE static void SleepFor(const uint64 number_of_nanoseconds) NOEXCEPT
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(number_of_nanoseconds));
		}

		/*
		*	Returns the current thread's index.
		*/
		FORCE_INLINE static uint64 &Index() NOEXCEPT
		{
			static thread_local uint64 INDEX{ 0 };

			return INDEX;
		}

		/*
		*	Initializes the current thread's index.
		*/
		FORCE_INLINE static void InitializeIndex() NOEXCEPT
		{
			static std::atomic<uint64> INDEX_COUNTER{ 0 };

			Index() = INDEX_COUNTER.fetch_add(1);
		}

		/*
		*	Returns if the current thread is the main thread.
		*/
		FORCE_INLINE static NO_DISCARD bool IsMainThread() NOEXCEPT
		{
			return Index() == Concurrency::MainThreadIndex();
		}

	}

}

//Implementation.
#if defined(CATALYST_PLATFORM_ANDROID)
	#include <Platform/Android/AndroidConcurrencyCore.inl>
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsConcurrencyCore.inl>
#endif