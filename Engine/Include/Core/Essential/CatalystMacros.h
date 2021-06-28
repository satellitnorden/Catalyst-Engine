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
	#define ASSERT(CONDITION, MESSAGE) if (!(UNLIKELY(CONDITION))) { PRINT_TO_OUTPUT(MESSAGE); BREAKPOINT(); }
#else
	#define ASSERT(CONDITION, MESSAGE) 
#endif

/*
*	Defines the bit at the specified index.
*/
#define BIT(INDEX) (static_cast<uint64>(1) << static_cast<uint64>(INDEX))

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#if defined(CATALYST_MSVC)
		#define BREAKPOINT() { __debugbreak(); }
	#elif defined(CATALYST_CLANG)
		#define BREAKPOINT() { __builtin_debugtrap(); }
	#endif
#else
	#define BREAKPOINT()
#endif

/*
*	Tracks the execution time of a given section of code and prints the execution time in profile builds.
*/
#if defined(CATALYST_CONFIGURATION_PROFILE)
#define CATALYST_BENCHMARK_SECTION_START()																					\
	std::chrono::time_point<std::chrono::steady_clock> time_before_function{ std::chrono::high_resolution_clock::now() };
#else
#define CATALYST_BENCHMARK_SECTION_START()
#endif

/*
*	Tracks the execution time of a given section of code and prints the execution time in profile builds.
*/
#if defined(CATALYST_CONFIGURATION_PROFILE)
#define CATALYST_BENCHMARK_SECTION_END(NAME)																																					\
	const float duration{ static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - time_before_function).count()) / 1'000'000.0f };	\
	PRINT_TO_OUTPUT(NAME << " - " << duration << " milliseconds.");
#else
#define CATALYST_BENCHMARK_SECTION_END(NAME)
#endif

/*
*	Tracks the average execution time of a given section of code and prints the average execution time in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define CATALYST_BENCHMARK_AVERAGE_SECTION_START()																		\
	static uint64 iterations{ 0 };																							\
	static uint64 average_duration{ 0 };																					\
	std::chrono::time_point<std::chrono::steady_clock> time_before_function{ std::chrono::high_resolution_clock::now() };
#else
	#define CATALYST_BENCHMARK_AVERAGE_SECTION_START()
#endif

/*
*	Tracks the average execution time of a given section of code and prints the average execution time in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
#define CATALYST_BENCHMARK_AVERAGE_SECTION_END(NAME)																									\
	average_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - time_before_function).count(); \
	++iterations;																																		\
	const float32 duration{ static_cast<float32>(average_duration / iterations) / 1'000'000.0f };														\
	PRINT_TO_OUTPUT(NAME << " - " << duration << " milliseconds.");
#else
	#define CATALYST_BENCHMARK_AVERAGE_SECTION_END(NAME)
#endif

/*
*	Clears the bit at the specified index.
*/
#define CLEAR_BIT(BIT_FIELD, BIT) (BIT_FIELD = BIT_FIELD & ~(BIT))

/*
*	Concatenates the arguments together.
*/
#define CONCATENATE(FIRST, SECOND) FIRST##SECOND

/*
*	Intentionally crashes the game.
*/
#define CRASH() { int32 *const RESTRICT x{ nullptr }; *x = 42; }

/*
*	Declares a singleton class. Must be done inside the class in the header file.
*/
#define DECLARE_SINGLETON(CLASS) public: static DestructorPointer<CLASS> Instance; CLASS(const CLASS &other) = delete; CLASS(CLASS &&other) = delete;

/*
*	Defines a singleton class. Must be done in the source file.
*/
#define DEFINE_SINGLETON(CLASS) DestructorPointer<CLASS> CLASS::Instance{ new (Memory::GlobalLinearAllocator()->Allocate(sizeof(CLASS))) CLASS() }; static_assert(sizeof(CLASS) % 8 == 0, "Singleton classes needs to be 8-byte aligned!");

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
FORCE_INLINE constexpr static void operator|=(ENUMERATION &first, const ENUMERATION second) NOEXCEPT						\
{																															\
	first = first | second;																									\
}																															\
FORCE_INLINE constexpr static void operator&=(ENUMERATION &first, const ENUMERATION second) NOEXCEPT						\
{																															\
	first = first & second;																									\
}																															\

/*
*	Given a condition and a message, if the condition is false, the message will be printed and the game will immediately crash.
*/
#define FATAL_ASSERT(condition, message) if (!(UNLIKELY(condition))) { PRINT_TO_OUTPUT(message); CRASH(); }

/*
*	Indicates to the branch predictor that an expression is expected to most times be true.
*/
#if defined(CATALYST_MSVC)
	#define LIKELY(expression) expression
#elif defined(CATALYST_CLANG)
	#define LIKELY(expression) __builtin_expect(expression, 1)
#endif

/*
*	Does a section of code exactly once.
*/
#define ONCE(code) { static bool once{ false }; if (!once) { code; once = true; } }

/*
*	Prints a message to the output in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define PRINT_TO_OUTPUT(MESSAGE) { std::ostringstream output; output << MESSAGE << std::endl; CatalystPlatform::PrintToOutput(output.str().c_str()); }
#else
	#define PRINT_TO_OUTPUT(MESSAGE)
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