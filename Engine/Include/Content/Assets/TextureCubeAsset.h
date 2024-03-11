#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/Asset.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TextureCube.h>

class TextureCubeAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The texture cube.
	TextureCube _TextureCube;

	//The number of mip levels.
	uint8 _MipLevels;

	//The texture cube handle.
	TextureCubeHandle _TextureCubeHandle{ EMPTY_HANDLE };

};