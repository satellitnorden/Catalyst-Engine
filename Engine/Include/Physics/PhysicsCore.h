#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Physics constants.
namespace PhysicsConstants
{
	constexpr float GRAVITATIONAL_SPEED{ 9.78'049f };
	constexpr Vector3<float> GRAVITATIONAL_DIRECTION{ 0.0f, -1.0f, 0.0f };
}

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	DynamicPhysical = BIT(0),
	Ocean = BIT(1),
	Terrain = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);