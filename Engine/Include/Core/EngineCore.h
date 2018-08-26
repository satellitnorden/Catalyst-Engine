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
#include <sstream>
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

class DynamicString;

template <class ObjectType>
class Optional;

template <uint64 Bytes>
class Padding;

template <class FirstType, class SecondType>
class Pair;

template <class ObjectType>
class UniquePointer;

//Core types, implementations.
#include <Core/BinaryFile.h>
#include <Core/DeltaTimer.h>
#include <Core/DynamicString.h>
#include <Core/Optional.h>
#include <Core/Padding.h>
#include <Core/Pair.h>
#include <Core/UniquePointer.h>

//Platform.
#include <Platform/General/Platform.h>

//The Catalyst project configuration.
#include <Core/CatalystProjectConfiguration.h>