#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/Asset.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureCompression.h>

class Texture2DAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The compression.
	TextureCompression _Compression;

	//The average value.
	Vector4<float32> _AverageValue;

	//The texture 2D handle.
	Texture2DHandle _Texture2DHandle{ EMPTY_HANDLE };

	//The index into the global array of texture 2D's.
	uint32 _Index{ UINT32_MAXIMUM };

	//The texture 2D.
	Texture2D<Vector4<byte>> _Texture2D;

};