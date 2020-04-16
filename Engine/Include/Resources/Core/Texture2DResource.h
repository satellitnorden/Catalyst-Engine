#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#include <Rendering/Native/Texture2D.h>
#endif

//Resources.
#include <Resources/Core/Resource.h>

class Texture2DResource final : public Resource
{

public:

	//The texture 2D handle.
	Texture2DHandle _Texture2DHandle{ EMPTY_HANDLE };

	//The index into the global array of texture 2D's.
	uint32 _Index{ UINT32_MAXIMUM };

#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
	//The texture 2D.
	Texture2D<Vector4<float>> _Texture2D;
#endif

};