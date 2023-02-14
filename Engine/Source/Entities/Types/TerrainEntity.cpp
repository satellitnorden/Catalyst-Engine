//Header file.
#include <Entities/Types/TerrainEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/TerrainInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>

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
*	Initializes this entity.
*/
void TerrainEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewTerrainComponentsIndex(this);

	//Copy the data.
	const TerrainInitializationData *const RESTRICT terrain_initialization_data{ static_cast<const TerrainInitializationData* const RESTRICT>(data) };
	TerrainGeneralComponent &general_component{ ComponentManager::GetTerrainTerrainGeneralComponents()[_ComponentsIndex] };
	TerrainRenderComponent &render_component{ ComponentManager::GetTerrainTerrainRenderComponents()[_ComponentsIndex] };

	ASSERT(general_component._HeightMap.GetWidth() == general_component._HeightMap.GetHeight(), "Terrain height map width and height doesn't match - This isn't okay.");

	general_component._WorldPosition = terrain_initialization_data->_WorldPosition;
	general_component._HeightMap = std::move(terrain_initialization_data->_HeightMap);
	general_component._NormalMap = std::move(terrain_initialization_data->_NormalMap);

	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	general_component._HeightMap.GetWidth(),
													&vertices,
													&indices);
	StaticArray<void* RESTRICT, 2> buffer_data;

	buffer_data[0] = vertices.Data();
	buffer_data[1] = indices.Data();

	StaticArray<uint64, 2> buffer_data_sizes;

	buffer_data_sizes[0] = sizeof(TerrainVertex) * vertices.Size();
	buffer_data_sizes[1] = sizeof(uint32) * indices.Size();

	RenderingSystem::Instance->CreateBuffer(buffer_data_sizes[0] + buffer_data_sizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &render_component._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(buffer_data.Data(), buffer_data_sizes.Data(), 2, &render_component._Buffer);

	render_component._IndexOffset = static_cast<uint32>(buffer_data_sizes[0]);
	render_component._IndexCount = static_cast<uint32>(indices.Size());

	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(general_component._HeightMap), TextureFormat::R_FLOAT32, TextureUsage::NONE), &render_component._HeightMapTexture);

	render_component._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(render_component._HeightMapTexture);

	Texture2D<Vector4<uint8>> converted_normal_map{ general_component._NormalMap.GetResolution() };

	for (uint32 Y{ 0 }; Y < converted_normal_map.GetResolution(); ++Y)
	{
		for (uint32 X{ 0 }; X < converted_normal_map.GetResolution(); ++X)
		{
			const Vector3<float32> &actual_normal{ general_component._NormalMap.At(X, Y) };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				converted_normal_map.At(X, Y)[i] = static_cast<uint8>((actual_normal[i] * 0.5f + 0.5f) * static_cast<float32>(UINT8_MAXIMUM));
			}
		}
	}

	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(converted_normal_map), TextureFormat::RGBA_UINT8, TextureUsage::NONE), &render_component._NormalMapTexture);

	render_component._NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(render_component._NormalMapTexture);

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<TerrainInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void TerrainEntity::Terminate() NOEXCEPT
{
	TerrainRenderComponent& render_component{ ComponentManager::GetTerrainTerrainRenderComponents()[_ComponentsIndex] };

	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._NormalMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._NormalMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(render_component._HeightMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&render_component._HeightMapTexture);
	RenderingSystem::Instance->DestroyBuffer(&render_component._Buffer);

	//Return this entitiy's components index.
	ComponentManager::ReturnTerrainComponentsIndex(_ComponentsIndex);
}