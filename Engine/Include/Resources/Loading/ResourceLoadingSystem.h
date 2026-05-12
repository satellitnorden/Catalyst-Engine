#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Loading/Texture3DData.h>

//Forward declarations.
class BinaryInputFile;

class ResourceLoadingSystem final
{

public:

	/*
	*	Given a file, load texture 3D data.
	*/
	void LoadTexture3D(BinaryInputFile *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT;

};