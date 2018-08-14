//Header file.
#include <Rendering/Engine/RenderPasses/PostProcessingBloomRenderPass.h>

//Managers.
#include <Managers/PostProcessingManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostProcessingBloomRenderPass);

/*
*	Default constructor.
*/
PostProcessingBloomRenderPass::PostProcessingBloomRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		PostProcessingBloomRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the post processing bloom render pass.
*/
void PostProcessingBloomRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the sub stage.
	SetSubStage(RenderPassSubStage::PostProcessingBloom);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::PostProcessingBloomFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::PostProcessingBloom));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(float));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::DontCare);
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
		PostProcessingBloomRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the post processing bloom.
*/
void PostProcessingBloomRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet());
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRenderDataTable(RenderDataTable::PostProcessingBloom));

	//Push constants.
	const float bloomStrength{ PostProcessingManager::Instance->GetBloomStrength() };
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(float), &bloomStrength);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}