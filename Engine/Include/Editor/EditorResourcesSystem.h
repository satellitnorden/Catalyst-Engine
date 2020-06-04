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
		MODEL
	};

	/*
	*	Create model resource data class definition.
	*/
	class CreateModelResourceData final
	{

	public:

		//The output file path.
		DynamicString _OutputFilePath;

		DynamicArray<DynamicString> _LevelOfDetailFilepaths;

	};

	//The current create resource mode.
	CreateResourceMode _CurrentCreateResourceMode{ CreateResourceMode::NONE };

	//The create model resource data.
	CreateModelResourceData _CreateModelResourceData;

};
#endif
