#pragma once

//Core.
#include <Core/Essential/CatalystPreDefines.h>

/*
*	Given a condition and a message, if the condition is false, the message will be printed and a breakpoint will be triggered.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#define ASSERT(condition, message) if (!(UNLIKELY(condition))) { PRINT_TO_OUTPUT(message); BREAKPOINT(); }
#else
	#define ASSERT(condition, message) 
#endif

/*
*	Defines the bit at the specified index.
*/
#define BIT(index) (static_cast<uint64>(1) << static_cast<uint64>(index))

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#if defined(CATALYST_MSVC)
		#define BREAKPOINT() { __debugbreak(); }
	#elif defined(CATALYST_CLANG)
		//#define BREAKPOINT() { __asm__ volatile("int3"); }
		#define BREAKPOINT()
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
*	Tracks the average execution time of a given section of code and prints the average execution time in profile builds.
*/
#if defined(CATALYST_CONFIGURATION_PROFILE)
	#define CATALYST_BENCHMARK_AVERAGE_SECTION_START()																				\
	static uint64 iterations{ 0 };																							\
	static uint64 average_duration{ 0 };																					\
	std::chrono::time_point<std::chrono::steady_clock> time_before_function{ std::chrono::high_resolution_clock::now() };
#else
	#define CATALYST_BENCHMARK_AVERAGE_SECTION_START()
#endif

/*
*	Tracks the average execution time of a given section of code and prints the average execution time in profile builds.
*/
#if defined(CATALYST_CONFIGURATION_PROFILE)
#define CATALYST_BENCHMARK_AVERAGE_SECTION_END(NAME)																											\
	average_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - time_before_function).count(); \
	++iterations;																																		\
	const float duration{ static_cast<float>(average_duration / iterations) / 1'000'000.0f };															\
	PRINT_TO_OUTPUT(NAME << " - " << duration << " milliseconds.");
#else
#define CATALYST_BENCHMARK_AVERAGE_SECTION_END(NAME)
#endif

/*
*	Clears the bit at the specified index.
*/
#define CLEAR_BIT(bitfield, bit) (bitfield = bitfield & ~(bit))

/*
*	Concatenates the arguments together.
*/
#define CONCATENATE(first, second) first##second

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
#define DEFINE_SINGLETON(CLASS) DestructorPointer<CLASS> CLASS::Instance{ new (Memory::GlobalLinearAllocator()->Allocate(sizeof(CLASS))) CLASS() };

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
return static_cast<ENUMERATION>(~UNDERLYING(enumeration));																	\
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
	#define PRINT_TO_OUTPUT(message) { std::ostringstream output; output << message << std::endl; CatalystPlatform::PrintToOutput(output.str().c_str()); }
#endif

/*
*	Sets the bit at the specified index.
*/
#define SET_BIT(bitfield, bit) (bitfield = (bitfield | bit))

/*
*	Tests the bit of the specified bitfield
*/
#define TEST_BIT(bitfield, bit) ((bitfield & bit) == bit)

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
	#define UNLIKELY(expression) __builtin_expect(expression, 0)
#endif