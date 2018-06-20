//Header file.
#include <Rendering/Engine Layer/Render Passes/DirectionalShadowHorizontalBlurRenderPass.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalShadowHorizontalBlurRenderPass);

/*
*	Default constructor.
*/
DirectionalShadowHorizontalBlurRenderPass::DirectionalShadowHorizontalBlurRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalShadowHorizontalBlurRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional shadow horizontal blur render pass.
*/
void DirectionalShadowHorizontalBlurRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectionalShadowHorizontalBlur);

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
	AddRenderTarget(RenderTarget::DirectionalShadowIntermediate);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderDataTableLayout::DynamicUniformData);
	AddRenderDataTableLayout(RenderDataTableLayout::GaussianBlur);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(PushConstantRange::ShaderStage::Fragment, 0, sizeof(GaussianBlurData));

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
		DirectionalShadowHorizontalBlurRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();

	//Initialize the gaussian blur data.
	data.direction = Vector2(1.0f, 0.0f);
	data.inverseResolution = Vector2(	1.0f / static_cast<float>(RenderingSystem::Instance->GetResolution().width),
										1.0f / static_cast<float>(RenderingSystem::Instance->GetResolution().height));
}

/*
*	Renders the directional shadow horizontal blur.
*/
void DirectionalShadowHorizontalBlurRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	StaticArray<RenderDataTableHandle, 2> descriptorSets
	{
		RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet(),
		RenderingSystem::Instance->GetRenderDataTable(RenderDataTable::DirectionalShadowHorizontalBlur)
	};

	commandBuffer->BindRenderDataTables(this, 0, static_cast<uint32>(descriptorSets.Size()), descriptorSets.Data());

	//Push the constant data.
	commandBuffer->PushConstants(this, PushConstantRange::ShaderStage::Fragment, 0, sizeof(GaussianBlurData), &data);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}