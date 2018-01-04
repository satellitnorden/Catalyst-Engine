#pragma once

//The project settings.
#include <ProjectSettings.h>

//Temporary defines.
#define CATALYST_WINDOWS
//#define CATALYST_FINAL

//STL.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>

//Windows.
#if defined(CATALYST_WINDOWS)
#include <Windows.h>
#include <Xinput.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef max
#undef min
#endif

/*
*	Alias types.
*/
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using byte = uint8;

/*
*	Preprocessor defines.
*/
#if !defined(CATALYST_FINAL)
#define BREAKPOINT __debugbreak()
#else
#define BREAKPOINT #error "This should not be in final!"
#endif

#define CATALYST_FORCE_INLINE __forceinline
#define CATALYST_LIKELY(expression) expression
#define CATALYST_NOALIAS __declspec(noalias)
#define CATALYST_NOEXCEPT noexcept
#define CATALYST_UNLIKELY(expression) expression
#define CATALYST_RESTRICT __restrict
#define CATALYST_RESTRICTED __declspec(restrict)

/*
*	Preprocessor macros.
*/
#if !defined(CATALYST_FINAL)
#define CATALYST_BENCHMARK_AVERAGE(message, function)																											\
	{																																								\
		static DynamicArray<long long> durations;																													\
		static long long averageDuration = 0;																														\
		auto timeBeforeFunction = std::chrono::high_resolution_clock::now();																						\
		function;																																					\
		durations.Emplace(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timeBeforeFunction).count());			\
		averageDuration += durations.Back();																														\
		float duration = static_cast<float>(averageDuration / durations.Size()) / 1'000.0f;																			\
		PRINT_TO_CONSOLE(message << " - " << duration << " milliseconds.");																							\
	}
#else
#define CATALYST_BENCHMARK_AVERAGE(message, function) function;
#endif

#define DECLARE_SINGLETON(SingletonClass) static UniquePointer<SingletonClass> Instance;
#define DEFINE_SINGLETON(SingletonClass) UniquePointer<SingletonClass> SingletonClass::Instance = new SingletonClass;

#if !defined(CATALYST_FINAL)
#define PRINT_TO_CONSOLE(message) std::cout << message << std::endl;
#else
#error "This should not be in final!"
#endif

#define THREAD_YIELD() std::this_thread::yield();

//Core types.
//#include "Bitfield.h"
#include <DynamicArray.h>
//#include "SharedPointer.h"
#include <StaticArray.h>
#include <UniquePointer.h>