#pragma once

/*
*	This file contains classes for level entries,
*	where one level entry is one entity.
*	Contains some functionality for serializing/deserializing level entries as well.
*/

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/StaticString.h>

//Entities.
#include <Entities/Core/Entity.h>

class LevelEntry final
{

public:

#if defined(CATALYST_EDITOR)
	/*
	*	Editor data class definition.
	*/
	class EditorData final
	{

	public:

		//The name.
		StaticString<64> _Name;

		//The rotation. Used in place of the quaternion rotation in WorldTransformComponent.
		EulerAngles _Rotation{ 0.0f, 0.0f, 0.0f };

	};

	//The editor data.
	EditorData _EditorData;
	
#endif

	//The entity.
	Entity *RESTRICT _Entity;

};