#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Lighting constants.
namespace LightingConstants
{
	constexpr uint32 MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS{ 4 };
}

//Enumeration covering all light types.
enum class LightType : uint32
{
	DIRECTIONAL,
	POINT
};