//Header file.
#include <Rendering/Engine Layer/Render Passes/LightingRenderPass.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LightingRenderPass);

/*
*	Initializes the lighting render pass.
*/
void LightingRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Lighting);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::LightingFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::Scene);
	AddRenderTarget(RenderTarget::WaterScene);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(3);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::Environment);
	AddDescriptorSetLayout(DescriptorSetLayout::Lighting);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetRenderResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Clear);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Back);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::DontCare);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::DontCare);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetTopology(Topology::TriangleFan);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LightingRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the lighting.
*/
void LightingRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const EventHandle currentDirectionalShadowEvent{ RenderingSystem::Instance->GetCurrentDirectionalShadowEvent() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	StaticArray<DescriptorSetHandle, 3> descriptorSets
	{
		RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet(),
		RenderingSystem::Instance->GetCurrentEnvironmentDataDescriptorSet(),
		RenderingSystem::Instance->GetLightingDescriptorSet()
	};

	commandBuffer->BindDescriptorSets(this, 0, 3, descriptorSets.Data());

	//Wait for the directional shadows to finish.
	commandBuffer->WaitForEvents(this, 1, &currentDirectionalShadowEvent);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}