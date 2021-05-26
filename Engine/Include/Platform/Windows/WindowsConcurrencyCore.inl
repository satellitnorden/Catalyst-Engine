#if defined(CATALYST_PLATFORM_WINDOWS)

//Intrinsics.
#include <immintrin.h>

/*
*	Concurrency namespace, containing common functions relating to concurrency.
*/
namespace Concurrency
{

	/*
	*	CurrentThread namespace, containing common functions relating to the current thread.
	*/
	namespace CurrentThread
	{

		/*
		*	Pauses the current thread.
		*/
		FORCE_INLINE static void Pause() NOEXCEPT
		{
			_mm_pause();
		}

	}

}
#endif