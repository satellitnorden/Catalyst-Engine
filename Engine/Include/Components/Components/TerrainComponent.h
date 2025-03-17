#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

//Terrain.
#include <Terrain/TerrainQuadTree.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class TerrainInitializationData final : public ComponentInitializationData
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

		//The height map texture.
		Texture2DHandle _HeightMapTexture;

		//The Height map texture index.
		uint32 _HeightMapTextureIndex;

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

		//The physics actor handle.
		ActorHandle _PhysicsActorHandle;

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

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

	//The preprocessed data.
	PreprocessedData _PreprocessedData;

};

class TerrainInstanceData final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

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

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint32 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The height map texture.
	Texture2DHandle _HeightMapTexture;

	//The Height map texture index.
	uint32 _HeightMapTextureIndex;

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

	//The quad tree.
	TerrainQuadTree _QuadTree;

	//The physics actor handle.
	ActorHandle _PhysicsActorHandle;

};

class TerrainComponent final : public Component
{
	//Component declaration.
	CATALYST_COMPONENT
	(
		Terrain,
		COMPONENT_INITIALIZE()
		COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE_RENDER, 1)
		COMPONENT_PRE_PROCESS(TerrainInitializationData)
	);
};