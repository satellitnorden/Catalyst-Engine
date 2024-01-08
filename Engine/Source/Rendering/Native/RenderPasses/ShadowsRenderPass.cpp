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
	AddPipeline(&_RayTracedShadowsPipeline);

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	_ShadowMapResolvePipeline.Initialize();

	{
		RayTracingRenderPipelineParameters parameters;

		parameters._ComputeRenderTargets.Emplace(HashString("Shadows"), RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));

		_RayTracedShadowsPipeline.Initialize(parameters);
	}

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
		_RayTracedShadowsPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED)
	{
		for (uint32 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
		{
			_ClearShadowMapPipelines[i].SetIncludeInRender(false);
			_OpaqueModelShadowMapPipelines[i].SetIncludeInRender(false);
			_MaskedModelShadowMapPipelines[i].SetIncludeInRender(false);
		}

		_ShadowMapResolvePipeline.SetIncludeInRender(false);
		_RayTracedShadowsPipeline.Execute();
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
	for (uint32 i{ 0 }; i < ShadowsSystem::MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA; ++i)
	{
		if (_ShadowMapPipelinesIsInitialized[i])
		{
			_ClearShadowMapPipelines[i].Terminate();
			_OpaqueModelShadowMapPipelines[i].Terminate();
			_MaskedModelShadowMapPipelines[i].Terminate();

			_ShadowMapPipelinesIsInitialized[i] = false;
		}
	}

	_ShadowMapResolvePipeline.Terminate();
	_RayTracedShadowsPipeline.Terminate();

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		pipeline.Terminate();
	}
}