//Header file.
#include <Rendering/Engine Layer/Render Passes/OceanRenderPass.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(OceanRenderPass);

/*
*	Default constructor.
*/
OceanRenderPass::OceanRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		OceanRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the ocean render pass.
*/
void OceanRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::Ocean);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::OceanFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(3);
	AddDescriptorSetLayout(RenderDataTableLayout::DynamicUniformData);
	AddDescriptorSetLayout(RenderDataTableLayout::Environment);
	AddDescriptorSetLayout(RenderDataTableLayout::Ocean);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetRenderResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
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
		OceanRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the ocean.
*/
void OceanRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	StaticArray<RenderDataTableHandle, 3> descriptorSets
	{
		RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet(),
		RenderingSystem::Instance->GetCurrentEnvironmentDataDescriptorSet(),
		RenderingSystem::Instance->GetCurrentOceanDescriptorSet()
	};

	commandBuffer->BindDescriptorSets(this, 0, static_cast<uint32>(descriptorSets.Size()), descriptorSets.Data());

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}