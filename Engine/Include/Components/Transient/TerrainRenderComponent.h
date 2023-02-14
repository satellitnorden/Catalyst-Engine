#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TerrainRenderComponent final
{

public:

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint32 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The height map texture index.
	uint32 _HeightMapTextureIndex;

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

	//The normal map texture index.
	uint32 _NormalMapTextureIndex;

};