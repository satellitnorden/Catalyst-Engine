//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ShadowsSpatialDenoisingGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Shadows spatial denoising fragment push constant data definition.
*/
class ShadowsSpatialDenoisingFragmentPushConstantData final
{

public:

	//The stride.
	int32 _Stride;

};

/*
*	Initializes this graphics pipeline.
*/
void ShadowsSpatialDenoisingGraphicsPipeline::Initialize(const RenderTargetHandle source, const int32 stride, const RenderTargetHandle target) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable(source);

	//Set the stride.
	_Stride = stride;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader"))->_Handle);
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("ShadowsSpatialDenoisingFragmentShader"))->_Handle);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(ShadowsSpatialDenoisingFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(1));

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Shadows Spatial Denoising");
#endif
}

/*
*	Executes this graphics pipeline.
*/
void ShadowsSpatialDenoisingGraphicsPipeline::Execute() NOEXCEPT
{
	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	ShadowsSpatialDenoisingFragmentPushConstantData data;

	data._Stride = _Stride;

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(ShadowsSpatialDenoisingFragmentPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void ShadowsSpatialDenoisingGraphicsPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

	//Destroy the render data table layout.
	RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
}

/*
*	Creates the render data table layout.
*/
void ShadowsSpatialDenoisingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void ShadowsSpatialDenoisingGraphicsPipeline::CreateRenderDataTable(const RenderTargetHandle source) NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, PostSceneFeaturesRenderPass::Instance->GetDepthMip(0), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, source, RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
}