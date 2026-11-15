#if defined(CATALYST_PLATFORM_ANDROID)

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
			//Do nothing. For now, I guess. (:
		}

	}

}
#endif