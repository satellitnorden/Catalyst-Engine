#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/Resource.h>

class Texture3DResource final : public Resource
{

public:

	//The texture 3D handle.
	Texture3DHandle _Texture3DHandle{ EMPTY_HANDLE };


};