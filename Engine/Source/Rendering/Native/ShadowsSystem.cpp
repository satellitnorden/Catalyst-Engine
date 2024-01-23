//Header file.
#include <Rendering/Native/ShadowsSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Components.
#include <Components/Core/ComponentManager.h>
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//Shadows systems constants.
namespace ShadowsSystemConstants
{
	constexpr float32 DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR{ 1.0f / 4.00f };
	constexpr StaticArray<float32, 4> DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTORS
	{
		1.0f * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f * DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f
	};
}

/*
*	Shadow mapping header data class definition.
*/
class ALIGN(16) ShadowMappingHeaderData final
{

public:

	//The number of shadow map data.
	uint32 _NumberOfShadowMapData;

};

//Push constant data struct definitions.
struct OpaqueModelPushConstantData
{
	Matrix4x4 _ModelMatrix;
	uint32 _LightMatrixIndex;
};

struct MaskedModelPushConstantData
{
	Matrix4x4 _ModelMatrix;
	uint32 _LightMatrixIndex;
	uint32 _MaterialIndex;
};

struct CascadeInformation final
{
	Matrix4x4 _WorldToLightMatrix;
	float32 _DepthRange;
};

/*
*	Calculates a cascade matrix.
*/
FORCE_INLINE NO_DISCARD CascadeInformation CalculateCascadeInformation(const Vector3<float32> &light_direction, const float32 cascade_start, const float32 cascade_end) NOEXCEPT
{
	//Cache the camera local_position.
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };

	//Set up the furstom corners.
	const Vector3<float32> lower_left_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.0f, 0.0f)) };
	const Vector3<float32> upper_left_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.0f, 1.0f)) };
	const Vector3<float32> lower_right_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(1.0f, 0.0f)) };
	const Vector3<float32> upper_right_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(1.0f, 1.0f)) };
	const Vector3<float32> straight_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.5f, 0.5f)) };

	StaticArray<Vector4<float32>, 8> frustum_corners;

	frustum_corners[0] = Vector4<float32>(camera_local_position + lower_left_direction * cascade_start, 1.0f);
	frustum_corners[1] = Vector4<float32>(camera_local_position + upper_left_direction * cascade_start, 1.0f);
	frustum_corners[2] = Vector4<float32>(camera_local_position + lower_right_direction * cascade_start, 1.0f);
	frustum_corners[3] = Vector4<float32>(camera_local_position + upper_right_direction * cascade_start, 1.0f);

	frustum_corners[4] = Vector4<float32>(camera_local_position + lower_left_direction * cascade_end, 1.0f);
	frustum_corners[5] = Vector4<float32>(camera_local_position + upper_left_direction * cascade_end, 1.0f);
	frustum_corners[6] = Vector4<float32>(camera_local_position + lower_right_direction * cascade_end, 1.0f);
	frustum_corners[7] = Vector4<float32>(camera_local_position + upper_right_direction * cascade_end, 1.0f);

	const Vector3<float32> center_position{ camera_local_position + straight_direction * CatalystBaseMath::LinearlyInterpolate(cascade_start, cascade_end, 0.5f) };

	//Construct the light matrix.
	const Matrix4x4 light_matrix{ Matrix4x4::LookAt(center_position, center_position + light_direction, CatalystWorldCoordinateSpace::UP) };

	//Calculate the light bounding box.
	float32 minX{ FLOAT32_MAXIMUM };
	float32 maxX{ -FLOAT32_MAXIMUM };
	float32 minY{ FLOAT32_MAXIMUM };
	float32 maxY{ -FLOAT32_MAXIMUM };
	float32 minZ{ FLOAT32_MAXIMUM };
	float32 maxZ{ -FLOAT32_MAXIMUM };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		const Vector4<float32> light_space_corner{ light_matrix * frustum_corners[i] };

		minX = CatalystBaseMath::Minimum<float32>(minX, light_space_corner._X);
		maxX = CatalystBaseMath::Maximum<float32>(maxX, light_space_corner._X);
		minY = CatalystBaseMath::Minimum<float32>(minY, light_space_corner._Y);
		maxY = CatalystBaseMath::Maximum<float32>(maxY, light_space_corner._Y);
		minZ = CatalystBaseMath::Minimum<float32>(minZ, light_space_corner._Z);
		maxZ = CatalystBaseMath::Maximum<float32>(maxZ, light_space_corner._Z);
	}

	//Cache the view distance.
	const float32 view_distance{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance };

	//Calculate the projection matrix.
	const Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(minX, maxX, minY, maxY, minZ, maxZ) };

	//Set up the information.
	CascadeInformation information;

	information._WorldToLightMatrix = projection_matrix * light_matrix;
	information._DepthRange = maxZ - minZ;

	return information;
}

