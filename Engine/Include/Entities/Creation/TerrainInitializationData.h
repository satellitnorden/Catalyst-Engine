#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//World.
#include <World/Core/WorldPosition.h>
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class TerrainInitializationData final : public EntityInitializationData
{

public:

	/*
	*	Preprocessed data class definition.
	*/
	class PreprocessedData final
	{

	public:

		//The world space axis aligned bounding box.
		WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

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

	};

	//The world position.
	WorldPosition _WorldPosition;

	//The patch size.
	uint32 _PatchSize;

	//The height map.
	Texture2D<float32> _HeightMap;

	//The normal map.
	Texture2D<Vector3<float32>> _NormalMap;

	//The index map.
	Texture2D<Vector4<uint8>> _IndexMap;

	//The blend map.
	Texture2D<Vector4<float32>> _BlendMap;

	//The preprocessed data.
	PreprocessedData _PreprocessedData;

};