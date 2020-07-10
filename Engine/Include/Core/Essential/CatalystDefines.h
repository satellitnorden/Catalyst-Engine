#pragma once

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
*	Turns optimizations off.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#if defined(CATALYST_MSVC)
		#define OPTIMIZATIONS_OFF __pragma(optimize("", off))
	#else
		#define OPTIMIZATIONS_OFF
	#endif
#else
	#define OPTIMIZATIONS_OFF
#endif

/*
*	Turns optimizations on.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#if defined(CATALYST_MSVC)
		#define OPTIMIZATIONS_ON __pragma(optimize("", on))
	#else
		#define OPTIMIZATIONS_ON
	#endif
#else
	#define OPTIMIZATIONS_ON
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
#define FLOAT_MAXIMUM (std::numeric_limits<float>::max())
#define FLOAT_MINIMUM (std::numeric_limits<float>::min())

#define DOUBLE_MAXIMUM (std::numeric_limits<double>::max())
#define DOUBLE_MINIMUM (std::numeric_limits<double>::min())

#define FLOAT32_MAXIMUM (FLOAT_MAXIMUM)
#define FLOAT32_MINIMUM (FLOAT_MINIMUM)

#define FLOAT64_MAXIMUM (DOUBLE_MAXIMUM)
#define FLOAT64_MINIMUM (DOUBLE_MINIMUM)

#define INT8_MAXIMUM (std::numeric_limits<int8>::max())
#define INT16_MAXIMUM (std::numeric_limits<int16>::max())
#define INT24_MAXIMUM (8'388'607)
#define INT32_MAXIMUM (std::numeric_limits<int32>::max())
#define INT64_MAXIMUM (std::numeric_limits<int64>::max())

#define INT8_MINIMUM (std::numeric_limits<int8>::min())
#define INT16_MINIMUM (std::numeric_limits<int16>::min())
#define INT24_MINIMUM (-8'388'608)
#define INT32_MINIMUM (std::numeric_limits<int32>::min())
#define INT64_MINIMUM (std::numeric_limits<int64>::min())

#define UINT8_MAXIMUM (std::numeric_limits<uint8>::max())
#define UINT16_MAXIMUM (std::numeric_limits<uint16>::max())
#define UINT24_MAXIMUM (16'777'215)
#define UINT32_MAXIMUM (std::numeric_limits<uint32>::max())
#define UINT64_MAXIMUM (std::numeric_limits<uint64>::max())

#define UINT8_MINIMUM (std::numeric_limits<uint8>::min())
#define UINT16_MINIMUM (std::numeric_limits<uint16>::min())
#define UINT24_MINIMUM (0)
#define UINT32_MINIMUM (std::numeric_limits<uint32>::min())
#define UINT64_MINIMUM (std::numeric_limits<uint64>::min())

#define BYTE_MAXIMUM (UINT8_MAXIMUM)

#define FLOAT32_EPSILON (std::numeric_limits<float32>::epsilon())
#define FLOAT64_EPSILON (std::numeric_limits<float64>::epsilon())