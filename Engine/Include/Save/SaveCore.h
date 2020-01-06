#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Type aliases.
using DefaultValuesCallback = void(*)(void* const RESTRICT data);
using SaveCallback = void(*)(void* const RESTRICT data);
using LoadCallback = void(*)(const void *const RESTRICT data);