#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Types/ModelFile.h>

//Math.
#include <Math/General/Matrix.h>

class ModelBuildParameters final
{

public:

	//Type aliases.
	using ProceduralFunction = void(*)(DynamicArray<ModelFile> *const RESTRICT model_files, ModelFile *const RESTRICT collision_model_file, void *const RESTRICT user_data);

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource resource identifier.
	const char *RESTRICT _ResourceIdentifier{ nullptr };

	//The level of details. Each entry represents one level of detail. The first element is the highest level of detail.
	DynamicArray<const char *RESTRICT> _LevelOfDetails;

	//The transformation.
	Matrix4x4 _Transformation{ };

	//The texture coordinate multiplier.
	float32 _TextureCoordinateMultiplier{ 1.0f };

	//The texture coordinate rotation.
	float32 _TexturCoordinateRotation{ 0.0f };

	//The procedural function. Will be called instead of loading the provided file(s) if set.
	ProceduralFunction _ProceduralFunction{ nullptr };

	//The procedural function user data.
	void *RESTRICT _ProceduralFunctionUserData{ nullptr };

	//The collision model file path.
	const char *RESTRICT _CollisionModelFilePath{ nullptr };

};