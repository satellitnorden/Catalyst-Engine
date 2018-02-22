#pragma once

/*
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#define CATALYST_BENCHMARK_SECTION_AVERAGE(function)																										\
	{																																							\
		static DynamicArray<long long, 1'000'000> durations;																									\
		static long long averageDuration = 0;																													\
		auto timeBeforeFunction = std::chrono::high_resolution_clock::now();																					\
		function;																																				\
		durations.EmplaceSlow(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeBeforeFunction).count());		\
		averageDuration += durations.Back();																													\
		float duration = static_cast<float>(averageDuration / durations.Size()) / 1'000.0f;																		\
		PRINT_TO_CONSOLE(#function << " - " << duration << " milliseconds.");																					\
	}
#else
	#define CATALYST_BENCHMARK_SECTION_AVERAGE(function) function;
#endif

/*
*	Tracks the average execution time of a given function or section of code and prints the average execution time in non-final builds.
*	Allows for a personalized message.
*/
#if !defined(CATALYST_FINAL)
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function)																							\
	{																																							\
		static DynamicArray<long long, 1'000'000> durations;																									\
		static long long averageDuration = 0;																													\
		auto timeBeforeFunction = std::chrono::high_resolution_clock::now();																					\
		function;																																				\
		durations.EmplaceSlow(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeBeforeFunction).count());		\
		averageDuration += durations.Back();																													\
		float duration = static_cast<float>(averageDuration / durations.Size()) / 1'000.0f;																		\
		PRINT_TO_CONSOLE(message << " - " << duration << " milliseconds.");																						\
	}
#else
	#define CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE(message, function) function;
#endif

/*
*	Declares a singleton class. Must be done inside the class in the header file.
*/
#define DECLARE_SINGLETON(SingletonClass) static UniquePointer<SingletonClass> Instance;

/*
*	Defines a singleton class. Must be done in the source file.
*/
#define DEFINE_SINGLETON(SingletonClass) UniquePointer<SingletonClass> SingletonClass::Instance = new SingletonClass;

/*
*	Declares a system class. Must be done inside the class in the header file.
*/
#define DECLARE_SYSTEM(SystemClass) public: DECLARE_SINGLETON(SystemClass); SystemClass(const SystemClass &otherSystemClass) = delete; SystemClass(SystemClass &&otherSystemClass) = delete;

/*
*	defines a system class. Must be done in the source file.
*/
#define DEFINE_SYSTEM(SystemClass) DEFINE_SINGLETON(SystemClass);

/*
*	Prints a message to the console in non-final builds.
*/
#if !defined(CATALYST_FINAL)
	#define PRINT_TO_CONSOLE(message) std::cout << message << std::endl;
#endif

/*
*	Casts a value to a uint32 so that it can be used as an index. Useful for enum classes.
*/
#define INDEX(value) static_cast<uint32>(value)

/*
*	Yields the current thread.
*/
#define THREAD_YIELD() std::this_thread::yield();