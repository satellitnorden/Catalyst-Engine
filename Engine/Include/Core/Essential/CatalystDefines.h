#pragma once

/*
*	Defines that a function's return value cannot be discarded by the caller.
*/
#if defined(CATALYST_CXX17) && !defined(CATALYST_CLANG)
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
*	Turns off the given warning.
*/
#if defined(CATALYST_MSVC)
	#define DISABLE_WARNING(WARNING_NUMBER) __pragma(warning(disable : WARNING_NUMBER))
#else
	#define DISABLE_WARNING(WARNING_NUMBER)
#endif

/*
*	Defines the maximum file path length. Matches Window's MAX_PATH currently.
*/
#define MAXIMUM_FILE_PATH_LENGTH (260)

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
	#define RESTRICT
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
*	Defines a variable or a function to be constexpr, but only in final configurattion.
*/
#if defined(CATALYST_CONFIGURATION_FINAL)
	#define FINAL_CONSTEXPR constexpr
#else
	#define FINAL_CONSTEXPR
#endif

/*
*	Type maximums/minimums.
*/
#define FLOAT32_MAXIMUM (3.40282347e+38f)
#define FLOAT32_MINIMUM (1.17549435e-38f)

#define FLOAT64_MAXIMUM (1.7976931348623157e+308)
#define FLOAT64_MINIMUM (2.2250738585072014e-308)

#define INT8_MAXIMUM (127)
#define INT16_MAXIMUM (32767)
#define INT24_MAXIMUM (8388607)
#define INT32_MAXIMUM (2147483647)
#define INT64_MAXIMUM (9223372036854775807)

#define INT8_MINIMUM (-128)
#define INT16_MINIMUM (-32768)
#define INT24_MINIMUM (-8388608)
#define INT32_MINIMUM (-2147483648)
#define INT64_MINIMUM (-9223372036854775808)

#define UINT8_MAXIMUM (255)
#define UINT16_MAXIMUM (65535)
#define UINT24_MAXIMUM (16777215)
#define UINT32_MAXIMUM (4294967295)
#define UINT64_MAXIMUM (18446744073709551615)

#define UINT8_MINIMUM (0)
#define UINT16_MINIMUM (0)
#define UINT24_MINIMUM (0)
#define UINT32_MINIMUM (0)
#define UINT64_MINIMUM (0)

#define BYTE_MAXIMUM (UINT8_MAXIMUM)

#define FLOAT32_EPSILON (1.19209290e-07f)
#define FLOAT64_EPSILON (2.2204460492503131e-16)