#if defined(CATALYST_PLATFORM_WINDOWS)
//Intrinsics.
#include <xmmintrin.h>

/*
*	Prefetches the memory pointed to by the given pointer.
*/
template <typename TYPE>
FORCE_INLINE void Memory::Prefetch(const TYPE *const RESTRICT pointer) NOEXCEPT
{
	_mm_prefetch(reinterpret_cast<const char *const RESTRICT>(pointer), _MM_HINT_T0);
}
#endif