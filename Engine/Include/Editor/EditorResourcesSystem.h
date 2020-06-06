#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

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

	};

	//The current create resource mode.
	CreateResourceMode _CurrentCreateResourceMode{ CreateResourceMode::NONE };

	//The create model resource data.
	CreateModelResourceData _CreateModelResourceData;

	//The create texture 2D resource data.
	CreateTexture2DResourceData _CreateTexture2DResourceData;

};
#endif
