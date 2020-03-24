//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The delta.
	Vector2<float> _Delta;

};

/*
*	Initializes this graphics pipeline.
*/
void ResampleGraphicsPipeline::Initialize(const RenderTargetHandle source, const RenderTargetHandle target, const Vector2<float> delta, const Resolution resolution, const bool blendEnabled) NOEXCEPT
{
	//Store the delta.
	_Delta = delta;

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable(source);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ResampleFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

	//Set the render resolution.
	SetRenderResolution(resolution);

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(blendEnabled);
	SetBlendFactorSourceColor(BlendFactor::One);
	SetBlendFactorDestinationColor(BlendFactor::One);
	SetBlendFactorSourceAlpha(BlendFactor::Zero);
	SetBlendFactorDestinationAlpha(BlendFactor::One);
	SetCullMode(CullMode::Back);
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
}

/*
*	Executes this graphics pipeline.
*/
void ResampleGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	PushConstantData data;

	data._Delta = _Delta;

	command_buffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void ResampleGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void ResampleGraphicsPipeline::CreateRenderDataTable(const RenderTargetHandle source) NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, source, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}