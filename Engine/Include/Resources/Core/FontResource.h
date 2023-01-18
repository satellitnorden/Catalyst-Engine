#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

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

		//The bearing.
		Vector2<float32> _Bearing;

		//The advance.
		float32 _Advance;

		//The texture width offset start.
		float32 _TextureWidthOffsetStart;

		//The texture width offset end.
		float32 _TextureWidthOffsetEnd;

	};

	//Container for all character descriptions.
	StaticArray<CharacterDescription, NUMBER_OF_CHARACTER_DESCRIPTIONS> _CharacterDescriptions;

	//The master texture index.
	uint32 _MasterTextureIndex;

	//The default height.
	float32 _DefaultHeight;

};