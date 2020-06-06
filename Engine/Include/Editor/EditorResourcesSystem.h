#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Building/Texture2DBuildParameters.h>

class EditorResourcesSystem final
{

public:

	/*
	*	Updates the editor resources system.
	*/
	void Update() NOEXCEPT;

private:

	//Enumeration covering all create resource modes.
	enum class CreateResourceMode : uint8
	{
		NONE,
		MODEL,
		TEXTURE_2D
	};

	/*
	*	Create model resource data class definition.
	*/
	class CreateModelResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The level of detail file paths.
		DynamicArray<DynamicString> _LevelOfDetailFilepaths;

	};

	/*
	*	Create texture 2D resource data class definition.
	*/
	class CreateTexture2DResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		//The file 1 file path.
		DynamicString _File1FilePath;

		//The file 2 file path.
		DynamicString _File2FilePath;

		//The file 3 file path.
		DynamicString _File3FilePath;

		//The file 4 file path.
		DynamicString _File4FilePath;

		//The default.
		Vector4<float32> _Default;

		//The channel mappings.
		StaticArray<Texture2DBuildParameters::ChannelMapping, 4> _ChannelMappings;

		//Denotes whether or not to apply gamma correction.
		bool _ApplyGammaCorrection{ false };

		//The base mipmap level.
		int32 _BaseMipmapLevel{ 0 };

		//The number of mipmap levels.
		int32 _NumberOfMipmapLevels{ 1 };

	};

	//The current create resource mode.
	CreateResourceMode _CurrentCreateResourceMode{ CreateResourceMode::NONE };

	//The create model resource data.
	CreateModelResourceData _CreateModelResourceData;

	//The create texture 2D resource data.
	CreateTexture2DResourceData _CreateTexture2DResourceData;

};
#endif
