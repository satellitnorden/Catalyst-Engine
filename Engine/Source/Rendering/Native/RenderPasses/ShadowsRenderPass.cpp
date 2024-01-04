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

	//Allocate the appropriate size for the shadow uniform data and set initial values.
	_ShadowUniformData.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	//Create the shadow uniform data buffers.
	_ShadowUniformDataBuffers.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &shadow_uniform_data_buffer : _ShadowUniformDataBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(ShadowUniformDataOld), BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &shadow_uniform_data_buffer);
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
	SetNumberOfPipelines(_ClearShadowMapPipelines.Size() + _OpaqueModelShadowMapPipelines.Size() + _MaskedModelShadowMapPipelines.Size() + 2 + _ShadowsSpatialDenoisingGraphicsPipelines.Size());

	//Add all pipelines.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_ClearShadowMapPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_OpaqueModelShadowMapPipelines[i]);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		AddPipeline(&_MaskedModelShadowMapPipelines[i]);
	}

	AddPipeline(&_ShadowMapResolvePipeline);
	AddPipeline(&_ShadowsRayTracingPipeline);

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	/*
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), _ShadowMapDepthBuffers[i]);
		parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), _ShadowMapRenderTargets[i]);

		_ClearShadowMapPipelines[i].Initialize(parameters);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), _ShadowMapDepthBuffers[i]);
		parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), _ShadowMapRenderTargets[i]);
		char buffer[64];
		sprintf_s(buffer, "OpaqueModelsShadowMap%u", static_cast<uint32>(i));
		parameters._InputStreamSubscriptions.Emplace(buffer);

		_OpaqueModelShadowMapPipelines[i].Initialize(parameters);
	}

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		GraphicsRenderPipelineParameters parameters;

		parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), _ShadowMapDepthBuffers[i]);
		parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), _ShadowMapRenderTargets[i]);
		char buffer[64];
		sprintf_s(buffer, "MaskedModelsShadowMap%u", static_cast<uint32>(i));
		parameters._InputStreamSubscriptions.Emplace(buffer);

		_MaskedModelShadowMapPipelines[i].Initialize(parameters);
	}
	*/

	_ShadowMapResolvePipeline.Initialize();
	_ShadowsRayTracingPipeline.Initialize();
	_ShadowsSpatialDenoisingGraphicsPipelines[0].Initialize(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1),
															1,
															RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2));
	_ShadowsSpatialDenoisingGraphicsPipelines[1].Initialize(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_2),
															1,
															RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
}

/*
*	Executes this render pass.
*/
void ShadowsRenderPass::Execute() NOEXCEPT
{	
	//Cache data that will be used.
	ShadowUniformDataOld &current_shadow_uniform_data{ _ShadowUniformData[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };
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
		//Cache the number of shadow map data.
		const uint32 number_of_shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData() };

		//Update the shadow uniform data.
		for (uint32 i{ 0 }; i < number_of_shadow_map_data; ++i)
		{
			const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(i) };
		
			current_shadow_uniform_data._WorldToLightMatrices[i] = shadow_map_data._WorldToLightMatrix;
			current_shadow_uniform_data._ShadowMapRenderTargetIndices[i] = shadow_map_data._RenderTargetIndex;
			current_shadow_uniform_data._ShadowMapCascadeDistances[i] = shadow_map_data._Distance;
		}

		for (uint32 i{ number_of_shadow_map_data }; i < 4; ++i)
		{
			current_shadow_uniform_data._WorldToLightMatrices[i] = Matrix4x4();
			current_shadow_uniform_data._ShadowMapRenderTargetIndices[i] = 0;
			current_shadow_uniform_data._ShadowMapCascadeDistances[i] = 0.0f;
		}

		//Upload the shadow uniform data.
		const void *const RESTRICT data_chunks[]{ &current_shadow_uniform_data };
		const uint64 data_sizes[]{ sizeof(ShadowUniformDataOld) };

		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_shadow_uniform_data_buffer);

		//Execute all shadow map rendering.
		for (uint32 i{ 0 }; i < number_of_shadow_map_data; ++i)
		{
			const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(i) };

			if (!_ShadowMapPipelinesIsInitialized[i])
			{
				{
					GraphicsRenderPipelineParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);

					_ClearShadowMapPipelines[i].Initialize(parameters);
				}

				{
					GraphicsRenderPipelineParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);
					char buffer[64];
					sprintf_s(buffer, "OpaqueModelsShadowMap%u", static_cast<uint32>(i));
					parameters._InputStreamSubscriptions.Emplace(buffer);

					_OpaqueModelShadowMapPipelines[i].Initialize(parameters);
				}

				{
					GraphicsRenderPipelineParameters parameters;

					parameters._DepthBuffer = Pair<HashString, DepthBufferHandle>(HashString("ShadowMapDepthBuffer"), shadow_map_data._DepthBuffer);
					parameters._OutputRenderTargets.Emplace(HashString("ShadowMap"), shadow_map_data._RenderTarget);
					char buffer[64];
					sprintf_s(buffer, "MaskedModelsShadowMap%u", static_cast<uint32>(i));
					parameters._InputStreamSubscriptions.Emplace(buffer);

					_MaskedModelShadowMapPipelines[i].Initialize(parameters);
				}

				_ShadowMapPipelinesIsInitialized[i] = true;
			}

			_ClearShadowMapPipelines[i].Execute();
			_OpaqueModelShadowMapPipelines[i].Execute();
			_MaskedModelShadowMapPipelines[i].Execute();
		}

		for (uint32 i{ number_of_shadow_map_data }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
		{
			if (_ShadowMapPipelinesIsInitialized[i])
			{
				_ClearShadowMapPipelines[i].Terminate();
				_ClearShadowMapPipelines[i].SetIncludeInRender(false);
				_OpaqueModelShadowMapPipelines[i].Terminate();
				_OpaqueModelShadowMapPipelines[i].SetIncludeInRender(false);
				_MaskedModelShadowMapPipelines[i].Terminate();
				_MaskedModelShadowMapPipelines[i].SetIncludeInRender(false);

				_ShadowMapPipelinesIsInitialized[i] = false;
			}
		}

		_ShadowMapResolvePipeline.Execute();
		_ShadowsRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_ClearShadowMapPipelines[i].SetIncludeInRender(false);
		}

		_ShadowMapResolvePipeline.SetIncludeInRender(false);
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
	_ShadowMapResolvePipeline.Terminate();
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
}