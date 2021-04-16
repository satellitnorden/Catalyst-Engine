#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class CollisionModelData final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		NONE,

		CONCAVE,
		CONVEX
	};

	//The type.
	Type _Type;

	//The data.
	DynamicArray<byte> _Data;

};