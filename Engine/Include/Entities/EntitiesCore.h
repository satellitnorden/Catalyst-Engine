#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class AxisAlignedBoundingBox;
template <typename Type>
class DynamicArray;
class Entity;

//Type aliases.
using EntityProceduralPlacementFunction = void(*)(const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities);