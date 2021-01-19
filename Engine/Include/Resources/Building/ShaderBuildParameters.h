#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class ShaderBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The file path.
	const char *RESTRICT _FilePath{ nullptr };

	//The defines.
	DynamicArray<const char *const RESTRICT> _Defines;

	//The shader stage.
	ShaderStage _Stage;

};