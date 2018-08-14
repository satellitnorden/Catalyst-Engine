//Header file.
#include <Rendering/Engine/RenderPasses/PostProcessingRenderPass.h>

//Managers.
#include <Managers/PostProcessingManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PostProcessingRenderPass);

/*
*	Default constructor.
*/
PostProcessingRenderPass::PostProcessingRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		PostProcessingRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the post processing render pass.
*/
void PostProcessingRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the sub stage.
	SetSubStage(RenderPassSubStage::PostProcessing);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::PostProcessingFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Screen);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::PostProcessing));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(Vector2));

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
		PostProcessingRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the post processing.
*/
void PostProcessingRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet());
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRenderDataTable(RenderDataTable::PostProcessing));

	//Pust constants.
	Vector2 postProcessingData{ PostProcessingManager::Instance->GetBlurStrength(), PostProcessingManager::Instance->GetSaturationStrength() };
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(Vector2), &postProcessingData);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}