#pragma once

//Platform.
#include <Platform/General/Platform.h>

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

//Core defines.
#include <Engine Core/CoreDefines.h>

//Core macros.
#include <Engine Core/CoreMacros.h>

//Core type aliases.
#include <Engine Core/CoreTypeAliases.h>

//Utilities.
#include <Engine Core/Utilities/CompileTimeUtilities.h>
#include <Engine Core/Utilities/MemoryUtilities.h>

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
#include <Engine Core/Containers/DynamicArray.h>
#include <Engine Core/DynamicString.h>
#include <Engine Core/Containers/Map.h>
#include <Engine Core/Optional.h>
#include <Engine Core/Padding.h>
#include <Engine Core/Pair.h>
#include <Engine Core/Containers/StaticArray.h>
#include <Engine Core/UniquePointer.h>

//The Catalyst project information.
#include <Engine Core/CatalystProjectInformation.h>