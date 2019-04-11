#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class GlobalTexture2D final
{

public:

	//The actual texture 2D.
	Texture2DHandle _Texture2D{ EMPTY_HANDLE };

	//The index into the global array of texture 2D's.
	uint32 _Index{ UINT32_MAXIMUM };

};