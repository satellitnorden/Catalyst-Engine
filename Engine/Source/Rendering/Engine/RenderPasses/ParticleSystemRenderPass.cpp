//Header file.
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>

//Core.
#include <Core/General/Padding.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ParticleSystemRenderPass);

/*
*	Default constructor.
*/
ParticleSystemRenderPass::ParticleSystemRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		ParticleSystemRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the particle system render pass.
*/
void ParticleSystemRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the main stage.
	SetMainStage(RenderPassMainStage::ParticleSystem);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::ParticleSystem);

	//Set the shaders.
	SetVertexShader(Shader::ParticleSystemVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::ParticleSystemGeometry);
	SetFragmentShader(Shader::ParticleSystemFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::Scene);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Geometry, 0, 24);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::Zero);
	SetBlendFactorDestinationAlpha(BlendFactor::OneMinusSourceAlpha);
	SetCullMode(CullMode::Front);
	SetDepthCompareOperator(CompareOperator::Greater);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::PointList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		ParticleSystemRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the particle systems.
*/
void ParticleSystemRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all particle system components and draw them all.
	const uint64 numberOfParticleSystemComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to render - render none.
	if (numberOfParticleSystemComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const ParticleSystemRenderComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data table.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		struct ParticleSystemData
		{
			Vector3 _WorldPosition;
			Padding<4> _Padding;
			float _ParticleSystemRandomSeed;
			float _ParticleSystemTotalTime;
		} particleSystemData;

		particleSystemData._WorldPosition = component->_WorldPosition;
		particleSystemData._ParticleSystemRandomSeed = component->_ParticleSystemRandomSeed;
		particleSystemData._ParticleSystemTotalTime = EngineSystem::Instance->GetTotalTime() - component->_ParticleSystemStartingTime;

		commandBuffer->PushConstants(this, ShaderStage::Geometry, 0, sizeof(ParticleSystemData), &particleSystemData);
		commandBuffer->BindRenderDataTable(this, 1, component->_RenderDataTable);
		commandBuffer->Draw(this, 1, component->_InstanceCount);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}