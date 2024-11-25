//Header file.
#include <Concurrency/Concurrency.h>

//Concurrency.
#include <Concurrency/Atomic.h>

/*
*	Concurrency namespace, containing common functions relating to concurrency.
*/
namespace Concurrency
{

	/*
	*	Returns the main thread's index.
	*/
	NO_DISCARD uint64 &MainThreadIndex() NOEXCEPT
	{
		static uint64 INDEX{ 0 };

		return INDEX;
	}

	/*
	*	CurrentThread namespace, containing common functions relating to the current thread.
	*/
	namespace CurrentThread
	{

		/*
		*	Initializes the current thread's index.
		*/
		void InitializeIndex() NOEXCEPT
		{
			static Atomic<uint64> INDEX_COUNTER{ 0 };

			Index() = INDEX_COUNTER.FetchAdd(1);
		}

		/*
		*	Returns the current thread's index.
		*/
		NO_DISCARD uint64 &Index() NOEXCEPT
		{
			static thread_local uint64 INDEX{ 0 };

			return INDEX;
		}

		/*
		*	Returns if the current thread is the main thread.
		*/
		NO_DISCARD bool IsMainThread() NOEXCEPT
		{
			return Index() == Concurrency::MainThreadIndex();
		}

	}

}