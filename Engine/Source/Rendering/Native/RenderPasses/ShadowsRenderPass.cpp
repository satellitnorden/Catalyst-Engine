//Header file.
#include <Rendering/Native/RenderPasses/ShadowsRenderPass.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ShadowsRenderPass);

//Shadow render pass constants.
namespace ShadowRenderPassConstants
{
	constexpr float32 SHADOW_MAP_VIEW_DISTANCE_FACTOR{ 0.850f };
	constexpr float32 SHADOW_MAP_CASCADE_DISTANCE_FACTOR{ 1.0f / 9.00f };
	constexpr StaticArray<float32, 4> SHADOW_MAP_CASCADE_DISTANCE_FACTORS
	{
		1.0f * SHADOW_MAP_CASCADE_DISTANCE_FACTOR * SHADOW_MAP_CASCADE_DISTANCE_FACTOR * SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f * SHADOW_MAP_CASCADE_DISTANCE_FACTOR * SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f * SHADOW_MAP_CASCADE_DISTANCE_FACTOR,
		1.0f
	};
}
/*
*	Calculates a cascade matrix.
*/
FORCE_INLINE NO_DISCARD Matrix4x4 CalculateCascadeMatrix(const uint8 frustum_index, const Vector3<float32> &light_direction, const float32 cascade_start, const float32 cascade_end) NOEXCEPT
{
	//Define constants.
	constexpr float32 BOUNDING_BOX_EXPANSION{ 14.50f };

	//Calculate the perceiver matrix.
	const Vector3<float32> perceiver_local_position{ Perceiver::Instance->GetWorldTransform().GetLocalPosition() };
	const Vector3<float32> perceiver_forward_vector{ Perceiver::Instance->GetForwardVector() };
	const Vector3<float32> perceiver_up_vector{ Perceiver::Instance->GetUpVector() };

	const Matrix4x4 perceiver_matrix{ Matrix4x4::LookAt(perceiver_local_position, perceiver_local_position + perceiver_forward_vector, perceiver_up_vector) };

	//Calculate the inverse perceiver matrix.
	Matrix4x4 inverse_perceiver_matrix{ perceiver_matrix };
	inverse_perceiver_matrix.Inverse();

	//Calculate the field of view variables.
#if 0
	const float32 aspect_ratio{ static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Width) / static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Height) };
#else
	const float32 aspect_ratio{ static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Height) / static_cast<float32>(RenderingSystem::Instance->GetFullResolution()._Width) };
