#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

class EditorContentSystem final
{

public:

	/*
	*	Initializes the editor content system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Create model state class definition.
	*/
	class CreateModelState final
	{

	public:

		//The input folder.
		DynamicString _InputFolder;

		//The output folder.
		DynamicString _OutputFolder;

		//The model files.
		DynamicArray<DynamicString> _ModelFiles;

		//The texture files.
		DynamicArray<DynamicString> _TextureFiles;

		//The base model file.
		DynamicString _BaseModelFile;

		//The collision model file.
		DynamicString _CollisionModelFile;

		//The albedo texture file.
		DynamicString _AlbedoTextureFile;

		//The normal map texture file.
		DynamicString _NormalMapTextureFile;

		//The displacement texture file.
		DynamicString _DisplacementTextureFile;

		//The roughness texture file.
		DynamicString _RoughnessTextureFile;

		//The albedo/thickness base mip level.
		uint32 _AlbedoThicknessBaseMipLevel{ 2 };

		//The normal map/displacement base mip level.
		uint32 _NormalMapDisplacementBaseMipLevel{ 2 };

		//The material properties base mip level.
		uint32 _MaterialPropertiesBaseMipLevel{ 3 };

	};

	//The create model state.
	CreateModelState _CreateModelState;

	/*
	*	The top bar callback.
	*/
	void TopBarCallback() NOEXCEPT;

	/*
	*	Sets up create model.
	*/
	void SetUpCreateModel() NOEXCEPT;

	/*
	*	The window callback.
	*/
	NO_DISCARD bool WindowCallback(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	Gathers files for create model.
	*/
	void CreateModelGatherFiles(const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Estimates the files for create model.
	*/
	void CreateModelEstimateFiles() NOEXCEPT;

	/*
	*	Compiles for create model.
	*/
	void CreateModelCompile() NOEXCEPT;

};

#endif
