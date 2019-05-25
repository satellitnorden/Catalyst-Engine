#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	Font,
	Model,
	SoundBank,
	TextureCube,
	Texture2D,

	NumberOfResourceTypes
};