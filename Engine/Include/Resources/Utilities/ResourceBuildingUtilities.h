#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

namespace ResourceBuildingUtilities
{

	/*
	*	Calculates the average color of the texture at the given file path.
	*/
	NO_DISCARD Vector4<float32> CalculateAverageColor(const char *const RESTRICT texture_file_path) NOEXCEPT;

	/*
	*	Build impostor material parameters class definition.
	*/
	class BuildImpostorMaterialParameters final
	{

	public:

		//The model file path.
		const char *RESTRICT _ModelFilePath;

		//The model transformation.
		Matrix4x4 _ModelTransformation;

		//The albedo texture file paths.
		StaticArray<const char *RESTRICT, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _AlbedoTextureFilePaths;

		//The normal map texture file paths.
		StaticArray<const char *RESTRICT, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _NormalMapTextureFilePaths;

		//The mask texture file paths.
		StaticArray<const char *RESTRICT, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaskTextureFilePaths;

		//The texture dimensions.
		Resolution _Dimensions;

		//The idenfitier.
		const char *RESTRICT _Identifier;

		//The raw textures file path.
		const char *RESTRICT _RawTexturesFilePath;

	};

	/*
	*	Creates an impostor material, with the associated textures.
	*/
	void BuildImpostorMaterial(const BuildImpostorMaterialParameters &parameters) NOEXCEPT;

}
#endif