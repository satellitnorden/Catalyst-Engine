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
#include <thread>
#include <unordered_map>

//Windows.
#if defined(CATALYST_WINDOWS)
	#include <Windows.h>
	#include <Xinput.h>

	//Undefine annoying Windows defines. Fuck Windows. I hate everything.
	#undef CreateSemaphore
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

//Core types, forward declarations.
template <uint8 BitFieldSize>
class BitField;

template <class ObjectType>
class DynamicArray;

class DynamicString;

template <class KeyType, class ValueType>
class Map;

template <class ObjectType>
class Optional;

template <class ObjectType, size_t ArraySize>
class StaticArray;

template <class ObjectType>
class UniquePointer;

//Core types, implementations.
#include <Bitfield.h>
#include <DynamicArray.h>
#include <DynamicString.h>
#include <Map.h>
#include <Optional.h>
#include <StaticArray.h>
#include <UniquePointer.h>