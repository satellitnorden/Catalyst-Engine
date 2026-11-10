#pragma once

//STD.
#include <iostream>

//Determine the platform.
#if defined(_WIN32)
    #define PLATFORM_WIN64
#endif

#if defined(__linux__)
    #define PLATFORM_LINUX
#endif

/*
*	Builtin types.
*/
using uint8 = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned int;
using uint64 = unsigned long long int;

using int8 = char;
using int16 = short int;
using int32 = int;
using int64 = long long int;

using byte = uint8;

using float32 = float;
using float64 = double;

/*
*	Ensure that all builtin types are indeed of the correct size.
*/
static_assert(sizeof(uint8) == 1, "Size of uint8 is not 1 bytes, how could this be?");
static_assert(sizeof(uint16) == 2, "Size of uint16 is not 2 bytes, how could this be?");
static_assert(sizeof(uint32) == 4, "Size of uint32 is not 4 bytes, how could this be?");
static_assert(sizeof(uint64) == 8, "Size of uint64 is not 8 bytes, how could this be?");

static_assert(sizeof(int8) == 1, "Size of int8 is not 1 bytes, how could this be?");
static_assert(sizeof(int16) == 2, "Size of int16 is not 2 bytes, how could this be?");
static_assert(sizeof(int32) == 4, "Size of int32 is not 4 bytes, how could this be?");
static_assert(sizeof(int64) == 8, "Size of int64 is not 8 bytes, how could this be?");

static_assert(sizeof(byte) == 1, "Size of byte is not 1 bytes, how could this be?");

static_assert(sizeof(float32) == 4, "Size of float32 is not 4 bytes, how could this be?");
static_assert(sizeof(float64) == 8, "Size of float64 is not 8 bytes, how could this be?");

/*
*	Type maximums/minimums.
*/
#define FLOAT_MAXIMUM (std::numeric_limits<float>::max())
#define FLOAT_MINIMUM (std::numeric_limits<float>::min())

#define DOUBLE_MAXIMUM (std::numeric_limits<double>::max())
#define DOUBLE_MINIMUM (std::numeric_limits<double>::min())

#define FLOAT32_MAXIMUM (FLOAT_MAXIMUM)
#define FLOAT32_MINIMUM (FLOAT_MINIMUM)

#define FLOAT64_MAXIMUM (DOUBLE_MAXIMUM)
#define FLOAT64_MINIMUM (DOUBLE_MINIMUM)

#define INT8_MAXIMUM (std::numeric_limits<int8>::max())
#define INT16_MAXIMUM (std::numeric_limits<int16>::max())
#define INT24_MAXIMUM (8'388'607)
#define INT32_MAXIMUM (std::numeric_limits<int32>::max())
#define INT64_MAXIMUM (std::numeric_limits<int64>::max())

#define INT8_MINIMUM (std::numeric_limits<int8>::min())
#define INT16_MINIMUM (std::numeric_limits<int16>::min())
#define INT24_MINIMUM (-8'388'608)
#define INT32_MINIMUM (std::numeric_limits<int32>::min())
#define INT64_MINIMUM (std::numeric_limits<int64>::min())

#define UINT8_MAXIMUM (std::numeric_limits<uint8>::max())
#define UINT16_MAXIMUM (std::numeric_limits<uint16>::max())
#define UINT24_MAXIMUM (16'777'215)
#define UINT32_MAXIMUM (std::numeric_limits<uint32>::max())
#define UINT64_MAXIMUM (std::numeric_limits<uint64>::max())

#define UINT8_MINIMUM (std::numeric_limits<uint8>::min())
#define UINT16_MINIMUM (std::numeric_limits<uint16>::min())
#define UINT24_MINIMUM (0)
#define UINT32_MINIMUM (std::numeric_limits<uint32>::min())
#define UINT64_MINIMUM (std::numeric_limits<uint64>::min())

#define BYTE_MAXIMUM (UINT8_MAXIMUM)

#define FLOAT32_EPSILON (std::numeric_limits<float32>::epsilon())
#define FLOAT64_EPSILON (std::numeric_limits<float64>::epsilon())

/*
*	Defines the maximum file path length. Matches Window's MAX_PATH currently.
*/
#define MAXIMUM_FILE_PATH_LENGTH (260)

//Macros.
#define PRINT_TO_OUTPUT(MESSAGE) std::cout << MESSAGE << std::endl;
#define BREAKPOINT() { int *x = nullptr; *x = 42; } //TODO: Crash instead of breakpoint. :x
#define ASSERT(CONDITION, MESSAGE) { if (!(CONDITION)) { PRINT_TO_OUTPUT(MESSAGE); BREAKPOINT(); } }
#define BIT(INDEX) (static_cast<uint64>(1) << static_cast<uint64>(INDEX))
#define TEST_BIT(BIT_FIELD, BIT) ((BIT_FIELD & BIT) == BIT)