#pragma once

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
*	Special types.
*/
#include <Core/General/Float16.h>
#include <Core/General/NormalizedFloat.h>