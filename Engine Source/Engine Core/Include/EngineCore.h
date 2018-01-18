#pragma once

//Project settings.
#include <ProjectSettings.h>

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
	#undef CopyMemory
	#undef CreateSemaphore
	#undef max
	#undef min
#endif

//Core defines.
#include <CoreDefines.h>

//Core macros.
#include <CoreMacros.h>

//Core type aliases.
#include <CoreTypeAliases.h>

//Utilities.
#include <MemoryUtilities.h>

//Core types, forward declarations.
template <uint8 BitFieldSize>
class BitField;

template <class ObjectType, size_t InitialCapacity>
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