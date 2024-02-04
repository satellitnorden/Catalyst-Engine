#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/Geometry/AxisAlignedBoundingBox2D.h>

//Resources.
#include <Resources/Core/Resource.h>

class FontResource final : public Resource
{

public:

	//Constants.
	constexpr static uint8 NUMBER_OF_CHARACTER_DESCRIPTIONS{ 127 };

	/*
	*	Character description definition.
	*/
	class CharacterDescription final
	{

	public:

		//The size.
		Vector2<float32> _Size;

		//The offset.
		Vector2<float32> _Offset;

		//The texture bounds.
		AxisAlignedBoundingBox2D _TextureBounds;

		//The advance.
		float32 _Advance;

	};

	//Container for all character descriptions.
	StaticArray<CharacterDescription, NUMBER_OF_CHARACTER_DESCRIPTIONS> _CharacterDescriptions;

	//The master texture index.
	uint32 _MasterTextureIndex;

	//The default height.
	float32 _DefaultHeight;

};