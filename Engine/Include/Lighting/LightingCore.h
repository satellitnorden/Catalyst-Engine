#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Enumeration.h>

//Lighting constants.
namespace LightingConstants
{
	constexpr uint32 MAXIMUM_NUMBER_OF_SHADOW_CASTING_LIGHTS{ 4 };
}

//Enumeration covering all light types.
CATALYST_ENUMERATION
(
	LightType,
	uint8,
	DIRECTIONAL,
	POINT
);