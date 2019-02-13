#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>

//Type aliases.
using EntityPlacementFunction = void(*)(const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities);