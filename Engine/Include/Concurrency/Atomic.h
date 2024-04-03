#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//STL.
#include <atomic>

//For now there's no atomic class, just use the standard library one.
template <typename TYPE>
using Atomic = std::atomic<TYPE>;