//Header file.
#include <Rendering/Native/RenderPasses/ShadowsRenderPass.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

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
	//Set the stage.
	SetStage(RenderPassStage::SHADOWS);

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
	//Create the shadow map depth buffer.
	RenderingSystem::Instance->CreateDepthBuffer(Resolution(CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution, CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ShadowMapResolution), &_ShadowMapDepthBuffer);

	//Add the pipelines.
	SetNumberOfPipelines(3 + _ShadowsSpatialDenoisingGraphicsPipelines.Size());
	AddPipeline(&_ModelShadowMapGraphicsPipeline);
	AddPipeline(&_RasterizedShadowsGraphicsPipeline);
	AddPipeline(&_ShadowsRayTracingPipeline);

	for (ShadowsSpatialDenoisingGraphicsPipeline &pipeline : _ShadowsSpatialDenoisingGraphicsPipelines)
	{
		AddPipeline(&pipeline);
	}

	//Initialize all pipelines.
	_ModelShadowMapGraphicsPipeline.Initialize(_ShadowMapDepthBuffer);
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
	constexpr float32 SHADOW_MAP_COVERAGE{ 64.0f };

	//Execute all pipelines.
	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::NONE)
	{
		SetEnabled(false);

		return;
	}

	if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RASTERIZED)
	{
		//Calculate the world to light matrix. Only include the directional light, for now.
		Matrix4x4 world_to_light_matrix{ MatrixConstants::IDENTITY };

		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent * RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_light_components; ++i, ++component)
		{
			if (component->_LightType == static_cast<uint32>(LightType::DIRECTIONAL))
			{
				const float32 view_distance{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance };

				const Matrix4x4 light_matrix{ Matrix4x4::LookAt(Perceiver::Instance->GetWorldTransform().GetLocalPosition() + -component->_Direction * view_distance + Perceiver::Instance->GetForwardVector() * SHADOW_MAP_COVERAGE, Perceiver::Instance->GetWorldTransform().GetLocalPosition() + component->_Direction * view_distance + Perceiver::Instance->GetForwardVector() * SHADOW_MAP_COVERAGE, CatalystWorldCoordinateSpace::UP) };
				const Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(-SHADOW_MAP_COVERAGE, SHADOW_MAP_COVERAGE, -SHADOW_MAP_COVERAGE, SHADOW_MAP_COVERAGE, 0.0f, view_distance * 2.0f) };

				world_to_light_matrix = projection_matrix * light_matrix;

				break;
			}
		}

		_ModelShadowMapGraphicsPipeline.Execute(world_to_light_matrix);
		_RasterizedShadowsGraphicsPipeline.Execute(world_to_light_matrix);
		_ShadowsRayTracingPipeline.SetIncludeInRender(false);
	}

	else if (RenderingSystem::Instance->GetRenderingConfiguration()->GetSurfaceShadowsMode() == RenderingConfiguration::SurfaceShadowsMode::RAY_TRACED)
	{
		_ModelShadowMapGraphicsPipeline.SetIncludeInRender(false);
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