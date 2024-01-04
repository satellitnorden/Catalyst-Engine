//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldBokehBlurGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/PostSceneFeaturesRenderPass.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Depth of field bokeh blur fragment push constant data definition.
*/
class DepthOfFieldBokehBlurFragmentPushConstantData final
{

public:

	//The depth of field focus distance.
	float32 _DepthOfFieldFocusDistance;

	//The depth of field size.
	float32 _DepthOfFieldSize;

};

/*
*	Initializes this graphics pipeline.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::Initialize() NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader"))->_Handle);
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("DepthOfFieldBokehBlurFragmentShader"))->_Handle);

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::INTERMEDIATE_RGBA_FLOAT32_QUARTER_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(DepthOfFieldBokehBlurFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(2));

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
	SetName("Depth Of Field Bokeh Blur");
#endif

	//Finalize the pipeline.
	FinalizePipeline();
}

/*
*	Executes this graphics pipeline.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::Execute() NOEXCEPT
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
	DepthOfFieldBokehBlurFragmentPushConstantData data;

	data._DepthOfFieldFocusDistance = RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFocalDistance();
	data._DepthOfFieldSize = RenderingUtilities::CalculateDepthOfFieldSize(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetAperture());

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(DepthOfFieldBokehBlurFragmentPushConstantData), &data);

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
void DepthOfFieldBokehBlurGraphicsPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

	//Destroy the render data table layout.
	RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
}

/*
*	Creates the render data table layout.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
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
void DepthOfFieldBokehBlurGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, PostSceneFeaturesRenderPass::Instance->GetDepthMip(1), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}