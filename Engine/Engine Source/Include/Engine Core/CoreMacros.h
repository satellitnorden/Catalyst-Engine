#pragma once

/*
*	Given a condition and a message, if the condition is false, the message will be printed and a breakpoint will be triggered.
*/
#if !defined(CATALYST_FINAL)
	#define ASSERT(condition, message) if (!condition) { PRINT_TO_CONSOLE(message); BREAKPOINT; }
#else
	#define ASSERT(condition, message) 
#endif

/*
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*	Allows for a personalized message.
*/
#if !defined(CATALYST_FINAL)
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function)																							\
	{																																							\
		static uint64 iterations = 0;																															\
		static uint64 averageDuration = 0;																														\
		auto timeBeforeFunction = std::chrono::high_resolution_clock::now();																					\
		function;																																				\
		averageDuration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeBeforeFunction).count(); 		\
		++iterations;																																			\
		float duration = static_cast<float>(averageDuration / iterations) / 1'000.0f;																			\
		PRINT_TO_CONSOLE(message << " - " << duration << " milliseconds.");																						\
	}
#else
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function) function;
#endif

/*
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*/
#define CATALYST_BENCHMARK_SECTION_AVERAGE(function) CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(#function, function);

/*
*	Declares a singleton class. Must be done inside the class in the header file.
*/
#define DECLARE_SINGLETON(SingletonClass) public: static UniquePointer<SingletonClass> Instance; SingletonClass(const SingletonClass &otherSingleton) = delete; SingletonClass(SingletonClass &&otherSingleton) = delete;

/*
*	Defines a singleton class. Must be done in the source file.
*/
#define DEFINE_SINGLETON(SingletonClass) UniquePointer<SingletonClass> SingletonClass::Instance = new SingletonClass;

/*
*	Prints a message to the console in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#define PRINT_TO_CONSOLE(message) std::cout << message << std::endl;
#endif

/*
*	Casts a value to a uint64 so that it can be used as an index. Useful for enum classes.
*/
#define INDEX(value) static_cast<uint64>(value)

/*
*	Puts the current thread to sleep.
*/
#define THREAD_SLEEP() std::this_thread::sleep_for(std::chrono::nanoseconds(1));

/*
*	Yields the current thread.
*/
#define THREAD_YIELD() std::this_thread::yield();