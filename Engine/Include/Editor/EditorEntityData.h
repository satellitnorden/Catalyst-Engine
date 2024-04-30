#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>
#include <Core/General/StaticString.h>

//Math.
#include <Math/General/EulerAngles.h>

class EditorEntityData final
{

public:

	/*
	*	Hash string data class definition.
	*/
	class HashStringData final
	{

	public:

		//The string.
		StaticString<128> _String;

		//The component identifier.
		HashString _ComponentIdentifier;

		//The editable field identifier.
		HashString _EditableFieldIdentifier;

	};

	//The name.
	StaticString<128> _Name;

	//The identifier.
	uint64 _Identifier;

	//The rotation. Used in place of the quaternion rotation in WorldTransformComponent.
	EulerAngles _Rotation{ 0.0f, 0.0f, 0.0f };

	//The links.
	DynamicArray<uint64> _Links;

	//The hash string data.
	DynamicArray<HashStringData> _HashStringData;

	//Denotes whether or not this entity should be serialized.
	bool _Serialize{ true };

};

#endif
