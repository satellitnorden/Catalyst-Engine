#pragma once

/*
*	Integer types.
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
*	Ensure that all types are indeed of the correct size.
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