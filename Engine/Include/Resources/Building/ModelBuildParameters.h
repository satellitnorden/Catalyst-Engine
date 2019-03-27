#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

class ModelBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource id.
	const char *RESTRICT _ID{ nullptr };

	//The file.
	const char *RESTRICT _File{ nullptr };

	//The transformation.
	Matrix4 _Transformation{ };

	//The texture coordinate multiplier.
	float _TextureCoordinateMultiplier{ 1.0f };

	//The texture coordinate rotation.
	float _TexturCoordinateRotation{ 0.0f };

};