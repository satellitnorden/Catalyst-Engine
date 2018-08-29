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
#include <Core/Core/CoreDefines.h>

//Core macros.
#include <Core/Core/CoreMacros.h>

//Core type aliases.
#include <Core/Core/CoreTypeAliases.h>

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

//Core types, implementations.
#include <Core/General/BinaryFile.h>
#include <Core/General/DeltaTimer.h>
#include <Core/Pointers/SharedPointer.h>
#include <Core/Pointers/UniquePointer.h>

//Platform.
#include <Platform/General/Platform.h>