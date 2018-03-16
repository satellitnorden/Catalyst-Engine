#pragma once

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
#include <string>
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
#include <Engine Core/CoreDefines.h>

//Core macros.
#include <Engine Core/CoreMacros.h>

//Core type aliases.
#include <Engine Core/CoreTypeAliases.h>

//Utilities.
#include <Engine Core/MemoryUtilities.h>

//Core types, forward declarations.
enum class IOMode : uint8;

template <IOMode IoMode>
class BinaryFile;

template <uint8 BitFieldSize>
class BitField;

template <class ObjectType, uint64 InitialCapacity>
class DynamicArray;

class DynamicString;

template <class KeyType, class ObjectType>
class Map;

template <class ObjectType>
class Optional;

template <uint64 Bytes>
class Padding;

template <class FirstType, class SecondType>
class Pair;

template <class ObjectType, uint64 ArraySize>
class StaticArray;

template <class ObjectType>
class UniquePointer;

//Core types, implementations.
#include <Engine Core/BinaryFile.h>
#include <Engine Core/Bitfield.h>
#include <Engine Core/DeltaTimer.h>
#include <Engine Core/DynamicArray.h>
#include <Engine Core/DynamicString.h>
#include <Engine Core/Map.h>
#include <Engine Core/Optional.h>
#include <Engine Core/Padding.h>
#include <Engine Core/Pair.h>
#include <Engine Core/StaticArray.h>
#include <Engine Core/UniquePointer.h>

//The Catalyst project information.
#include <Engine Core/CatalystProjectInformation.h>