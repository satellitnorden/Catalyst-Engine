//Header file.
#include <Rendering/Native/RenderPasses/ShadowsRenderPass.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ShadowsRenderPass);

//Shadow render pass constants.
namespace ShadowRenderPassConstants
{
	constexpr float32 SHADOW_MAP_VIEW_DISTANCE_FACTOR{ 1.0f };
	constexpr float32 SHADOW_MAP_CASCADE_DISTANCE_FACTOR{ 1.0f / 5.25f };
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
	//Cache the camera local_position.
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };

	//Set up the furstom corners.
	const Vector3<float32> lower_left_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.0f, 0.0f)) };
	const Vector3<float32> upper_left_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(0.0f, 1.0f)) };
	const Vector3<float32> lower_right_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(1.0f, 0.0f)) };
	const Vector3<float32> upper_right_direction{ RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(1.0f, 1.0f)) };

	StaticArray<Vector4<float32>, 8> frustum_corners;

	frustum_corners[0] = Vector4<float32>(camera_local_position + lower_left_direction * cascade_start, 1.0f);
	frustum_corners[1] = Vector4<float32>(camera_local_position + upper_left_direction * cascade_start, 1.0f);
	frustum_corners[2] = Vector4<float32>(camera_local_position + lower_right_direction * cascade_start, 1.0f);
	frustum_corners[3] = Vector4<float32>(camera_local_position + upper_right_direction * cascade_start, 1.0f);

	frustum_corners[4] = Vector4<float32>(camera_local_position + lower_left_direction * cascade_end, 1.0f);
	frustum_corners[5] = Vector4<float32>(camera_local_position + upper_left_direction * cascade_end, 1.0f);
	frustum_corners[6] = Vector4<float32>(camera_local_position + lower_right_direction * cascade_end, 1.0f);
	frustum_corners[7] = Vector4<float32>(camera_local_position + upper_right_direction * cascade_end, 1.0f);

	//Construct the light matrix.
	const Matrix4x4 light_matrix{ Matrix4x4::LookAt(VectorConstants::ZERO, light_direction, CatalystWorldCoordinateSpace::UP) };

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

	return projection_matrix * light_matrix;
}

/*
*	Default constructor.
*/
ShadowsRenderPass::ShadowsRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::SHADOWS, this);

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

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ShadowsRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ShadowsRenderPass::Initialize() NOEXCEPT
{
	//Reset this render pass.
	ResetRenderPass();

	//Create the shadow map depth buffers.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		RenderingSystem::Instance->CreateDepthBuffer(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), SampleCount::SAMPLE_COUNT_1, &_ShadowMapDepthBuffers[i]);
	}

	//Create the shadow map render targets.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		RenderingSystem::Instance->CreateRenderTarget(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), TextureFormat::R_FLOAT32, SampleCount::SAMPLE_COUNT_1, &_ShadowMapRenderTargets[i]);
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
	SetNumberOfPipelines(_ClearPipelines.Size() + _TerrainShadowMapGraphicsPipelines.Size() + _InstancedOpaqueModelShadowsGraphicsPipelines.Size() + _ModelShadowMapGraphicsPipelines.Size() + 2 + _ShadowsSpatialDenoisingGraphicsPipelines.Size());

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_ClearPipelines[i]);
	}

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
		GraphicsRenderPipelineParameters parameters;

		parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowDepthBuffer"), _ShadowMapDepthBuffers[i]);
		parameters._OutputRenderTargets.Emplace(HashString("Shadow"), _ShadowMapRenderTargets[i]);

		_ClearPipelines[i].Initialize(parameters);
	}

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
	_ShadowsSpatialDenoisingGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
															1,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_ShadowsSpatialDenoisingGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
															1,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
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
					current_shadow_uniform_data._WorldToLightMatrices[i] = CalculateCascadeMatrix(i, CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation), i == 0 ? RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetNearPlane() : shadow_map_distances[i - 1], shadow_map_distances[i]);
				}

				break;
			}
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			current_shadow_uniform_data._ShadowMapCascadeDistances[i] = shadow_map_distances[i];
		}

		//Upload the shadow uniform data.
		const void *const RESTRICT data_chunks[]{ &current_shadow_uniform_data };
		const uint64 data_sizes[]{ sizeof(ShadowUniformData) };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_shadow_uniform_data_buffer);

		//Render all cascades.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_ClearPipelines[i].Execute();
		}

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
			_ClearPipelines[i].SetIncludeInRender(false);
		}

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

/*
*	Terminates this render pass.
*/
void ShadowsRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	for (GraphicsRenderPipeline &pipeline : _ClearPipelines)
	{
		pipeline.Terminate();
	}

	for (TerrainShadowMapGraphicsPipeline &pipeline : _TerrainShadowMapGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (InstancedOpaqueModelShadowsGraphicsPipeline &pipeline : _InstancedOpaqueModelShadowsGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	for (ModelShadowMapGraphicsPipeline &pipeline : _ModelShadowMapGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	_RasterizedShadowsGraphicsPipeline.Terminate();
	_ShadowsRayTracingPipeline.Terminate();

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}

	//Destroy all shadow uniform data render data tables.
	for (RenderDataTableHandle &render_data_table : _ShadowUniformDataRenderDataTables)
	{
		RenderingSystem::Instance->DestroyRenderDataTable(&render_data_table);
	}

	_ShadowUniformDataRenderDataTables.Clear();

	//Destroy all shadow uniform data buffers.
	for (BufferHandle &buffer : _ShadowUniformDataBuffers)
	{
		RenderingSystem::Instance->DestroyBuffer(&buffer);
	}

	_ShadowUniformDataBuffers.Clear();

	//Clear the shadow uniform data.
	_ShadowUniformData.Clear();

	//Return the render target indices to the global render data.
	for (const uint32 index : _ShadowMapRenderTargetIndices)
	{
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(index);
	}

	//Destroy the shadow map render targets.
	for (RenderTargetHandle &render_target : _ShadowMapRenderTargets)
	{
		RenderingSystem::Instance->DestroyRenderTarget(&render_target);
	}

	//Destroy the shadow map depth buffers.
	for (DepthBufferHandle &depth_buffer : _ShadowMapDepthBuffers)
	{
		RenderingSystem::Instance->DestroyDepthBuffer(&depth_buffer);
	}
}