/*
*	Post-initializes the shadows system.
*/
void ShadowsSystem::PostInitialize() NOEXCEPT
{
	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("ShadowMapping"),
		sizeof(ShadowMappingHeaderData) + sizeof(ShadowMapData) * MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA,
		[](DynamicArray<byte> *const RESTRICT data, void *const RESTRICT arguments)
		{
			data->Clear();
			ShadowsSystem *const RESTRICT shadows_system{ static_cast<ShadowsSystem *const RESTRICT>(arguments) };

			ShadowMappingHeaderData header_data;

			header_data._NumberOfShadowMapData = shadows_system->GetNumberOfShadowMapData();

			for (uint64 i{ 0 }; i < sizeof(ShadowMappingHeaderData); ++i)
			{
				data->Emplace(((const byte *const RESTRICT)&header_data)[i]);
			}

			for (const ShadowMapData &shadow_map_data : shadows_system->_ShadowMapData)
			{
				//Copy the world to light matrix.
				for (uint64 i{ 0 }; i < sizeof(Matrix4x4); ++i)
				{
					data->Emplace(((const byte *const RESTRICT)&shadow_map_data._WorldToLightMatrix)[i]);
				}

				for (uint64 i{ 0 }; i < sizeof(Vector3<float32>); ++i)
				{
					data->Emplace(((const byte *const RESTRICT)&shadow_map_data._Direction)[i]);
				}

				for (uint64 i{ 0 }; i < sizeof(uint32); ++i)
				{
					data->Emplace(((const byte *const RESTRICT)&shadow_map_data._RenderTargetIndex)[i]);
				}

				{
					/*
					*	This would work great if all the geometry was perfectly planar, but alas, it is not.
					*	So increase it by some magic amount that seems to work fine. (:
					*/
					const float32 maximum_depth_bias{ shadow_map_data._DepthRange / static_cast<float32>(UINT16_MAXIMUM) };

					for (uint64 i{ 0 }; i < sizeof(float32); ++i)
					{
						data->Emplace(((const byte *const RESTRICT)&maximum_depth_bias)[i]);
					}
				}

				for (uint64 i{ 0 }; i < 12; ++i)
				{
					data->Emplace(0);
				}
			}
		},
		this
	);

	//Register input streams.
	{
		DynamicArray<VertexInputAttributeDescription> models_required_vertex_input_attribute_descriptions;

		models_required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));

		DynamicArray<VertexInputBindingDescription> models_required_vertex_input_binding_descriptions;

		models_required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

		#define REGISTER_INPUT_STREAM(INDEX)																				\
		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream												\
		(																													\
			HashString("OpaqueModelsShadowMap" #INDEX),																		\
			models_required_vertex_input_attribute_descriptions,															\
			models_required_vertex_input_binding_descriptions,																\
			sizeof(OpaqueModelPushConstantData),																			\
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)								\
			{																												\
				static_cast<ShadowsSystem *const RESTRICT>(user_data)->GatherOpaqueModelInputStream(INDEX, input_stream);	\
			},																												\
			RenderInputStream::Mode::DRAW_INDEXED,																			\
			this																											\
		);
		REGISTER_INPUT_STREAM(0);
		REGISTER_INPUT_STREAM(1);
		REGISTER_INPUT_STREAM(2);
		REGISTER_INPUT_STREAM(3);
		REGISTER_INPUT_STREAM(4);
		REGISTER_INPUT_STREAM(5);
		REGISTER_INPUT_STREAM(6);
		REGISTER_INPUT_STREAM(7);
		REGISTER_INPUT_STREAM(8);
		REGISTER_INPUT_STREAM(9);
		REGISTER_INPUT_STREAM(10);
		REGISTER_INPUT_STREAM(11);
		REGISTER_INPUT_STREAM(12);
		REGISTER_INPUT_STREAM(13);
		REGISTER_INPUT_STREAM(14);
		REGISTER_INPUT_STREAM(15);
		REGISTER_INPUT_STREAM(16);
		REGISTER_INPUT_STREAM(17);
		REGISTER_INPUT_STREAM(18);
		REGISTER_INPUT_STREAM(19);
		REGISTER_INPUT_STREAM(20);
		REGISTER_INPUT_STREAM(21);
		REGISTER_INPUT_STREAM(22);
		REGISTER_INPUT_STREAM(23);
		#undef REGISTER_INPUT_STREAM
	}

	{
		DynamicArray<VertexInputAttributeDescription> models_required_vertex_input_attribute_descriptions;

		models_required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
		models_required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));

		DynamicArray<VertexInputBindingDescription> models_required_vertex_input_binding_descriptions;

		models_required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

		#define REGISTER_INPUT_STREAM(INDEX)																				\
		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream												\
		(																													\
			HashString("MaskedModelsShadowMap" #INDEX),																		\
			models_required_vertex_input_attribute_descriptions,															\
			models_required_vertex_input_binding_descriptions,																\
			sizeof(MaskedModelPushConstantData),																			\
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)								\
			{																												\
				static_cast<ShadowsSystem *const RESTRICT>(user_data)->GatherMaskedModelInputStream(INDEX, input_stream);	\
			},																												\
			RenderInputStream::Mode::DRAW_INDEXED,																			\
			this																											\
		);
		REGISTER_INPUT_STREAM(0);
		REGISTER_INPUT_STREAM(1);
		REGISTER_INPUT_STREAM(2);
		REGISTER_INPUT_STREAM(3);
		REGISTER_INPUT_STREAM(4);
		REGISTER_INPUT_STREAM(5);
		REGISTER_INPUT_STREAM(6);
		REGISTER_INPUT_STREAM(7);
		REGISTER_INPUT_STREAM(8);
		REGISTER_INPUT_STREAM(9);
		REGISTER_INPUT_STREAM(10);
		REGISTER_INPUT_STREAM(11);
		REGISTER_INPUT_STREAM(12);
		REGISTER_INPUT_STREAM(13);
		REGISTER_INPUT_STREAM(14);
		REGISTER_INPUT_STREAM(15);
		REGISTER_INPUT_STREAM(16);
		REGISTER_INPUT_STREAM(17);
		REGISTER_INPUT_STREAM(18);
		REGISTER_INPUT_STREAM(19);
		REGISTER_INPUT_STREAM(20);
		REGISTER_INPUT_STREAM(21);
		REGISTER_INPUT_STREAM(22);
		REGISTER_INPUT_STREAM(23);