#endif
	const float32 tangent_half_horizontal_field_of_view{ CatalystBaseMath::Tangent(CatalystBaseMath::DegreesToRadians(Perceiver::Instance->GetFieldOfView() / 2.0f)) };
	const float32 tangent_half_vertical_field_of_view{ CatalystBaseMath::Tangent(CatalystBaseMath::DegreesToRadians((Perceiver::Instance->GetFieldOfView() * aspect_ratio) / 2.0f)) };
	
	//Set up the furstom corners.
	const float32 xn{ ((cascade_start) * tangent_half_horizontal_field_of_view) };
	const float32 xf{ ((cascade_end) * tangent_half_horizontal_field_of_view) };
	const float32 yn{ ((cascade_start) * tangent_half_vertical_field_of_view) };
	const float32 yf{ ((cascade_end) * tangent_half_vertical_field_of_view) };

	StaticArray<Vector4<float32>, 8> frustum_corners;

	frustum_corners[0] = Vector4<float32>(xn, yn, -(cascade_start), 1.0f);
	frustum_corners[1] = Vector4<float32>(-xn, yn, -(cascade_start), 1.0f);
	frustum_corners[2] = Vector4<float32>(xn, -yn, -(cascade_start), 1.0f);
	frustum_corners[3] = Vector4<float32>(-xn, -yn, -(cascade_start), 1.0f);

	frustum_corners[4] = Vector4<float32>(xf, yf, -(cascade_end), 1.0f);
	frustum_corners[5] = Vector4<float32>(-xf, yf, -(cascade_end), 1.0f);
	frustum_corners[6] = Vector4<float32>(xf, -yf, -(cascade_end), 1.0f);
	frustum_corners[7] = Vector4<float32>(-xf, -yf, -(cascade_end), 1.0f);

	//Calculate the center point.
	Vector3<float32> center_point{ 0.0f, 0.0f, 0.0f };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		Vector4<float32> world_space_corner{ inverse_perceiver_matrix * frustum_corners[i] };

		center_point += Vector3<float32>(world_space_corner._X, world_space_corner._Y, world_space_corner._Z);
	}

	center_point /= static_cast<float32>(8);

	//Construct the light matrix.
	const Matrix4x4 light_matrix{ Matrix4x4::LookAt(center_point, center_point + light_direction, CatalystWorldCoordinateSpace::UP) };

	//Calculate the light bounding box.
	float32 minX{ FLOAT32_MAXIMUM };
	float32 maxX{ -FLOAT32_MAXIMUM };
	float32 minY{ FLOAT32_MAXIMUM };
	float32 maxY{ -FLOAT32_MAXIMUM };
	float32 minZ{ FLOAT32_MAXIMUM };
	float32 maxZ{ -FLOAT32_MAXIMUM };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		const Vector4<float32> world_space_corner{ inverse_perceiver_matrix * frustum_corners[i] };
		const Vector4<float32> light_space_corner{ light_matrix * world_space_corner };

		minX = CatalystBaseMath::Minimum<float32>(minX, light_space_corner._X);
		maxX = CatalystBaseMath::Maximum<float32>(maxX, light_space_corner._X);
		minY = CatalystBaseMath::Minimum<float32>(minY, light_space_corner._Y);
		maxY = CatalystBaseMath::Maximum<float32>(maxY, light_space_corner._Y);
		minZ = CatalystBaseMath::Minimum<float32>(minY, light_space_corner._Z);
		maxZ = CatalystBaseMath::Maximum<float32>(maxY, light_space_corner._Z);
	}

	//Cache the view distance.
	const float32 view_distance{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance };

	//Calculate the projection matrix.
	Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(minX * BOUNDING_BOX_EXPANSION, maxX * BOUNDING_BOX_EXPANSION, minY * BOUNDING_BOX_EXPANSION, maxY * BOUNDING_BOX_EXPANSION, view_distance * -0.5f, view_distance * 0.5f) };
	//Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(minX * BOUNDING_BOX_EXPANSION, maxX * BOUNDING_BOX_EXPANSION, minY * BOUNDING_BOX_EXPANSION, maxY * BOUNDING_BOX_EXPANSION, minZ * BOUNDING_BOX_EXPANSION, maxZ * BOUNDING_BOX_EXPANSION) };
	//Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(minX * BOUNDING_BOX_EXPANSION, maxX * BOUNDING_BOX_EXPANSION, minY * BOUNDING_BOX_EXPANSION, maxY * BOUNDING_BOX_EXPANSION, minZ * BOUNDING_BOX_EXPANSION, view_distance * 0.5f) };

	return projection_matrix * light_matrix;
}

