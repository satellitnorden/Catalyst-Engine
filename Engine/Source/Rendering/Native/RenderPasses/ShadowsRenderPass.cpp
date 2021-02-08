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
	constexpr StaticArray<float32, 4> SHADOW_MAP_CASCADE_DISTANCES
	{
		16.0f,
		64.0f,
		256.0f,
		1'024.0f
	};
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
		
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			shadow_uniform_data._ShadowMapCascadeDistancesSquared[i] = ShadowRenderPassConstants::SHADOW_MAP_CASCADE_DISTANCES[i] * ShadowRenderPassConstants::SHADOW_MAP_CASCADE_DISTANCES[i];
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
															2,
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
	//Define constants.
	

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
					current_shadow_uniform_data._WorldToLightMatrices[i] = RenderingUtilities::CalculateDirectionalLightMatrix(ShadowRenderPassConstants::SHADOW_MAP_CASCADE_DISTANCES[i] * 8.0f, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance, component->_Direction);
				}

				break;
			}
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