//Header file.
#include <Entities/Types/TerrainEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/TerrainInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>

/*
*	Default constructor.
*/
TerrainEntity::TerrainEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Terrain;
}

/*
*	Returns the preprocessing parameters.
*/
void TerrainEntity::GetPreprocessingParameters(EntityPreprocessingParameters *const RESTRICT parameters) NOEXCEPT
{
	parameters->_ShouldPreprocess = true;
	parameters->_CanPreprocessOnMainThread = false;
}

/*
*	Preprocesses this entity.
*/
void TerrainEntity::Preprocess(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	//Cast to terrain initialization data.
	TerrainInitializationData *const RESTRICT terrain_initialization_data{ static_cast<TerrainInitializationData* const RESTRICT>(data) };

	//Calculate the world space axis aligned bounding box.
	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	axis_aligned_bounding_box._Minimum._X = -(static_cast<float32>(terrain_initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Minimum._Z = -(static_cast<float32>(terrain_initialization_data->_PatchSize) * 0.5f);

	axis_aligned_bounding_box._Maximum._X = (static_cast<float32>(terrain_initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Maximum._Z = (static_cast<float32>(terrain_initialization_data->_PatchSize) * 0.5f);

	for (uint32 Y{ 0 }; Y < terrain_initialization_data->_HeightMap.GetResolution(); ++Y)
	{
		for (uint32 X{ 0 }; X < terrain_initialization_data->_HeightMap.GetResolution(); ++X)
		{
			const float32 height{ terrain_initialization_data->_HeightMap.At(X, Y) };

			axis_aligned_bounding_box._Minimum._Y = CatalystBaseMath::Minimum<float32>(axis_aligned_bounding_box._Minimum._Y, height);
			axis_aligned_bounding_box._Maximum._Y = CatalystBaseMath::Maximum<float32>(axis_aligned_bounding_box._Maximum._Y, height);
		}
	}

	terrain_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(terrain_initialization_data->_WorldPosition.GetCell(), axis_aligned_bounding_box._Minimum);
	terrain_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(terrain_initialization_data->_WorldPosition.GetCell(), axis_aligned_bounding_box._Maximum);

	//Construct the plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	terrain_initialization_data->_BaseResolution,
													&vertices,
													&indices);

	StaticArray<void* RESTRICT, 2> buffer_data;

	buffer_data[0] = vertices.Data();
	buffer_data[1] = indices.Data();

	StaticArray<uint64, 2> buffer_data_sizes;

	buffer_data_sizes[0] = sizeof(TerrainVertex) * vertices.Size();
	buffer_data_sizes[1] = sizeof(uint32) * indices.Size();

	RenderingSystem::Instance->CreateBuffer(buffer_data_sizes[0] + buffer_data_sizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &terrain_initialization_data->_PreprocessedData._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(buffer_data.Data(), buffer_data_sizes.Data(), 2, &terrain_initialization_data->_PreprocessedData._Buffer);

	terrain_initialization_data->_PreprocessedData._IndexOffset = static_cast<uint32>(buffer_data_sizes[0]);
	terrain_initialization_data->_PreprocessedData._IndexCount = static_cast<uint32>(indices.Size());

	//Create the height map texture.
	{
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(terrain_initialization_data->_HeightMap), TextureFormat::R_FLOAT32, TextureUsage::NONE, false), &terrain_initialization_data->_PreprocessedData._HeightMapTexture);
		terrain_initialization_data->_PreprocessedData._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(terrain_initialization_data->_PreprocessedData._HeightMapTexture);
	}

	//Create the normal map texture.
	{
		Texture2D<Vector4<uint8>> converted_normal_map_texture{ terrain_initialization_data->_NormalMap.GetResolution() };

		for (uint32 Y{ 0 }; Y < converted_normal_map_texture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < converted_normal_map_texture.GetResolution(); ++X)
			{
				const Vector3<float32>& actual_normal{ terrain_initialization_data->_NormalMap.At(X, Y) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					converted_normal_map_texture.At(X, Y)[i] = static_cast<uint8>((actual_normal[i] * 0.5f + 0.5f) * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(converted_normal_map_texture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &terrain_initialization_data->_PreprocessedData._NormalMapTexture);
		terrain_initialization_data->_PreprocessedData._NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(terrain_initialization_data->_PreprocessedData._NormalMapTexture);
	}

	//Create the index map texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(terrain_initialization_data->_IndexMap), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &terrain_initialization_data->_PreprocessedData._IndexMapTexture);
	terrain_initialization_data->_PreprocessedData._IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(terrain_initialization_data->_PreprocessedData._IndexMapTexture);

	//Create the blend map texture.
	{
		Texture2D<Vector4<uint8>> converted_blend_map_texture{ terrain_initialization_data->_NormalMap.GetResolution() };

		for (uint32 Y{ 0 }; Y < converted_blend_map_texture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < converted_blend_map_texture.GetResolution(); ++X)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					converted_blend_map_texture.At(X, Y)[i] = static_cast<uint8>(terrain_initialization_data->_BlendMap.At(X, Y)[i] * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(converted_blend_map_texture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &terrain_initialization_data->_PreprocessedData._BlendMapTexture);
		terrain_initialization_data->_PreprocessedData._BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(terrain_initialization_data->_PreprocessedData._BlendMapTexture);
	}

	//Preprocess the entity physics.
	PhysicsSystem::Instance->PreprocessEntityPhysics(this, data);
}

/*
*	Initializes this entity.
*/
void TerrainEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewTerrainComponentsIndex(this);

	//Copy the data.
	TerrainInitializationData* const RESTRICT terrain_initialization_data{ static_cast<TerrainInitializationData* const RESTRICT>(data) };
	TerrainGeneralComponent& general_component{ ComponentManager::GetTerrainTerrainGeneralComponents()[_ComponentsIndex] };
	TerrainRenderComponent& render_component{ ComponentManager::GetTerrainTerrainRenderComponents()[_ComponentsIndex] };

	ASSERT(general_component._HeightMap.GetWidth() == general_component._HeightMap.GetHeight(), "Terrain height map width and height doesn't match - This isn't okay.");

	general_component._WorldPosition = terrain_initialization_data->_WorldPosition;
	general_component._WorldSpaceAxisAlignedBoundingBox = terrain_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	general_component._PatchSize = terrain_initialization_data->_PatchSize;
	general_component._HeightMap = std::move(terrain_initialization_data->_HeightMap);
	general_component._NormalMap = std::move(terrain_initialization_data->_NormalMap);
	general_component._IndexMap = std::move(terrain_initialization_data->_IndexMap);
	general_component._BlendMap = std::move(terrain_initialization_data->_BlendMap);
	general_component._BaseResolution = terrain_initialization_data->_BaseResolution;
	general_component._MaximumSubdivisionSteps = terrain_initialization_data->_MaximumSubdivisionSteps;

	render_component._Buffer = terrain_initialization_data->_PreprocessedData._Buffer;
	render_component._IndexOffset = terrain_initialization_data->_PreprocessedData._IndexOffset;
	render_component._IndexCount = terrain_initialization_data->_PreprocessedData._IndexCount;
	render_component._HeightMapTexture = terrain_initialization_data->_PreprocessedData._HeightMapTexture;
	render_component._HeightMapTextureIndex = terrain_initialization_data->_PreprocessedData._HeightMapTextureIndex;
	render_component._NormalMapTexture = terrain_initialization_data->_PreprocessedData._NormalMapTexture;
	render_component._NormalMapTextureIndex = terrain_initialization_data->_PreprocessedData._NormalMapTextureIndex;
	render_component._IndexMapTexture = terrain_initialization_data->_PreprocessedData._IndexMapTexture;
	render_component._IndexMapTextureIndex = terrain_initialization_data->_PreprocessedData._IndexMapTextureIndex;
	render_component._BlendMapTexture = terrain_initialization_data->_PreprocessedData._BlendMapTexture;
	render_component._BlendMapTextureIndex = terrain_initialization_data->_PreprocessedData._BlendMapTextureIndex;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<TerrainInitializationData>(data);

	//Initialize the entity physics.
	PhysicsSystem::Instance->InitializeEntityPhysics(this);
}

/*
*	Terminates this entity.
*/
void TerrainEntity::Terminate() NOEXCEPT
{
	//Terminate the entity physics.
	PhysicsSystem::Instance->TerminateEntityPhysics(this);

	TerrainRenderComponent& render_component{ ComponentManager::GetTerrainTerrainRenderComponents()[_ComponentsIndex] };

	RenderingSystem::Instance->DestroyBuffer(&render_component._Buffer);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._HeightMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._HeightMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._NormalMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._NormalMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._IndexMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._IndexMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._BlendMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._BlendMapTexture);

	//Return this entitiy's components index.
	ComponentManager::ReturnTerrainComponentsIndex(_ComponentsIndex);
}