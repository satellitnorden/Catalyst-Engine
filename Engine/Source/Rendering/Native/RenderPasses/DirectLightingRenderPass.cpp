//Header file.
#include <Rendering/Native/RenderPasses/DirectLightingRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingRenderPass);

/*
*	Default constructor.
*/
DirectLightingRenderPass::DirectLightingRenderPass() NOEXCEPT
{
	//Set the stage.
#if !defined(CATALYST_ENABLE_PATH_TRACING)
	SetStage(RenderPassStage::DirectLighting);
#endif

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingRenderPass::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingRenderPass::Instance->Execute();
	});
}

/*
*	Initializes this render pass.
*/
void DirectLightingRenderPass::Initialize() NOEXCEPT
{
	//Add the pipelines.
	SetNumberOfPipelines(_LightsPipelines.Size() * 4);

	for (LightPipelines &pipelines : _LightsPipelines)
	{
		AddPipeline(&pipelines._VisibilityRayTracingPipeline);

		for (VisibilityDenoisingGraphicsPipeline &denoisingPipeline : pipelines._VisibilityDenoisingGraphicsPipelines)
		{
			AddPipeline(&denoisingPipeline);
		}

		AddPipeline(&pipelines._DirectLightingApplicationGraphicsPipeline);
	}
	
	//Initialize all pipelines.
	for (int32 i{ 0 }; i < LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS; ++i)
	{
		_LightsPipelines[i]._VisibilityRayTracingPipeline.Initialize(i);

		_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[0].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Horizontal,
																				1.0f,
																				RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_1),
																				RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_2));
		_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[1].Initialize(VisibilityDenoisingGraphicsPipeline::Direction::Vertical,
																				1.0f,
																				RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_2),
																				RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_1));

		_LightsPipelines[i]._DirectLightingApplicationGraphicsPipeline.Initialize(i);
	}

	//Post-initialize all pipelines.
	for (Pipeline *const RESTRICT pipeline : GetPipelines())
	{
		pipeline->PostInitialize();
	}
}

/*
*	Executes this render pass.
*/
void DirectLightingRenderPass::Execute() NOEXCEPT
{	
	//Execute all pipelines.
	int32 numberOfActiveLights{ RenderingSystem::Instance->GetLightingSystem()->GetNumberOfActiveLights() };

	for (int32 i{ 0 }; i < LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS; ++i)
	{
		if (i < numberOfActiveLights)
		{
			_LightsPipelines[i]._VisibilityRayTracingPipeline.Execute();
			_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[0].Execute();
			_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[1].Execute();
			_LightsPipelines[i]._DirectLightingApplicationGraphicsPipeline.Execute();
		}

		else
		{
			_LightsPipelines[i]._VisibilityRayTracingPipeline.SetIncludeInRender(false);
			_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[0].SetIncludeInRender(false);
			_LightsPipelines[i]._VisibilityDenoisingGraphicsPipelines[1].SetIncludeInRender(false);
			_LightsPipelines[i]._DirectLightingApplicationGraphicsPipeline.SetIncludeInRender(false);
		}
	}
}