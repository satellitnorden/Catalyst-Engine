#pragma once

/*
*	Declares that a function will never throw exception. Actually throwing an exception will result in immediate termination.
*/
#define NOEXCEPT noexcept

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#if defined(_MSC_VER)
		#define BREAKPOINT { __debugbreak(); }
	#elif defined(__clang__)
		#define BREAKPOINT { __asm__ volatile("int3"); }
	#endif
#else
	#define BREAKPOINT #error "This should not be in final!"
#endif

/*
*	Stronger hint than the regular inline to inline a given function. Note that this does not guarantee it will actually be inlined.
*/
#if defined(_MSC_VER)
	#define FORCE_INLINE __forceinline
#elif defined(__clang__)
	#define FORCE_INLINE __attribute__((always_inline))
#endif

/*
*	Indicates to the branch predictor that an expression is expected to most times be true.
*/
#if defined(_MSC_VER)
	#define LIKELY(expression) expression
#elif defined(__clang__)
	#define LIKELY(expression) __builtin_expect(expression, 1)
#endif

/*
*	Indicates to the compiler that a given function does not modify memory outside the first level of indirection from the function's parameters.
*/
#if defined(_MSC_VER)
	#define NOALIAS __declspec(noalias)
#elif defined(__clang__)
	#define NOALIAS 
#endif

/*
*	Indicates to the branch predictor that an expression is expected to most times not be true.
*/
#if defined(_MSC_VER)
	#define UNLIKELY(expression) expression
#elif defined(__clang__)
	#define UNLIKELY(expression) __builtin_expect(expression, 0)
#endif

/*
*	Indicates to the compiler that within the scope of the given pointer, it does not alias with other pointers of the same type in the same scope.
*/
#if defined(_MSC_VER)
	#define RESTRICT __restrict
#elif defined(__clang__)
	#define RESTRICT __restrict__
#endif

/*
*	Indicates to the compiler that the pointer returned by a given function does not alias with other pointers in the same scope.
*/
#if defined(_MSC_VER)
	#define RESTRICTED __declspec(restrict)
#elif defined(__clang__)
	#define RESTRICTED
#endif

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