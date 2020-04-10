#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

class Font final
{

public:

	/*
	*	Character description definition.
	*/
	class CharacterDescription final
	{

	public:

		//The texture index.
		uint32 _TextureIndex;

		//The size.
		Vector2<float> _Size;

		//The bearing.
		Vector2<float> _Bearing;

		//The advance.
		float _Advance;

	};

	//The texture index.
	uint32 _TextureIndex;

	//Container for all character descriptions.
	StaticArray<CharacterDescription, INT8_MAXIMUM> _CharacterDescriptions;

};