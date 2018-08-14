//Header file.
#include <Rendering/Engine/RenderPasses/BloomVerticalBlurRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(BloomVerticalBlurRenderPass);

/*
*	Default constructor.
*/
BloomVerticalBlurRenderPass::BloomVerticalBlurRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		BloomVerticalBlurRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the bloom vertical blur render pass.
*/
void BloomVerticalBlurRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table.
	CreateRenderDataTable();

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::BloomVerticalBlur);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::GaussianBlurFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Bloom);

	//Add the descriptor set layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GaussianBlur));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(GaussianBlurData));

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
		BloomVerticalBlurRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();

	//Initialize the gaussian blur data.
	data.direction = Vector2(0.0f, 1.0f);
	data.inverseResolution = Vector2(	1.0f / static_cast<float>(RenderingSystem::Instance->GetResolution().width),
										1.0f / static_cast<float>(RenderingSystem::Instance->GetResolution().height));
}

/*
*	Creates the render data table.
*/
void BloomVerticalBlurRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GaussianBlur), &renderDataTable);

	RenderingSystem::Instance->UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::RenderTarget, RenderingSystem::Instance->GetRenderTarget(RenderTarget::BloomIntermediate)), renderDataTable);
}

/*
*	Renders the bloom vertical blur.
*/
void BloomVerticalBlurRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet());
	commandBuffer->BindRenderDataTable(this, 1, renderDataTable);

	//Push the constant data.
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(GaussianBlurData), &data);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}