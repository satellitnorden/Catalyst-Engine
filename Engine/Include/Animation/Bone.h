#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/General/Matrix.h>

class Bone final
{

public:

	//The name.
	HashString _Name;

	//The index.
	uint32 _Index;

	//The bind transform.
	Matrix4x4 _BindTransform;

	//The inverse bind transform.
	Matrix4x4 _InverseBindTransform;

};