/*
*	Default constructor.
*/
ShadowsRenderPass::ShadowsRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(NativeRenderPassStage::SHADOWS);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ShadowsRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ShadowsRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void ShadowsRenderPass::Initialize() NOEXCEPT
{
	//Create the shadow map depth buffers.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		RenderingSystem::Instance->CreateDepthBuffer(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), &_ShadowMapDepthBuffers[i]);
	}

	//Create the shadow map render targets.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		RenderingSystem::Instance->CreateRenderTarget(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), TextureFormat::R_FLOAT32, &_ShadowMapRenderTargets[i]);
	}

	//Add the shadow map render targets to the global render data.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_ShadowMapRenderTargetIndices[i] = RenderingSystem::Instance->AddTextureToGlobalRenderData(_ShadowMapRenderTargets[i]);
	}

	//Allocate the appropriate size for the shadow uniform data and set initial values.
	_ShadowUniformData.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (ShadowUniformData &shadow_uniform_data : _ShadowUniformData)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			shadow_uniform_data._ShadowMapRenderTargetIndices[i] = _ShadowMapRenderTargetIndices[i];
		}
	}

	//Create the shadow uniform data buffers.
	_ShadowUniformDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &shadow_uniform_data_buffer : _ShadowUniformDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(ShadowUniformData), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &shadow_uniform_data_buffer);
	}

	//Create the shadow uniform data render data tables.
	_ShadowUniformDataRenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &shadow_uniform_data_render_data_table : _ShadowUniformDataRenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::SHADOW), &shadow_uniform_data_render_data_table);
	}

	//Bind all the shadow uniform data buffers to the render data tables.
	for (uint8 i{ 0 }; i < RenderingSystem::Instance->GetNumberOfFramebuffers(); ++i)
	{
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &_ShadowUniformDataRenderDataTables[i], _ShadowUniformDataBuffers[i]);
	}

	//Add the pipelines.
	SetNumberOfPipelines(_TerrainShadowMapGraphicsPipelines.Size() + _InstancedOpaqueModelShadowsGraphicsPipelines.Size() + _ModelShadowMapGraphicsPipelines.Size() + 2 + _ShadowsSpatialDenoisingGraphicsPipelines.Size());

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_TerrainShadowMapGraphicsPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		AddPipeline(&_InstancedOpaqueModelShadowsGraphicsPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_ModelShadowMapGraphicsPipelines[i]);
	}

	AddPipeline(&_RasterizedShadowsGraphicsPipeline);
	AddPipeline(&_ShadowsRayTracingPipeline);

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_TerrainShadowMapGraphicsPipelines[i].Initialize(_ShadowMapDepthBuffers[i], _ShadowMapRenderTargets[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_InstancedOpaqueModelShadowsGraphicsPipelines[i].Initialize(false, _ShadowMapDepthBuffers[i], _ShadowMapRenderTargets[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_InstancedOpaqueModelShadowsGraphicsPipelines[4 + i].Initialize(true, _ShadowMapDepthBuffers[i], _ShadowMapRenderTargets[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_ModelShadowMapGraphicsPipelines[i].Initialize(_ShadowMapDepthBuffers[i], _ShadowMapRenderTargets[i]);
	}

	_RasterizedShadowsGraphicsPipeline.Initialize();
	_ShadowsRayTracingPipeline.Initialize();
	_ShadowsSpatialDenoisingGraphicsPipelines[0].Initialize(CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_1_RENDER_TARGET_INDEX,
															1,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_ShadowsSpatialDenoisingGraphicsPipelines[1].Initialize(CatalystShaderConstants::INTERMEDIATE_RGBA_FLOAT32_HALF_2_RENDER_TARGET_INDEX,
															1,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void ShadowsRenderPass::Execute() NOEXCEPT
{	
	//Cache data that will be used.
	ShadowUniformData &current_shadow_uniform_data{ _ShadowUniformData[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };
	BufferHandle &current_shadow_uniform_data_buffer{ _ShadowUniformDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };
	RenderDataTableHandle &current_shadow_uniform_data_render_data_table{ _ShadowUniformDataRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	//Cache the view distance.
	const float32 view_distance{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance };

	//Calculate the shadow map distances.
	StaticArray<float32, 4> shadow_map_distances;

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		shadow_map_distances[i] = view_distance * ShadowRenderPassConstants::SHADOW_MAP_VIEW_DISTANCE_FACTOR * ShadowRenderPassConstants::SHADOW_MAP_CASCADE_DISTANCE_FACTORS[i];
	}

	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RASTERIZED)
	{
		//Calculate the world to light matrices.
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent * RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
		{
			if (component->_LightType == LightType::DIRECTIONAL)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					current_shadow_uniform_data._WorldToLightMatrices[i] = CalculateCascadeMatrix(i, component->_Direction, i == 0 ? Perceiver::Instance->GetNearPlane() : shadow_map_distances[i - 1], shadow_map_distances[i]);
				}

				break;
			}
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			current_shadow_uniform_data._ShadowMapCascadeDistancesSquared[i] = shadow_map_distances[i] * shadow_map_distances[i];
		}

		//Upload the shadow uniform data.
		const void *const RESTRICT data_chunks[]{ &current_shadow_uniform_data };
		const uint64 data_sizes[]{ sizeof(ShadowUniformData) };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_shadow_uniform_data_buffer);

		//Render all cascades.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_TerrainShadowMapGraphicsPipelines[i].Execute(current_shadow_uniform_data._WorldToLightMatrices[i]);
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_InstancedOpaqueModelShadowsGraphicsPipelines[i].Execute(i, current_shadow_uniform_data._WorldToLightMatrices[i]);
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_InstancedOpaqueModelShadowsGraphicsPipelines[4 + i].Execute(i, current_shadow_uniform_data._WorldToLightMatrices[i]);
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_ModelShadowMapGraphicsPipelines[i].Execute(current_shadow_uniform_data._WorldToLightMatrices[i]);
		}
		
		_RasterizedShadowsGraphicsPipeline.Execute(current_shadow_uniform_data_render_data_table);
		_ShadowsRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_TerrainShadowMapGraphicsPipelines[i].SetIncludeInRender(false);
		}

		for (uint8 i{ 0 }; i < 8; ++i)
		{
			_InstancedOpaqueModelShadowsGraphicsPipelines[i].SetIncludeInRender(false);
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_ModelShadowMapGraphicsPipelines[i].SetIncludeInRender(false);
		}

		_RasterizedShadowsGraphicsPipeline.SetIncludeInRender(false);
		_ShadowsRayTracingPipeline.Execute();
	}

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Execute();
	}

	//Enable this render pass.
	SetEnabled(true);
}