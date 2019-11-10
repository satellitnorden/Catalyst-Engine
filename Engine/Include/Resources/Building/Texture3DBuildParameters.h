#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture3D.h>

class Texture3DBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The texture.
	const Texture3D<Vector4<byte>>* RESTRICT _Texture;

};