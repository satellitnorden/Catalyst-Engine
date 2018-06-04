//Header file.
#include <Rendering/Engine Layer/Render Passes/ParticleSystemRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ParticleSystemRenderPass);

//Particle system render pass constants.
namespace ParticleSystemRenderPassConstants
{
	constexpr uint32 MAXIMUM_NUMBER_OF_PARTICLES{ 16'384 };
}

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
	//Set the stage.
	SetStage(RenderPassStage::ParticleSystem);

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
	AddRenderTarget(RenderTarget::WaterScene);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::ParticleSystem);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(PushConstantRange::ShaderStage::Geometry, 0, sizeof(float));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetRenderResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Front);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::DontCare);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(false);
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
	const ParticleSystemRenderComponent *RESTRICT component{ ComponentManager::GetParticleSystemRenderComponents() };
	DescriptorSetHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindDescriptorSets(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	for (uint64 i = 0; i < numberOfParticleSystemComponents; ++i, ++component)
	{
		const float randomSeed{ CatalystMath::RandomFloatInRange(0.0f, 1.0f) };
		commandBuffer->PushConstants(this, PushConstantRange::ShaderStage::Geometry, 0, sizeof(float), &randomSeed);
		commandBuffer->BindDescriptorSets(this, 1, 1, &component->descriptorSet);
		commandBuffer->Draw(this, ParticleSystemRenderPassConstants::MAXIMUM_NUMBER_OF_PARTICLES, 1);
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}