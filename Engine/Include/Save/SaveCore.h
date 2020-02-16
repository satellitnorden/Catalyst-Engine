#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using DefaultValuesCallback = void(*)(void* const RESTRICT data);
using LoadCallback = void(*)(const uint64 loaded_size, const void *const RESTRICT loaded_data);
using SaveSizeCallback = uint64(*)();
using SaveCallback = void(*)(void* const RESTRICT data);