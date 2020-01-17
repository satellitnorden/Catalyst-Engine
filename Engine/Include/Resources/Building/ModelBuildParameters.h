#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Matrix.h>

class ModelBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The level of details. Each entry represents one level of detail. The first element is the highest level of detail.
	DynamicArray<const char *RESTRICT> _LevelOfDetails;

	//The transformation.
	Matrix4 _Transformation{ };

	//The texture coordinate multiplier.
	float _TextureCoordinateMultiplier{ 1.0f };

	//The texture coordinate rotation.
	float _TexturCoordinateRotation{ 0.0f };

};