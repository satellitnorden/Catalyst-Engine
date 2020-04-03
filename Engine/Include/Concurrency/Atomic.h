#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//For now there's no atomic class, just use the standard library one.
template <typename TYPE>
using Atomic = std::atomic<TYPE>;