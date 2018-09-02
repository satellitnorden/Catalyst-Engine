#pragma once

//Core.
#include <Core/Core/CatalystPreDefines.h>

/*
*	Given a condition and a message, if the condition is false, the message will be printed and a breakpoint will be triggered.
*/
#if !defined(CATALYST_FINAL)
	#define ASSERT(condition, message) if (!(UNLIKELY(condition))) { PRINT_TO_OUTPUT(message); BREAKPOINT(); }
#else
	#define ASSERT(condition, message) 
#endif

/*
*	Defines the bit at the specified index.
*/
#define BIT(index) (static_cast<uint64>(1) << index)

/*
*	Sets a breakpoint in the code in non-final builds.
*/
#if !defined(CATALYST_FINAL)
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
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*	Allows for a personalized message.
*/
#if !defined(CATALYST_FINAL)
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function)																							\
	{																																							\
		static uint64 iterations{ 0 };																															\
		static uint64 averageDuration{ 0 };																														\
		std::chrono::time_point<std::chrono::steady_clock> timeBeforeFunction{ std::chrono::high_resolution_clock::now() };										\
		function;																																				\
		averageDuration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeBeforeFunction).count(); 		\
		++iterations;																																			\
		float duration{ static_cast<float>(averageDuration / iterations) / 1'000.0f };																			\
		PRINT_TO_OUTPUT(message << " - " << duration << " milliseconds.");																						\
	}
#else
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function) function;
#endif

/*
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*/
#define CATALYST_BENCHMARK_SECTION_AVERAGE(function) CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(#function, function);

/*
*	Concatenates the arguments together.
*/
#define CONCATENATE(first, second) first##second

/*
*	Declares a singleton class. Must be done inside the class in the header file.
*/
#define DECLARE_SINGLETON(SingletonClass) public: static UniquePointer<SingletonClass> Instance; SingletonClass(const SingletonClass &otherSingleton) = delete; SingletonClass(SingletonClass &&otherSingleton) = delete;

/*
*	Defines a singleton class. Must be done in the source file.
*/
#define DEFINE_SINGLETON(SingletonClass) UniquePointer<SingletonClass> SingletonClass::Instance{ new SingletonClass };

/*
*	Casts a value to a uint64 so that it can be used as an index. Useful for enum classes.
*/
#define INDEX(value) static_cast<uint64>(value)

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
#if !defined(CATALYST_FINAL)
	#define PRINT_TO_OUTPUT(message) { std::ostringstream output; output << message << std::endl; CatalystPlatform::PrintToOutput(output.str().c_str()); }
#endif

/*
*	Indicates to the branch predictor that an expression is expected to most times not be true.
*/
#if defined(CATALYST_MSVC)
	#define UNLIKELY(expression) expression
#elif defined(CATALYST_CLANG)
	#define UNLIKELY(expression) __builtin_expect(expression, 0)
#endif