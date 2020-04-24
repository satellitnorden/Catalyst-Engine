#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/Resource.h>

class TextureCubeResource final : public Resource
{

public:

	//The texture cube handle.
	TextureCubeHandle _TextureCubeHandle{ EMPTY_HANDLE };

};