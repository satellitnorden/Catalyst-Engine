#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/General/Matrix.h>

class Bone final
{

public:

	//The name.
	HashString _Name;

	//The bind transform.
	Matrix4 _BindTransform;

	//The inverse bind transform.
	Matrix4 _InverseBindTransform;

	//The children.
	DynamicArray<Bone> _Children;

};