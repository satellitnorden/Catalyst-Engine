#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Put this macro in a header file to generate an event that can be received by a singleton.
*/
#define CATALYST_EVENT(SINGLETON, NAME, ...) void NAME(__VA_ARGS__) NOEXCEPT;