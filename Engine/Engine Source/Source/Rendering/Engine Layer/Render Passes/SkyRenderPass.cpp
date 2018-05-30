//Header file.
#include <Rendering/Engine Layer/Render Passes/SkyRenderPass.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(SkyRenderPass);

/*
*	Initializes the sky render pass.
*/
void SkyRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Sky);

	//Set the shaders.
	SetVertexShader(Shader::CubeMapVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::CubeMapFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(RenderTarget::Scene);
	AddRenderTarget(RenderTarget::WaterScene);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::Environment);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetRenderResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Back);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetDepthCompareOperator(CompareOperator::LessOrEqual);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::TriangleList);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		SkyRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the sky.
*/
void SkyRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the descriptor set2.
	StaticArray<DescriptorSetHandle, 2> descriptorSets
	{
		RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet(),
		RenderingSystem::Instance->GetCurrentEnvironmentDataDescriptorSet()
	};

	commandBuffer->BindDescriptorSets(this, 0, static_cast<uint32>(descriptorSets.Size()), descriptorSets.Data());

	//Draw!
	commandBuffer->Draw(this, 36, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}