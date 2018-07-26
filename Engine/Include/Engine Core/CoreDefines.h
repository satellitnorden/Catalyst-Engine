#pragma once

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#define BREAKPOINT __debugbreak()
#else
	#define BREAKPOINT #error "This should not be in final!"
#endif

/*
*	Stronger hint than the regular inline to inline a given function. Note that this does not guarantee it will actually be inlined.
*/
#define FORCE_INLINE __forceinline

/*
*	Indicates to the branch predictor that an expression is expected to most times be true.
*/
#define LIKELY(expression) expression

/*
*	Indicates to the compiler that a given function does not modify memory outside the first level of indirection from the function's parameters.
*/
#define NOALIAS __declspec(noalias)

/*
*	Declares that a function will never throw exception. Actually throwing an exception will result in immediate termination.
*/
#define NOEXCEPT noexcept

/*
*	Indicates to the branch predictor that an expression is expected to most times not be true.
*/
#define UNLIKELY(expression) expression

/*
*	Indicates to the compiler that within the scope of the given pointer, it does not alias with other pointers of the same type in the same scope.
*/
#define RESTRICT __restrict

/*
*	Indicates to the compiler that the pointer returned by a given function does not alias with other pointers in the same scope.
*/
#define RESTRICTED __declspec(restrict)

/*
*	Type maximums/minimums.
*/
#define DOUBLE_MAXIMUM std::numeric_limits<double>::max()
#define DOUBLE_MINIMUM std::numeric_limits<double>::min()

#define FLOAT_MAXIMUM std::numeric_limits<float>::max()
#define FLOAT_MINIMUM std::numeric_limits<float>::min()

#define INT8_MAXIMUM std::numeric_limits<int8>::max()
#define INT16_MAXIMUM std::numeric_limits<int16>::max()
#define INT32_MAXIMUM std::numeric_limits<int32>::max()
#define INT64_MAXIMUM std::numeric_limits<int64>::max()

#define INT8_MINIMUM std::numeric_limits<int8>::min()
#define INT16_MINIMUM std::numeric_limits<int16>::min()
#define INT32_MINIMUM std::numeric_limits<int32>::min()
#define INT64_MINIMUM std::numeric_limits<int64>::min()

#define UINT8_MAXIMUM std::numeric_limits<uint8>::max()
#define UINT16_MAXIMUM std::numeric_limits<uint16>::max()
#define UINT32_MAXIMUM std::numeric_limits<uint32>::max()
#define UINT64_MAXIMUM std::numeric_limits<uint64>::max()

#define UINT8_MINIMUM std::numeric_limits<uint8>::min()
#define UINT16_MINIMUM std::numeric_limits<uint16>::min()
#define UINT32_MINIMUM std::numeric_limits<uint32>::min()
#define UINT64_MINIMUM std::numeric_limits<uint64>::min()