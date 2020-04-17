#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/Resource.h>

class ShaderResource final : public Resource
{

public:

	//The handle.
	ShaderHandle _Handle{ EMPTY_HANDLE };

};