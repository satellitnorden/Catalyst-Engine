#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/Resource.h>

class Texture2DResource final : public Resource
{

public:

	//The texture 2D handle.
	Texture2DHandle _Texture2DHandle{ EMPTY_HANDLE };

	//The index into the global array of texture 2D's.
	uint32 _Index{ UINT32_MAXIMUM };

	//The texture 2D.
	Texture2D<Vector4<float32>> _Texture2D;

};