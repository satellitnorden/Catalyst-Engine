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

//Core defines.
#include <Core/CoreDefines.h>

//Core macros.
#include <Core/CoreMacros.h>

//Core type aliases.
#include <Core/CoreTypeAliases.h>

//Utilities.
#include <Core/Utilities/CompileTimeUtilities.h>
#include <Core/Utilities/MemoryUtilities.h>

//Core types, forward declarations.
enum class IOMode : uint8;

template <IOMode IoMode>
class BinaryFile;

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
#include <Core/BinaryFile.h>
#include <Core/DeltaTimer.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/DynamicString.h>
#include <Core/Containers/Map.h>
#include <Core/Optional.h>
#include <Core/Padding.h>
#include <Core/Pair.h>
#include <Core/Containers/StaticArray.h>
#include <Core/UniquePointer.h>

//Platform.
#include <Platform/General/Platform.h>

//The Catalyst project information.
#include <Core/CatalystProjectInformation.h>