#undef REGISTER_INPUT_STREAM
	}
}

/*
*	Updates the shadows system during the pre render update phase.
*/
void ShadowsSystem::PreRenderUpdate() NOEXCEPT
{
	//Update the shadow map data.
	uint32 current_shadow_map_data_index{ 0 };
	const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
	const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

	for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
	{
		if (component->_LightType == LightType::DIRECTIONAL)
		{
			Vector4<float32> cascade_distances;

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				cascade_distances[i] = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance * ShadowsSystemConstants::DIRECTIONAL_LIGHT_SHADOW_MAP_CASCADE_DISTANCE_FACTORS[i];
			}

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				ShadowMapData *RESTRICT shadow_map_data;

				if (current_shadow_map_data_index >= _ShadowMapData.Size())
				{
					_ShadowMapData.Emplace();
					shadow_map_data = &_ShadowMapData[current_shadow_map_data_index];

					RenderingSystem::Instance->CreateDepthBuffer(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), TextureFormat::D_UINT16, SampleCount::SAMPLE_COUNT_1, &shadow_map_data->_DepthBuffer);
					RenderingSystem::Instance->CreateRenderTarget(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), TextureFormat::R_UINT16, SampleCount::SAMPLE_COUNT_1, &shadow_map_data->_RenderTarget);
					shadow_map_data->_RenderTargetIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(shadow_map_data->_RenderTarget);
				}

				else
				{
					shadow_map_data = &_ShadowMapData[current_shadow_map_data_index];
				}

				const CascadeInformation cascade_information{ CalculateCascadeInformation(CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation), i == 0 ? RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetNearPlane() : cascade_distances[i - 1], cascade_distances[i]) };

				shadow_map_data->_WorldToLightMatrix = cascade_information._WorldToLightMatrix;
				shadow_map_data->_DepthRange = cascade_information._DepthRange;

				{
					//Construct the frustum planes.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[0][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] + shadow_map_data->_WorldToLightMatrix._Matrix[j][0]; //Left.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[1][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] - shadow_map_data->_WorldToLightMatrix._Matrix[j][0]; //Right.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[2][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] + shadow_map_data->_WorldToLightMatrix._Matrix[j][1]; //Bottom.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[3][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] - shadow_map_data->_WorldToLightMatrix._Matrix[j][1]; //Top.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[4][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] + shadow_map_data->_WorldToLightMatrix._Matrix[j][2]; //Near.
					for (uint8 j{ 4 }; j--;) shadow_map_data->_Frustum._Planes[5][j] = shadow_map_data->_WorldToLightMatrix._Matrix[j][3] - shadow_map_data->_WorldToLightMatrix._Matrix[j][2]; //Far.

					//Normalize the frustum planes.
					for (uint8 j{ 0 }; j < 6; ++j)
					{
						const float32 length_reciprocal{ CatalystBaseMath::InverseSquareRoot(shadow_map_data->_Frustum._Planes[j]._X * shadow_map_data->_Frustum._Planes[j]._X + shadow_map_data->_Frustum._Planes[j]._Y * shadow_map_data->_Frustum._Planes[j]._Y + shadow_map_data->_Frustum._Planes[j]._Z * shadow_map_data->_Frustum._Planes[j]._Z) };

						shadow_map_data->_Frustum._Planes[j]._X *= length_reciprocal;
						shadow_map_data->_Frustum._Planes[j]._Y *= length_reciprocal;
						shadow_map_data->_Frustum._Planes[j]._Z *= length_reciprocal;
						shadow_map_data->_Frustum._Planes[j]._W *= length_reciprocal;
					}
				}

				shadow_map_data->_Distance = cascade_distances[i];
				shadow_map_data->_Direction = CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation);

				++current_shadow_map_data_index;
			}
		}
	}

	//Destroy old shadow map data.
	while (current_shadow_map_data_index < _ShadowMapData.Size())
	{
		RenderingSystem::Instance->DestroyDepthBuffer(&_ShadowMapData.Back()._DepthBuffer);
		RenderingSystem::Instance->DestroyRenderTarget(&_ShadowMapData.Back()._RenderTarget);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_ShadowMapData.Back()._RenderTargetIndex);

		_ShadowMapData.Pop();
	}
}

