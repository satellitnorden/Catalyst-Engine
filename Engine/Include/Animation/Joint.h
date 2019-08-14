#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Matrix.h>

class Joint final
{

public:

	//The bind transform.
	Matrix4 _BindTransform;

	//The inverse bind transform.
	Matrix4 _InverseBindTransform;

	//The children.
	DynamicArray<Joint> _Children;

};