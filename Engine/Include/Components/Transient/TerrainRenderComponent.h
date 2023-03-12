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

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

	//The normal map texture index.
	uint32 _NormalMapTextureIndex;

	//The index map texture.
	Texture2DHandle _IndexMapTexture;

	//The index map texture index.
	uint32 _IndexMapTextureIndex;

	//The blend map texture.
	Texture2DHandle _BlendMapTexture;

	//The blend map texture index.
	uint32 _BlendMapTextureIndex;

	//Denotes whether or not this terrain is visible.
	bool _Visibility;

};