/*
*	Gathers an opaque model input stream.
*/
void ShadowsSystem::GatherOpaqueModelInputStream
(
	const uint8 shadow_map_index,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Skip if doesn't exist.
	if (shadow_map_index >= _ShadowMapData.Size())
	{
		return;
	}

	//Cache the required visibility flag.
	const VisibilityFlags required_visibility_flag{ VisibilityFlags::SHADOW_MAP_START << shadow_map_index };

	//Gather static models.
	{
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < StaticModelComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			const EntityIdentifier entity_identifier{ StaticModelComponent::Instance->InstanceToEntity(instance_index) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity_identifier) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity_identifier) };

			//Skip this model if it's not visible.
			if (!TEST_BIT(static_model_instance_data._VisibilityFlags, required_visibility_flag))
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ static_model_instance_data._ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (static_model_instance_data._MaterialResources[i]->_Type != MaterialResource::Type::OPAQUE)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ static_model_instance_data._ModelResource->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				OpaqueModelPushConstantData push_constant_data;

				push_constant_data._ModelMatrix = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._LightMatrixIndex = shadow_map_index;

				for (uint64 i{ 0 }; i < sizeof(OpaqueModelPushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}

/*
*	Gathers a masked model input stream.
*/
void ShadowsSystem::GatherMaskedModelInputStream
(
	const uint8 shadow_map_index,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	//Set up the entry proxies.
	struct EntryProxy final
	{
		RenderInputStreamEntry _Entry;
		MaskedModelPushConstantData _PushConstantData;
		float32 _Distance;
	};

	static thread_local DynamicArray<EntryProxy> entry_proxies;

	entry_proxies.Clear();

	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Skip if doesn't exist.
	if (shadow_map_index >= _ShadowMapData.Size())
	{
		return;
	}

	//Cache the required visibility flag.
	const VisibilityFlags required_visibility_flag{ VisibilityFlags::SHADOW_MAP_START << shadow_map_index };

	//Gather static models.
	{
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < StaticModelComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			const EntityIdentifier entity_identifier{ StaticModelComponent::Instance->InstanceToEntity(instance_index) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity_identifier) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity_identifier) };

			//Skip this model if it's not visible.
			if (!TEST_BIT(static_model_instance_data._VisibilityFlags, required_visibility_flag))
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ static_model_instance_data._ModelResource->_Meshes.Size() }; i < size; ++i)
			{
				//Skip this mesh depending on the material type.
				if (static_model_instance_data._MaterialResources[i]->_Type != MaterialResource::Type::MASKED)
				{
					continue;
				}

				//Cache the mesh.
				const Mesh &mesh{ static_model_instance_data._ModelResource->_Meshes[i] };

				//Add a new entry.
				entry_proxies.Emplace();
				EntryProxy &new_entry{ entry_proxies.Back() };

				new_entry._Entry._PushConstantDataOffset = 0;
				new_entry._Entry._VertexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._Entry._IndexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._Entry._IndexBufferOffset = 0;
				new_entry._Entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._Entry._VertexCount = 0;
				new_entry._Entry._IndexCount = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexCount;
				new_entry._Entry._InstanceCount = 0;

				//Set up the push constant data.
				new_entry._PushConstantData._ModelMatrix = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				new_entry._PushConstantData._LightMatrixIndex = shadow_map_index;
				new_entry._PushConstantData._MaterialIndex = static_model_instance_data._MaterialResources[i]->_Index;

				//Calculate the distance.
				const Vector4<float32> clip_position{ _ShadowMapData[shadow_map_index]._WorldToLightMatrix * new_entry._PushConstantData._ModelMatrix * Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f) };
				new_entry._Distance = clip_position._Z;
			}
		}
	}

	//Perform a lazy sort on the entries.
	if (entry_proxies.Size() > 1)
	{
		SortingAlgorithms::StandardSort<EntryProxy>
		(
			entry_proxies.Begin(),
			entry_proxies.End(),
			nullptr,
			[](const void *const RESTRICT user_data, const EntryProxy *const RESTRICT first, const EntryProxy *const RESTRICT second)
			{
				return first->_Distance < second->_Distance;
			}
		);
	}

	//Actually add the entries.
	for (const EntryProxy &entry_proxy : entry_proxies)
	{
		input_stream->_Entries.Emplace(entry_proxy._Entry);
		input_stream->_Entries.Back()._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();

		for (uint64 i{ 0 }; i < sizeof(MaskedModelPushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&entry_proxy._PushConstantData)[i]);
		}
	}
}