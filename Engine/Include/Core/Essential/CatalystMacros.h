#pragma once

//Core.
#include <Core/Essential/CatalystPreDefines.h>

/*
*	Aligns the preceeding object to N bytes.
*/
#define ALIGN(N) alignas(N)

/*
*	Calculates the length of a C-style array.
*/
#define ARRAY_LENGTH(X) (sizeof(X) / sizeof(*X))

/*
*	Given a condition and a message, if the condition is false, the message will be printed and a breakpoint will be triggered.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define ASSERT(CONDITION, FORMAT, ...) if (!(UNLIKELY(CONDITION))) { PRINT_TO_OUTPUT(FORMAT, __VA_ARGS__); BREAKPOINT(); }
#else
	#define ASSERT(CONDITION, FORMAT, ...) 
#endif

/*
*	Defines the bit at the specified index.
*/
#define BIT(INDEX) (static_cast<uint64>(1) << static_cast<uint64>(INDEX))

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define BREAKPOINT() Compiler::Breakpoint();
#else
	#define BREAKPOINT()
#endif

/*
*	Clears the bit at the specified index.
*/
#define CLEAR_BIT(BIT_FIELD, BIT) (BIT_FIELD = BIT_FIELD & ~(BIT))

/*
*	Defines bit operations for an enumeration. Must be placed in the global namespace.
*/
#define ENUMERATION_BIT_OPERATIONS(ENUMERATION)																				\
FORCE_INLINE constexpr static NO_DISCARD ENUMERATION operator|(const ENUMERATION first, const ENUMERATION second) NOEXCEPT	\
{																															\
	return static_cast<ENUMERATION>(UNDERLYING(first) | UNDERLYING(second));												\
}																															\
FORCE_INLINE constexpr static NO_DISCARD ENUMERATION operator&(const ENUMERATION first, const ENUMERATION second) NOEXCEPT	\
{																															\
	return static_cast<ENUMERATION>(UNDERLYING(first) & UNDERLYING(second));												\
}																															\
FORCE_INLINE constexpr static NO_DISCARD ENUMERATION operator~(const ENUMERATION enumeration) NOEXCEPT						\
{																															\
	return static_cast<ENUMERATION>(~UNDERLYING(enumeration));																\
}																															\
FORCE_INLINE constexpr static NO_DISCARD ENUMERATION operator<<(const ENUMERATION first, const uint64 second) NOEXCEPT		\
{																															\
	return static_cast<ENUMERATION>(UNDERLYING(first) << second);															\
}																															\
FORCE_INLINE constexpr static void operator|=(ENUMERATION &first, const ENUMERATION second) NOEXCEPT						\
{																															\
	first = first | second;																									\
}																															\
FORCE_INLINE constexpr static void operator&=(ENUMERATION &first, const ENUMERATION second) NOEXCEPT						\
{																															\
	first = first & second;																									\
}																															\

/*
*	Indicates to the branch predictor that an expression is expected to most times be true.
*/
#if defined(CATALYST_MSVC)
	#define LIKELY(expression) expression
#elif defined(CATALYST_CLANG)
	#define LIKELY(expression) __builtin_expect(expression, 1)
#endif

/*
*	Prints a message to the output in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define PRINT_TO_OUTPUT(FORMAT, ...) { CatalystPlatform::PrintToOutput(FORMAT, __VA_ARGS__); }
#else
	#define PRINT_TO_OUTPUT(FORMAT)
#endif

/*
*	Sets the bit at the specified index.
*/
#define SET_BIT(BIT_FIELD, BIT) (BIT_FIELD = (BIT_FIELD | BIT))

/*
*	Tests the bit of the specified bit field.
*/
#define TEST_BIT(BIT_FIELD, BIT) ((BIT_FIELD & BIT) == BIT)

/*
*	Casts a value to it's underlying type.
*/
#define UNDERLYING(value) (static_cast<std::underlying_type<decltype(value)>::type>(value))

/*
*	Indicates to the branch predictor that an expression is expected to most times not be true.
*/
#if defined(CATALYST_MSVC)
	#define UNLIKELY(expression) expression
#elif defined(CATALYST_CLANG)
	#define UNLIKELY(expression) expression
#endif