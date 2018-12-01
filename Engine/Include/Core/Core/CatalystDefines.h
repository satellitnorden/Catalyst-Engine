#pragma once

//Core.
#include <Core/Core/CatalystPreDefines.h>

/*
*	Defines that a function's return value cannot be discarded by the caller.
*/
#if defined(CATALYST_CXX17)
	#define NO_DISCARD [[nodiscard]]
#else
	#define NO_DISCARD
#endif

/*
*	Declares that a function will never throw exception. Actually throwing an exception will result in immediate termination.
*/
#define NOEXCEPT noexcept

/*
*	Stronger hint than the regular inline to inline a given function. Note that this does not guarantee it will actually be inlined.
*/
#if defined(CATALYST_MSVC)
	#define FORCE_INLINE __forceinline
#elif defined(CATALYST_CLANG)
	#define FORCE_INLINE __attribute__((always_inline))
#endif

/*
*	Indicates to the compiler that a given function does not modify memory outside the first level of indirection from the function's parameters.
*/
#if defined(CATALYST_MSVC)
	#define NOALIAS __declspec(noalias)
#elif defined(__clang__)
	#define NOALIAS 
#endif

/*
*	Indicates to the compiler that within the scope of the given pointer, it does not alias with other pointers of the same type in the same scope.
*/
#if defined(CATALYST_MSVC)
	#define RESTRICT __restrict
#elif defined(CATALYST_CLANG)
	#define RESTRICT __restrict__
#endif

/*
*	Indicates to the compiler that the pointer returned by a given function does not alias with other pointers in the same scope.
*/
#if defined(CATALYST_MSVC)
	#define RESTRICTED __declspec(restrict)
#elif defined(CATALYST_CLANG)
	#define RESTRICTED
#endif

/*
*	Type maximums/minimums.
*/
#define DOUBLE_MAXIMUM (std::numeric_limits<double>::max())
#define DOUBLE_MINIMUM (std::numeric_limits<double>::min())

#define FLOAT_MAXIMUM (std::numeric_limits<float>::max())
#define FLOAT_MINIMUM (std::numeric_limits<float>::min())

#define INT8_MAXIMUM (std::numeric_limits<int8>::max())
#define INT16_MAXIMUM (std::numeric_limits<int16>::max())
#define INT32_MAXIMUM (std::numeric_limits<int32>::max())
#define INT64_MAXIMUM (std::numeric_limits<int64>::max())

#define INT8_MINIMUM (std::numeric_limits<int8>::min())
#define INT16_MINIMUM (std::numeric_limits<int16>::min())
#define INT32_MINIMUM (std::numeric_limits<int32>::min())
#define INT64_MINIMUM (std::numeric_limits<int64>::min())

#define UINT8_MAXIMUM (std::numeric_limits<uint8>::max())
#define UINT16_MAXIMUM (std::numeric_limits<uint16>::max())
#define UINT32_MAXIMUM (std::numeric_limits<uint32>::max())
#define UINT64_MAXIMUM (std::numeric_limits<uint64>::max())

#define UINT8_MINIMUM (std::numeric_limits<uint8>::min())
#define UINT16_MINIMUM (std::numeric_limits<uint16>::min())
#define UINT32_MINIMUM (std::numeric_limits<uint32>::min())
#define UINT64_MINIMUM (std::numeric_limits<uint64>::min())