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
#include <CoreDefines.h>

//Core macros.
#include <CoreMacros.h>

//Core type aliases.
#include <CoreTypeAliases.h>

//Utilities.
#include <MemoryUtilities.h>

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
#include <BinaryFile.h>
#include <Bitfield.h>
#include <DynamicArray.h>
#include <DynamicString.h>
#include <Map.h>
#include <Optional.h>
#include <Padding.h>
#include <Pair.h>
#include <StaticArray.h>
#include <UniquePointer.h>

//The Catalyst project information.
#include <CatalystProjectInformation.h>