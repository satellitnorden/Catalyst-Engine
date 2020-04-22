#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/FontResource.h>

class FontData final
{

public:

	//The character descriptions.
	StaticArray<FontResource::CharacterDescription, INT8_MAXIMUM> _CharacterDescriptions;

	//The character dimensions.
	StaticArray<Vector2<uint32>, INT8_MAXIMUM> _CharacterDimensions;

	//The texture data.
	StaticArray<DynamicArray<Texture2D<byte>>, INT8_MAXIMUM> _TextureData;

};