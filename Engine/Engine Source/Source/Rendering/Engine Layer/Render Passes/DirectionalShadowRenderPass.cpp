//Header file.
#include <Rendering/Engine Layer/Render Passes/DirectionalShadowRenderPass.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalShadowRenderPass);

/*
*	Initializes the directional shadow render pass.
*/
void DirectionalShadowRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectionalShadow);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DirectionalShadowFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalPreBlurShadowMap);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::DirectionalShadow);

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
		DirectionalShadowRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the directional shadow.
*/
void DirectionalShadowRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const EventHandle currentDirectionalShadowEvent{ RenderingSystem::Instance->GetCurrentDirectionalShadowEvent() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	StaticArray<DescriptorSetHandle, 2> descriptorSets
	{
		RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet(),
		RenderingSystem::Instance->GetDirectionalShadowDescriptorSet()
	};

	commandBuffer->BindDescriptorSets(this, 0, static_cast<uint32>(descriptorSets.Size()), descriptorSets.Data());

	//Wait for the directional shadows to finish.
	commandBuffer->WaitForEvents(this, 1, &currentDirectionalShadowEvent);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}