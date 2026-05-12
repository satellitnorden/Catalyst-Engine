#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/TextureCube.h>

//Resources.
#include <Resources/Core/Texture3DResource.h>
#include <Resources/Loading/Texture3DData.h>

class ResourceCreationSystem final
{

public:

	/*
	*	Creates a texture 3D.
	*/
	void CreateTexture3D(Texture3DData *const RESTRICT data, Texture3DResource *const RESTRICT resource) NOEXCEPT;

};