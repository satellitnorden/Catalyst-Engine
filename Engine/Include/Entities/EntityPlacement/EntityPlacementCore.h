#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Entity.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//Type aliases.
using EntityPlacementFunction = void(*)(const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities);