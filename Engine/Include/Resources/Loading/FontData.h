#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/FontResource.h>

class FontData final
{

public:

	//The character descriptions.
	StaticArray<FontResource::CharacterDescription, INT8_MAXIMUM> _CharacterDescriptions;

	//Master texture width.
	uint32 _MasterTextureWidth;

	//The master texture height.
	uint32 _MasterTextureHeight;

	//The number of mipmap levels.
	uint8 _NumberOfMipmapLevels;

	//The master texture data.
	DynamicArray<DynamicArray<byte>> _MasterTextureData;

};