//Header file.
#include <Rendering/Engine/RenderPasses/ScreenSpaceAmbientOcclusionRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ScreenSpaceAmbientOcclusionRenderPass);

/*
*	Default constructor.
*/
ScreenSpaceAmbientOcclusionRenderPass::ScreenSpaceAmbientOcclusionRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		ScreenSpaceAmbientOcclusionRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the screen space ambient occlusion render pass.
*/
void ScreenSpaceAmbientOcclusionRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the sub stage.
	SetSubStage(RenderPassSubStage::SceenSpaceAmbientOcclusion);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ScreenSpaceAmbientOcclusionFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::ScreenSpaceAmbientOcclusion);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ScreenSpaceAmbientOcclusion));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(Vector2));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

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
		ScreenSpaceAmbientOcclusionRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();

	//Set the noise scale.
	Resolution resolution{ RenderingSystem::Instance->GetResolution() };

	noiseScale.X = static_cast<float>(resolution.width) / 4.0f;
	noiseScale.Y = static_cast<float>(resolution.height) / 4.0f;
}

/*
*	Renders the screen space ambient occlusion.
*/
void ScreenSpaceAmbientOcclusionRenderPass::RenderInternal() NOEXCEPT
{
	//If screen space ambient occlusion is not enabled, don't render this render pass.
	if (!RenderingConfigurationManager::Instance->GetScreenSpaceAmbientOcclusionEnabled())
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet());
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRenderDataTable(RenderDataTable::ScreenSpaceAmbientOcclusion));

	//Push constants.
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(Vector2), &noiseScale);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}